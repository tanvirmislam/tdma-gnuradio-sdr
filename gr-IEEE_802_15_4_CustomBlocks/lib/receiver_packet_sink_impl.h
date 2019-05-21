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

#ifndef INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_RECEIVER_PACKET_SINK_IMPL_H
#define INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_RECEIVER_PACKET_SINK_IMPL_H

#include <IEEE_802_15_4_CustomBlocks/receiver_packet_sink.h>

// very verbose output for almost each sample
#define VERBOSE 0
// less verbose output for higher level debugging
#define VERBOSE2 0

// this is the mapping between chips and symbols if we do
// a fm demodulation of the O-QPSK signal. Note that this
// is different than the O-QPSK chip sequence from the
// 802.15.4 standard since there there is a translation
// happening.
// See "CMOS RFIC Architectures for IEEE 802.15.4 Networks",
// John Notor, Anthony Caviglia, Gary Levy, for more details.
static const unsigned int CHIP_MAPPING[] = {
                                                1618456172,
                                                1309113062,
                                                1826650030,
                                                1724778362,
                                                778887287,
                                                2061946375,
                                                2007919840,
                                                125494990,
                                                529027475,
                                                838370585,
                                                320833617,
                                                422705285,
                                                1368596360,
                                                85537272,
                                                139563807,
                                                2021988657
                                            };

static const int MAX_PKT_LEN    = 128 -  1; // remove header and CRC
static const int MAX_LQI_SAMPLES = 8;       // Number of chip correlation samples to take


namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {
        
        class receiver_packet_sink_impl : public receiver_packet_sink
        {
        private:
            enum {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER} d_state;

            unsigned int      d_sync_vector;            // 802.15.4 standard is 4x 0 bytes and 1x0xA7
            int      d_threshold;                       // how many bits may be wrong in sync vector

            unsigned int      d_shift_reg;              // used to look for sync_vector
            int               d_preamble_cnt;           // count on where we are in preamble
            int               d_chip_cnt;               // counts the chips collected

            unsigned int      d_header;                 // header bits
            int               d_headerbitlen_cnt;       // how many so far

            unsigned char     d_packet[MAX_PKT_LEN];    // assembled payload
            unsigned char     d_packet_byte;            // byte being assembled
            int               d_packet_byte_index;      // which bit of d_packet_byte we're working on
            int               d_packetlen;              // length of packet
            int               d_packetlen_cnt;          // how many so far
            int               d_payload_cnt;            // how many bytes in payload

            unsigned int      d_lqi;                    // Link Quality Information
            unsigned int      d_lqi_sample_count;

            // FIXME:
            char buf[256];

        public:
            receiver_packet_sink_impl(int threshold);
            ~receiver_packet_sink_impl();
            

            void enter_search();
            void enter_have_sync();
            void enter_have_header(int payload_len);
            unsigned int count_bits32(unsigned int x);
            unsigned char decode_chips(unsigned int chips);
            int slice(float x);


            int general_work(int noutput_items,
                            gr_vector_int &ninput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items);
        };

  } // namespace IEEE_802_15_4_CustomBlocks
} // namespace gr

#endif /* INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_RECEIVER_PACKET_SINK_IMPL_H */

