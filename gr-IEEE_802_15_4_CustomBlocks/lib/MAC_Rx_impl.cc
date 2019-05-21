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
#include "MAC_Rx_impl.h"

#include <gnuradio/block_detail.h>
#include <iostream>
#include <iomanip>

#define dout std::cout

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        MAC_Rx::sptr
        MAC_Rx::make(int fcf, int seq_nr, int dst_pan, int dst, int src_pan, int src)
        {
            return gnuradio::get_initial_sptr (new MAC_Rx_impl(fcf, seq_nr, dst_pan, dst, src_pan, src));
        }

        /*
         * The private constructor
         */
        MAC_Rx_impl::MAC_Rx_impl(int fcf, int seq_nr, int dst_pan, int dst, int src_pan, int src)
            :   gr::block(  "MAC_Rx",
                            gr::io_signature::make(0, 0, 0),
                            gr::io_signature::make(0, 0, 0)),
                d_msg_offset(0),
                d_fcf(fcf),
                d_seq_nr(seq_nr),
                d_dst_pan(dst_pan),
                d_dst(dst),
                d_src(src),
                d_num_packet_errors(0),
                d_num_packets_received(0)
        {
            // In port
            message_port_register_in(pmt::mp("received message in"));
            // Bind the port to a function
            // The 'this' and '_1' are standard ways of using the Boost bind function to 
            // pass the 'this' pointer as the first argument to the class (standard OOP practice) and 
            // the _1 is an indicator that the function expects 1 additional argument.
            set_msg_handler(pmt::mp("received message in"), boost::bind(&MAC_Rx_impl::recvd_msg_in, this, _1));

            // Out port
            message_port_register_out(pmt::mp("payload out"));
        }

        MAC_Rx_impl::~MAC_Rx_impl()
        {

        }

        uint16_t MAC_Rx_impl::crc16(char *buf, int len) {
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

        void MAC_Rx_impl::recvd_msg_in(pmt::pmt_t msg) {
            pmt::pmt_t blob;

            // if(pmt::is_blob(msg)) {
            //     blob = msg;
            // } 
            // else if(pmt::is_pair(msg)) {
            //     // pair's second element-> pmt::cdr
            //     blob = pmt::cdr(msg);
            // } 
            // size_t data_len = pmt::blob_length(blob);
            // pmt::pmt_t mac_payload = pmt::make_blob((char*)pmt::blob_data(blob), data_len);
            // message_port_pub(pmt::mp("payload out"), pmt::cons(pmt::PMT_NIL, mac_payload));


            if(pmt::is_pair(msg)) {
                blob = pmt::cdr(msg);
            } 
            else {
                assert(false);
            }

            size_t data_len = pmt::blob_length(blob);
            if(data_len < 11) {
                dout << "MAC: frame too short. Dropping!" << std::endl;
                return;
            }

            uint16_t crc = crc16((char*)pmt::blob_data(blob), data_len);
            d_num_packets_received++;
            
            if(crc) {
                d_num_packet_errors++;
                dout << "MAC: wrong crc. Dropping packet!" << std::endl;
                return;
            }
            else{
                dout << "MAC: correct crc. Propagate packet to APP layer." << std::endl;
            }

            pmt::pmt_t mac_payload = pmt::make_blob((char*)pmt::blob_data(blob) + 9 , data_len - 9 - 2);

            message_port_pub(pmt::mp("payload out"), pmt::cons(pmt::PMT_NIL, mac_payload));
        }

        

  } /* namespace IEEE_802_15_4_CustomBlocks */
} /* namespace gr */

