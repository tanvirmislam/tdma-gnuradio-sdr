/* -*- c++ -*- */
/* 
 * Copyright 2019 gr-IEEE_802_15_4_CustomBlocks author.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "MAC_Sender_impl.h"


namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        MAC_Sender::sptr
        MAC_Sender::make(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr, int coord_pan, int coord_addr)
        {
            return gnuradio::get_initial_sptr (new MAC_Sender_impl(fcf, seq_nr, dst_pan, dst_addr, src_pan, src_addr, coord_pan, coord_addr));
        }

        /*
        * The private constructor
        */
        MAC_Sender_impl::MAC_Sender_impl(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr, int coord_pan, int coord_addr)
            :   gr::block(  "MAC_Sender",
                            gr::io_signature::make(0, 0, 0),
                            gr::io_signature::make(0, 0, 0)),
                d_fcf(fcf),
                d_seq_nr(seq_nr),
                d_dst_pan(dst_pan),
                d_dst_addr(dst_addr),
                d_src_pan(src_pan),
                d_src_addr(src_addr),
                d_coord_pan(coord_pan),
                d_coord_addr(coord_addr),
                d_msg_offset(0),
                d_num_packet_errors(0),
                d_num_packets_received(0),
                //d_assigned_slot(0),
                d_is_assigned(false)
        {
            // In port 1
            message_port_register_in(pmt::mp("message in"));
            set_msg_handler(pmt::mp("message in"), boost::bind(&MAC_Sender_impl::msg_in, this, _1));

            // In port 1
            message_port_register_in(pmt::mp("signal in"));
            set_msg_handler(pmt::mp("signal in"), boost::bind(&MAC_Sender_impl::signal_in, this, _1));

            // Out port 1
            message_port_register_out(pmt::mp("mac out"));
        }

        /*
        * Our virtual destructor.
        */
        MAC_Sender_impl::~MAC_Sender_impl()
        {

        }

        /*
            Input port receives the signal

            It has to check the signal's destination address
            The destination address has to match the sender's address for it to proceed

            Signal can be the beacon
                - If slot is assigned, then after receiving the beacon, send the message
            Or the signal can be a slot association
                - If slot is not assigned, then read the payload and assign slot
        */
        void MAC_Sender_impl::signal_in(pmt::pmt_t msg) {
            // Get BLOT
            pmt::pmt_t blob;
            
            // blob = msg;
            if(pmt::is_eof_object(msg)) {
                return;
            } 
            else if(pmt::is_pair(msg)) {
                // pair's second element-> pmt::cdr
                blob = pmt::cdr(msg);
            } 
            else if(pmt::is_blob(msg)) {
                blob = msg;
            } 
            else {
                return;
            }


            size_t data_len = pmt::blob_length(blob);
            if(data_len < 13) {
                #if DEBUG == 1
                    std::cout << "Sender #" << std::hex << d_src_addr << " MAC Rx: frame too short. Dropping!\n" << std::endl;
                #endif
                
                return;
            }

            uint16_t crc = crc16((char*)pmt::blob_data(blob), data_len);

            d_num_packets_received++;
            
            if(crc) {
                d_num_packet_errors++;
                
                #if DEBUG == 1
                    std::cout << "Sender #" << std::hex << d_src_addr << " MAC Rx: wrong crc. Dropping packet!\n" << std::endl;
                #endif

                return;
            }
            else{
                #if DEBUG == 1
                    std::cout << "Sender #" << std::hex << d_src_addr << " MAC Rx: correct crc. Propagate packet to APP layer.\n" << std::endl;
                #endif
            }

            // If the destination is someone else, ignore
            char msg_dst_arr[2];
            strncpy(msg_dst_arr, (char*)pmt::blob_data(blob)+5, 2);

            uint16_t msg_dst = (msg_dst_arr[1] & 0xFF);
            msg_dst  = (msg_dst << 8) | (msg_dst_arr[0] & 0xFF);
            // msg_dst = static_cast<uint16_t>(msg_dst);


            #if DEBUG == 1
                std::cout << "From Sender #" << std::hex << d_src_addr << ": My address = " << std::hex << d_src_addr << std::endl;
                std::cout << "From Sender #" << std::hex << d_src_addr << ": The dest_address received = " << std::hex << msg_dst << std::endl; 
            #endif

            if ((msg_dst != (uint16_t)(0xFFFF)) && (msg_dst != d_src_addr)) {
                #if DEBUG == 1
                    std::cout << "The message is not for me. Ignoring.\n" << std::endl;
                #endif

                return;
            }

            #if DEBUG == 1
                std::cout << "This message is for me. Proceeding.\n" << std::endl;
            #endif

            // Check type
            char type[1];
            strncpy(type, (char*)pmt::blob_data(blob)+1, 1);

            if ((type[0] & 0xFF) == BEACON_TYPE) {
                #if DEBUG == 1
                    std::cout << "From Sender #" << std::hex << d_src_addr << ": RECEIVED A BEACON" << std::endl;
                #endif

                // If slot is not assigned, send a join request
                if (!d_is_assigned) {
                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Slot is not assigned yet. Sending a request.\n" << std::endl;
                    #endif
                    
                    // Generate MAC
                    generate_mac_request(NULL, 2);

                    // Pass formatted to output port
                    message_port_pub(pmt::mp("mac out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));

                    // clear message buffer
                    memset(d_msg, 0, sizeof d_msg);
                }
                // Else, if there is a message in queue, send it in the assigned time slot
                else if (!q.empty()) {
                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Preparing to send the message." << std::endl;
                    #endif

                    // Extract beacon total ms
                    char payload[4];
                    strncpy(payload, (char*)pmt::blob_data(blob)+14, 4);

                    d_beacon_total_ms   = (payload[3] & 0xFF);
                    d_beacon_total_ms   = (d_beacon_total_ms << 8) | (payload[2] & 0xFF);
                    d_beacon_total_ms   = (d_beacon_total_ms << 8) | (payload[1] & 0xFF);
                    d_beacon_total_ms   = (d_beacon_total_ms << 8) | (payload[0] & 0xFF);

                    d_beacon_total_ms = static_cast<int>(d_beacon_total_ms);

                    // //Get current time
                    // d_current_time = boost::posix_time::microsec_clock::local_time();
                    
                    // // Total miliseconds
                    // boost::posix_time::time_duration tm_current  = d_current_time.time_of_day();
                    // long current_time_ms = tm_current.total_milliseconds();
                
                    // // Difference
                    // int diff = (int) current_time_ms - d_beacon_total_ms;
                
                    // int wait_time_ms = (d_assigned_slot * 1000) - diff;
                    // boost::this_thread::sleep(boost::posix_time::milliseconds(wait_time_ms));

                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Received: beacon total ms = " << std::dec << d_beacon_total_ms << std::endl;
                    #endif


                    // int wait_till = d_beacon_total_ms + (d_assigned_slot * 1000);
                    int wait_till = d_beacon_total_ms + d_assigned_slot;
                    
                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Waiting till: " << std::dec << wait_till << " ...." << std::endl;
                    #endif

                    // int current_ms = (int)(((boost::posix_time::time_duration) (boost::posix_time::microsec_clock::local_time()).time_of_day()).total_milliseconds());
                    // while (current_ms < wait_till) {
                    //     std::cout << current_ms << std::endl;
                    //     current_ms = (int)(((boost::posix_time::time_duration) (boost::posix_time::microsec_clock::local_time()).time_of_day()).total_milliseconds());
                    // }
                    while ( (int)((boost::posix_time::time_duration) (boost::posix_time::microsec_clock::local_time()).time_of_day()).total_milliseconds() < wait_till) {}
                    
                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Wait done. Sending the message.\n" << std::endl;
                    #endif


                    // Send the message to the receiver
                    // Generate MAC
                    generate_mac_msgtx((const char*)pmt::blob_data(q.front()), pmt::blob_length(q.front()));
                    q.pop();

                    // Pass it to output port
                    message_port_pub(pmt::mp("mac out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));

                    // clear buffer
                    memset(d_msg, 0, sizeof d_msg);
                }

            }
            else if ((type[0] & 0xFF) == ASSOC_TYPE) {
                #if DEBUG == 1
                    std::cout << "From Sender #" << std::hex << d_src_addr << ": RECEIVED ASSOCIATION" << std::endl;
                #endif

                if (!d_is_assigned) {
                    char payload[1];
                    strncpy(payload, (char*)pmt::blob_data(blob)+11, 1);
                    
                    d_assigned_slot = (payload[0] & 0xFF);
                    d_assigned_slot = static_cast<int>(d_assigned_slot);
                    
                    #if DEBUG == 1
                        std::cout << "From Sender #" << std::hex << d_src_addr << ": Assigned slot: " << std::dec << d_assigned_slot << "\n" << std::endl;
                    #endif

                    d_is_assigned = true;
                }
            }
            else {
                #if DEBUG == 1
                    std::cout << "UNKNOWN TYPE\n" << std::endl;
                #endif
            }

        }


        void MAC_Sender_impl::msg_in(pmt::pmt_t msg) {
            if (!d_is_assigned) {
                #if DEBUG == 1
                    std::cout << "From Sender #" << std::hex << d_src_addr << ": \nSlot is not assigned yet\n" << std::endl;
                #endif

                return;
            }

            // Get BLOB
            pmt::pmt_t blob;
            
            // blob = msg;
            if(pmt::is_eof_object(msg)) {
                return;
            } 
            else if(pmt::is_blob(msg)) {
                blob = msg;
            } 
            else if(pmt::is_pair(msg)) {
                // pair's second element-> pmt::cdr
                blob = pmt::cdr(msg);
            } 
            else {
                return;
            }

            // Push message in the queue
            q.push(blob);
        }

        void MAC_Sender_impl::generate_mac_request(const char *buf, int len) {
            // FCF
            d_msg[0]  = d_fcf                   & 0xFF;

            d_msg[1]  = REQ_TYPE;

            // seq nr
            d_msg[2]  = d_seq_nr++;

            // addr info of receiver (the coordinator)
            d_msg[3]  = d_coord_pan               & 0xFF;
            d_msg[4]  = (d_coord_pan >> 8)        & 0xFF;

            d_msg[5]  = d_coord_addr              & 0xFF;
            d_msg[6]  = (d_coord_addr >> 8)       & 0xFF;
                
            
            d_msg[7]  = d_src_pan               & 0xFF;
            d_msg[8]  = (d_src_pan >> 8)        & 0xFF;

            d_msg[9]  = d_src_addr              & 0xFF;
            d_msg[10] = (d_src_addr >> 8)       & 0xFF;


            // Payload -> Sender Address
            d_msg[11] = d_src_addr              & 0xFF;
            d_msg[12] = (d_src_addr >> 8)       & 0xFF;      
            
            
            // Add crc - 2 bytes
            uint16_t crc = crc16(d_msg, 13);

            d_msg[13] = crc & 0xFF;
            d_msg[14] = crc >> 8;

            // total len
            d_msg_len = 15;
        }

        void MAC_Sender_impl::generate_mac_msgtx(const char *buf, int len) {
            // Test: forward the buffer to port
            //std::memcpy(d_msg, buf, len);
            //d_msg_len = len;
            //return;   
            
            // FCF
            d_msg[0]  = d_fcf                   & 0xFF;

            d_msg[1]  = MSG_TYPE;

            // seq nr
            d_msg[2]  = d_seq_nr++;

            // addr info -> destination = receiver
            d_msg[3]  = d_dst_pan               & 0xFF;
            d_msg[4]  = (d_dst_pan >> 8)        & 0xFF;

            d_msg[5]  = d_dst_addr              & 0xFF;
            d_msg[6]  = (d_dst_addr >> 8)       & 0xFF;

            
            d_msg[7]  = d_src_pan               & 0xFF;
            d_msg[8]  = (d_src_pan >> 8)        & 0xFF;

            d_msg[9]  = d_src_addr              & 0xFF;
            d_msg[10] = (d_src_addr >> 8)       & 0xFF;


            // Encrypt message before sending

            // Create an instance of aes, and call the encrypt function to encrypt the buffer
            AES_impl aes;
            std::pair<uint8_t*, int> ct = aes.encrypt(buf, len);
            
            // The encrypted message will have a length that is multiple of 16 (AES CBC BLOCKLENGTH)
            // First byet of the payload holds the message length
            uint8_t msg_len = (uint8_t)ct.second;
            d_msg[11] = (msg_len & 0xFF);
            
            // Copy the content in d_msg, starting at the 12th index
            std::memcpy(d_msg + 12, ct.first, msg_len);

            // Free memory
            delete ct.first;
            
            // Add crc - 2 bytes
            uint16_t crc = crc16(d_msg, 12 + msg_len);

            d_msg[12 + msg_len] = crc & 0xFF;
            d_msg[13 + msg_len] = crc >> 8;

            // total len
            d_msg_len = 12 + msg_len + 2;
        }


        uint16_t MAC_Sender_impl::crc16(char *buf, int len) {
            uint16_t crc = 0;

            for(int i = 0; i < len; i++) {
                for(int k = 0; k < 8; k++) {
                    int input_bit = (!!(buf[i] & (1 << k)) ^ (crc & 1));
                    crc = crc >> 1;
                    if(input_bit) {
                        crc ^= (1 << 15);
                        crc ^= (1 << 10);
                        crc ^= (1 <<  3);
                    }
                }
            }
            
            return crc;
        }

    } /* namespace IEEE_802_15_4_CustomBlocks */
} /* namespace gr */

