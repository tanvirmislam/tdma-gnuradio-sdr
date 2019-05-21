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
#include "MAC_Receiver_impl.h"


namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        MAC_Receiver::sptr
        MAC_Receiver::make(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr, int coord_pan, int coord_addr) 
        {
            return gnuradio::get_initial_sptr (new MAC_Receiver_impl(fcf, seq_nr, dst_pan, dst_addr, src_pan, src_addr, coord_pan, coord_addr));
        }

        
        MAC_Receiver_impl::MAC_Receiver_impl(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr, int coord_pan, int coord_addr)
            :   gr::block(  "MAC_Receiver",
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
                d_num_packets_received(0)
        {
            // In port 1
            message_port_register_in(pmt::mp("in"));
            set_msg_handler(pmt::mp("in"), boost::bind(&MAC_Receiver_impl::mac_in, this, _1));

            // Out port 1
            message_port_register_out(pmt::mp("out"));
        }

        MAC_Receiver_impl::~MAC_Receiver_impl()
        {
            
        }

        void MAC_Receiver_impl::mac_in(pmt::pmt_t msg) {
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
                    std::cout << "Receiver #" << std::hex << d_src_addr << " MAC Rx: frame too short. Dropping.\n" << std::endl;
                #endif

                return;
            }

            uint16_t crc = crc16((char*)pmt::blob_data(blob), data_len);

            d_num_packets_received++;
            
            if(crc) {
                d_num_packet_errors++;
                
                #if DEBUG == 1
                    std::cout << "Receiver #" << std::hex << d_src_addr << " MAC Rx: wrong crc. Dropping packet.\n" << std::endl;
                #endif
                
                return;
            }
            else{
                #if DEBUG == 1
                    std::cout << "Receiver #" << std::hex << d_src_addr << " MAC Rx: correct crc. Propagate packet to APP layer.\n" << std::endl;
                #endif
            }

            // If the destination is someone else, ignore
            char msg_dst_arr[2];
            strncpy(msg_dst_arr, (char*)pmt::blob_data(blob)+5, 2);

            uint16_t msg_dst = (msg_dst_arr[1] & 0xFF);
            msg_dst  = (msg_dst << 8) | (msg_dst_arr[0] & 0xFF);


            #if DEBUG == 1
                std::cout << "From Receiver #" << std::hex << d_src_addr << ": My address = " << std::hex << d_src_addr << std::endl;
                std::cout << "From Receiver #" << std::hex << d_src_addr << ": The dest_address received = " << std::hex << msg_dst << std::endl; 
            #endif

            // If the signal's destination is not this receiver or a broadcast, then quit
            if ((msg_dst != (uint16_t)(0xFFFF)) && (msg_dst != d_src_addr)) {
                #if DEBUG == 1
                    std::cout << "The message is not for me. Ignoring.\n" << std::endl;
                #endif
                
                return;
            }

            #if DEBUG == 1
                std::cout << "This message is for me. Proceeding.\n" << std::endl;
            #endif
            
            
            // Check if the signal was sent out by me. If so, ignore
            char msg_src_arr[2];
            strncpy(msg_src_arr, (char*)pmt::blob_data(blob)+9, 2);

            uint16_t msg_src = (msg_src_arr[1] & 0xFF);
            msg_src  = (msg_src << 8) | (msg_src_arr[0] & 0xFF);

            if (msg_src == d_src_addr) {
                #if DEBUG == 1
                    std::cout << "Found message that I sent out. Ignoring.\n" << std::endl;
                #endif
            
                return;
            }


            // Check type
            char type[1];
            strncpy(type, (char*)pmt::blob_data(blob)+1, 1);

            if ((type[0] & 0xFF) == MSG_TYPE) {
                #if DEBUG == 1
                    std::cout << "From Receiver #" << std::hex << d_src_addr << ": RECEIVED A MESSAGE.\n" << std::endl;
                #endif
                
                // Extract message
                extract_msg((const char*)pmt::blob_data(blob), pmt::blob_length(blob), msg_src);

                // Pass formatted to output port
                message_port_pub(pmt::mp("out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));

                // clear message buffer
                memset(d_msg, 0, sizeof d_msg);
            }
        }

        void MAC_Receiver_impl::extract_msg(const char *buf, int len, uint16_t msg_src) {
            int msg_len = (buf[11] & 0xFF);
            msg_len = static_cast<int>(msg_len);

            // Decrypt
            // Create an instance of AES_impl 
            AES_impl aes;

            std::pair<char*, int> dt = aes.decrypt(buf+12, msg_len);

            
            // Add name header
            int intro_len;
            if (msg_src == 0x5566) {
                memcpy(d_msg, "-> Alpha: ", 10);
                intro_len = 10;
            }
            else if (msg_src == 0x6677) {
                memcpy(d_msg, "-> Beta: ", 9);
                intro_len = 9;
            }
            else {
                memcpy(d_msg, "-> Gamma: ", 10);
                intro_len = 10;
            }

            
            memcpy(d_msg + intro_len, dt.first, dt.second);
            d_msg_len = intro_len + dt.second;

            // Add new line at the end
            memcpy(d_msg + d_msg_len, "\n", 1);
            d_msg_len++;


            //memcpy(d_msg, dt.first, dt.second);
            //d_msg_len = dt.second;


            delete dt.first;
        }

        uint16_t MAC_Receiver_impl::crc16(char *buf, int len) {
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

