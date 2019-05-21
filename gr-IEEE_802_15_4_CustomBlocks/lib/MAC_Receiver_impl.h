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

#ifndef INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_MAC_RECEIVER_IMPL_H
#define INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_MAC_RECEIVER_IMPL_H

#include <IEEE_802_15_4_CustomBlocks/MAC_Receiver.h>

#include "const.h"
#include "AES_impl.h"

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        class MAC_Receiver_impl : public MAC_Receiver
        {
        private:
        	int         d_msg_offset;
            int         d_msg_len;
            uint16_t    d_fcf;
            uint8_t     d_seq_nr;
            uint16_t    d_dst_pan;
            uint16_t    d_dst_addr;
            uint16_t    d_src_pan;
            uint16_t    d_src_addr;
            uint16_t    d_coord_pan;
            uint16_t    d_coord_addr;

            int         d_num_packet_errors;
            int         d_num_packets_received;

            char        d_msg[256];


        public:
            MAC_Receiver_impl(int fcf, int seq_nr, int dst_pan, int dst_addr, int src_pan, int src_addr, int coord_pan, int coord_addr);
            ~MAC_Receiver_impl();

            void mac_in(pmt::pmt_t msg);
            void extract_msg(const char *buf, int len, uint16_t msg_src);
            uint16_t crc16(char *buf, int len);
        };

    } // namespace IEEE_802_15_4_CustomBlocks
} // namespace gr

#endif /* INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_MAC_RECEIVER_IMPL_H */

