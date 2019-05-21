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

#include "MAC_Tx_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>

#include <iostream>
#include <iomanip>

#define dout std::cout

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        MAC_Tx::sptr
        MAC_Tx::make(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr)
        {
            // Default
            return gnuradio::get_initial_sptr (new MAC_Tx_impl(fcf, seq_nr, dst_pan, dst_addr, src_pan, src_addr));
        }

        /*
        * The private constructor
        */
        MAC_Tx_impl::MAC_Tx_impl(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr) 
        :   gr::block(  "MAC_Tx", 
                        gr::io_signature::make(0, 0, 0), 
                        gr::io_signature::make(0, 0, 0)),
            d_msg_offset(0),
            d_fcf(fcf),
            d_seq_nr(seq_nr),
            d_dst_pan(dst_pan),
            d_dst_addr(dst_addr),
            d_src_pan(src_pan),
            d_src_addr(src_addr),
            d_num_packet_errors(0),
            d_num_packets_received(0)
        {
            // In port
            message_port_register_in(pmt::mp("message in"));
            // Bind the port to a function
            // The 'this' and '_1' are standard ways of using the Boost bind function to 
            // pass the 'this' pointer as the first argument to the class (standard OOP practice) and 
            // the _1 is an indicator that the function expects 1 additional argument.
            set_msg_handler(pmt::mp("message in"), boost::bind(&MAC_Tx_impl::msg_in, this, _1));

            // Out port
            message_port_register_out(pmt::mp("mac out"));
        }

        /*
        * Our virtual destructor.
        */
        MAC_Tx_impl::~MAC_Tx_impl()
        {

        }

        void MAC_Tx_impl::msg_in(pmt::pmt_t msg) {
            // Get BLOB
            pmt::pmt_t blob;

            // Check for msg
            if(pmt::is_eof_object(msg)) {
                dout << "MAC: exiting" << std::endl;
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
                dout << "MAC: unknown input" << std::endl;
                return;
            }

            //dout << "MAC: received new message from APP of length " << pmt::blob_length(blob) << std::endl;

            generate_mac_format((const char*)pmt::blob_data(blob), pmt::blob_length(blob));

            // pass it to output port
            message_port_pub(pmt::mp("mac out"), pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_msg, d_msg_len)));
        }


        uint16_t MAC_Tx_impl::crc16(char *buf, int len) {
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

        void MAC_Tx_impl::generate_mac_format(const char *buf, int len) {
            // for (int i = 0; i < 256; i++) {
            //     d_msg[i] = 't';
            // }
            // d_msg_len = 256;

            // std::memcpy(d_msg, buf, len);
            // d_msg_len = len;


            // FCF
            // data frame, no security
            d_msg[0] = d_fcf         & 0xFF;
            d_msg[1] = (d_fcf >> 8)  & 0xFF;

            // seq nr
            d_msg[2] = d_seq_nr++;

            // addr info
            d_msg[3] = d_dst_pan            & 0xFF;
            d_msg[4] = (d_dst_pan >> 8)     & 0xFF;

            d_msg[5] = d_dst_addr           & 0xFF;
            d_msg[6] = (d_dst_addr >> 8)    & 0xFF;
            
            d_msg[7] = d_src_addr           & 0xFF;
            d_msg[8] = (d_src_addr >> 8)    & 0xFF;

            // Copy messsage
            std::memcpy(d_msg + 9, buf, len);

            // Add crc - 2 bytes
            uint16_t crc = crc16(d_msg, len + 9);

            d_msg[ 9 + len] = crc & 0xFF;
            d_msg[10 + len] = crc >> 8;

            // total len
            d_msg_len = 9 + len + 2;
        }

    } /* namespace IEEE_802_15_4_CustomBlocks */
} /* namespace gr */

