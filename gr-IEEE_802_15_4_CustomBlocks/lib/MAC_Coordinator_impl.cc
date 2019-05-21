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
#include "MAC_Coordinator_impl.h"


namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        MAC_Coordinator::sptr
        MAC_Coordinator::make(long superframe_len, int total_slots, long threshold, int fcf, int seq_nr, int dst_pan, int coord_pan, int coord_addr)
        {
            return gnuradio::get_initial_sptr (new MAC_Coordinator_impl(superframe_len, total_slots, threshold, fcf, seq_nr, dst_pan, coord_pan, coord_addr));
        }

        /*
        * The private constructor
        */
        MAC_Coordinator_impl::MAC_Coordinator_impl(long superframe_len, int total_slots, long threshold, int fcf, int seq_nr, int dst_pan, int coord_pan, int coord_addr)
            :   gr::block(  "MAC_Coordinator",
                            gr::io_signature::make(0, 0, 0),
                            gr::io_signature::make(0, 0, 0)),
                d_superframe_len(superframe_len),
                d_th(threshold),
                d_total_slots(total_slots),
                d_fcf(fcf),
                d_seq_nr(seq_nr),
                d_dst_pan(dst_pan),
                // d_dst_addr(),
                d_coord_pan(coord_pan),
                d_coord_addr(coord_addr),
                d_msg_offset(0),
                d_num_packet_errors(0),
                d_num_packets_received(0),
                d_current_slot(0x0A)
        {
            // In port 1
            message_port_register_in(pmt::mp("beacon signal in"));
            set_msg_handler(pmt::mp("beacon signal in"), boost::bind(&MAC_Coordinator_impl::send_beacon, this, _1));

            // In port 2
            message_port_register_in(pmt::mp("request in"));
            set_msg_handler(pmt::mp("request in"), boost::bind(&MAC_Coordinator_impl::assign_slot, this, _1));

                
            // Out port 1
            message_port_register_out(pmt::mp("mac out"));
        }

        /*
        * Our virtual destructor.
        */
        MAC_Coordinator_impl::~MAC_Coordinator_impl()
        {

        }


        void MAC_Coordinator_impl::send_beacon(pmt::pmt_t msg) {
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

            // Save beacon receive time
            d_last_beacon = boost::posix_time::microsec_clock::local_time();

            boost::posix_time::time_duration tm_beacon  = d_last_beacon.time_of_day();
            // d_beacon_recv_hour                          = tm_beacon.hours();
            // d_beacon_recv_minute                        = tm_beacon.minutes();
            // d_beacon_recv_second                        = tm_beacon.seconds();
            // d_beacon_recv_fractional_second             = tm_beacon.fractional_seconds();
            d_beacon_recv_ms = tm_beacon.total_milliseconds();

            #if DEBUG == 1
                std::cout << "\n******************************************************" << std::endl;
                std::cout << "From COORDINATOR: Beacon signal received at: " << d_last_beacon << std::endl;
                std::cout << "From COORDINATOR: Sending the beacon" << std::endl;
                std::cout << "From COORDINATOR: total milliseconds: " << std::dec << d_beacon_recv_ms << std::endl;
                std::cout << "******************************************************\n" << std::endl;
            #endif

            // Broadcast: destination addres = 0xFFFF
            d_dst_addr = BROADCAST_ADDR;

            // Generate MAC
            generate_mac_beacon((const char*)pmt::blob_data(blob), pmt::blob_length(blob));

            // Pass it to output port
            message_port_pub(pmt::mp("mac out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));

            // clear buffer
            memset(d_msg, 0, sizeof d_msg);
        }


        void MAC_Coordinator_impl::assign_slot(pmt::pmt_t msg) {
            // CHECK TIME
            //-------------

            // If beacon has never been receievd, discard
            if (d_last_beacon.is_not_a_date_time()) {
                #if DEBUG == 1
                    std::cout << "From COORDINATOR: No beacon signal has been received yet\n" << std::endl;
                #endif

                return;
            }

            // Get difference between Beacon receive time and Request receive time
            d_current_time = boost::posix_time::microsec_clock::local_time();
            
            #if DEBUG == 1
                std::cout << "From COORDINATOR: Request received" << std::endl;
            #endif

            boost::posix_time::time_duration diff = d_current_time - d_last_beacon;
            long diff_ms = diff.total_milliseconds();
            
            #if DEBUG == 1
                std::cout << "From COORDINATOR: Time difference: " << std::dec << diff_ms << " ms\n" << std::endl;
            #endif

            // Disregard if time difference > threshold
            if (diff_ms > d_th) {
                return;
            }

            //--------------------------------------------------------------------------------
            // ***************** Slot can be assigned, output the requst *****************
            //--------------------------------------------------------------------------------

            // Get BLOB
            pmt::pmt_t blob;
            
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
                    std::cout << "From COORDINATOR MAC: frame too short. Dropping.\n" << std::endl;
                #endif

                return;
            }

            uint16_t crc = crc16((char*)pmt::blob_data(blob), data_len);
            d_num_packets_received++;
            
            if(crc) {
                d_num_packet_errors++;
                    
                #if DEBUG == 1
                    std::cout << "From COORDINATOR MAC: wrong crc. Dropping packet.\n" << std::endl;
                #endif

                return;
            }
            else{
                #if DEBUG  == 1
                    std::cout << "From COORDINATOR MAC: correct crc. Propagate packet.\n" << std::endl;
                #endif
            }

            // Check type
            char type[1];
            strncpy(type, (char*)pmt::blob_data(blob)+1, 1);

            if ((type[0] & 0xFF) == REQ_TYPE) {
                #if DEBUG == 1
                    std::cout << "From COORDINATOR: Sending Association...\n" << std::endl;
                #endif

                // Extract payloaddiblesdibles
                char payload[2];
                std::memcpy(payload, (char*)pmt::blob_data(blob) + 11, sizeof payload);

                // Get destination address form the req payload
                d_dst_addr  = (payload[1] & 0xFF);
                d_dst_addr  = (d_dst_addr << 8) | (payload[0] & 0xFF);

                
                // Generate MAC
                generate_mac_association((const char*)pmt::blob_data(blob), pmt::blob_length(blob));

                // Pass it to output port
                message_port_pub(pmt::mp("mac out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));
                
                // clear buffer
                memset(d_msg, 0, sizeof d_msg);
            }
        }


        void MAC_Coordinator_impl::generate_mac_beacon(const char *buf, int len) {
            // // Test: forward the buffer to port
            // std::memcpy(d_msg, buf, len);
            // d_msg_len = len;

            // FCF
            d_msg[0]  = d_fcf                   & 0xFF;
            d_msg[1]  = BEACON_TYPE;

            // seq nr
            d_msg[2]  = d_seq_nr++;

            // addr info
            d_msg[3]  = d_dst_pan               & 0xFF;
            d_msg[4]  = (d_dst_pan >> 8)        & 0xFF;

            d_msg[5]  = d_dst_addr              & 0xFF;
            d_msg[6]  = (d_dst_addr >> 8)       & 0xFF;
            
            d_msg[7]  = d_coord_pan             & 0xFF;
            d_msg[8]  = (d_coord_pan >> 8)      & 0xFF;

            d_msg[9]  = d_coord_addr            & 0xFF;
            d_msg[10] = (d_coord_addr >> 8)     & 0xFF;


            // Payload
            // // Test
            // std::memcpy(d_msg + 9, buf, len);

            // Superframe length - 2 bytes
            d_msg[11]   = d_superframe_len              & 0xFF;
            d_msg[12]   = (d_superframe_len >> 8)       & 0xFF;      

            // Total slots - 1 byte
            d_msg[13]   = d_total_slots                 & 0xFF;

            // Time sent : 1 + 1 + 1 + 3 = 6 bytes
            // d_msg[14]   = d_beacon_recv_hour                            & 0xFF;
            // d_msg[15]   = d_beacon_recv_minute                          & 0xFF;
            // d_msg[16]   = d_beacon_recv_second                          & 0xFF;
            // d_msg[17]   = d_beacon_recv_fractional_second               & 0xFF;
            // d_msg[18]   = (d_beacon_recv_fractional_second >> 8)        & 0xFF;
            // d_msg[19]   = (d_beacon_recv_fractional_second >> 16)       & 0xFF;
            d_msg[14]   = d_beacon_recv_ms              & 0xFF;
            d_msg[15]   = (d_beacon_recv_ms >> 8)       & 0xFF;
            d_msg[16]   = (d_beacon_recv_ms >> 16)      & 0xFF;
            d_msg[17]   = (d_beacon_recv_ms >> 24)      & 0xFF;

            // Add crc - 2 bytes
            uint16_t crc = crc16(d_msg, 18);

            d_msg[18] = crc & 0xFF;
            d_msg[19] = crc >> 8;

            // total len
            d_msg_len = 20;
        }

        void MAC_Coordinator_impl::generate_mac_association(const char *buf, int len) {
            // // Test: forward the buffer to port
            // std::memcpy(d_msg, buf, len);
            // d_msg_len = len;

            // FCF
            d_msg[0]  = d_fcf                   & 0xFF;
            d_msg[1]  = ASSOC_TYPE;

            // seq nr
            d_msg[2]  = d_seq_nr++;

            // addr info
            d_msg[3]  = d_dst_pan               & 0xFF;
            d_msg[4]  = (d_dst_pan >> 8)        & 0xFF;

            d_msg[5]  = d_dst_addr              & 0xFF;
            d_msg[6]  = (d_dst_addr >> 8)       & 0xFF;
            
            d_msg[7]  = d_coord_pan             & 0xFF;
            d_msg[8]  = (d_coord_pan >> 8)      & 0xFF;

            d_msg[9]  = d_coord_addr            & 0xFF;
            d_msg[10] = (d_coord_addr >> 8)     & 0xFF;


            // Payload
            // // Test
            // std::memcpy(d_msg + 9, buf, len);

            d_msg[11] = d_current_slot         & 0xFF;

            #if DEBUG == 1
                std::cout << "Assignig slot# at d_msg[11] = " << std::hex << (d_msg[11] & 0xFF) << "\n" << std::endl;
            #endif

            d_current_slot = d_current_slot + (0x05);

            // Add crc - 2 bytes
            uint16_t crc = crc16(d_msg, 12);

            d_msg[12] = crc & 0xFF;
            d_msg[13] = crc >> 8;

            // total len
            d_msg_len = 14;  
        }



        uint16_t MAC_Coordinator_impl::crc16(char *buf, int len) {
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

