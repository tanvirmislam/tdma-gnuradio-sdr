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


#ifndef INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_H
#define INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_H

#include <IEEE_802_15_4_CustomBlocks/api.h>
#include <gnuradio/block.h>

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {
        /*!
        * \brief <+description of block+>
        * \ingroup IEEE_802_15_4_CustomBlocks
        *
        */
        class IEEE_802_15_4_CUSTOMBLOCKS_API Beacon : virtual public gr::block
        {
        public:
            typedef boost::shared_ptr<Beacon> sptr;

            /*!
            * \brief Return a shared_ptr to a new instance of IEEE_802_15_4_CustomBlocks::Beacon.
            *
            * To avoid accidental use of raw pointers, IEEE_802_15_4_CustomBlocks::Beacon's
            * constructor is in a private implementation
            * class. IEEE_802_15_4_CustomBlocks::Beacon::make is the public interface for
            * creating new instances.
            */
            static sptr make(long interval = 1000, long total_msg = -1);
        };

    } // namespace IEEE_802_15_4_CustomBlocks
} // namespace gr

#endif /* INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_H */

