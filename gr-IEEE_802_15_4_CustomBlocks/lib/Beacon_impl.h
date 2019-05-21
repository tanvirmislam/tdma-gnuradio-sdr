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

#ifndef INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_IMPL_H
#define INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_IMPL_H

#include <IEEE_802_15_4_CustomBlocks/Beacon.h>

// #include <boost/thread/thread.hpp>
// #include <boost/thread/scoped_thread.hpp>
#include <string>

#define FLOW_DELAY 600

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        class Beacon_impl : public Beacon
        {
        private:
            long    d_interval;
            long    d_total_msg;
            long    d_nmsg_left;

            std::string  d_msg;
            size_t       d_msg_len;

            gr::thread::thread      d_thread;
            gr::thread::mutex       d_mutex;
            bool                    d_started;

        public:
            Beacon_impl(long interval, long total_msg);
            ~Beacon_impl();

            static void* run_thread (void* arg);
            void run();
        };

        // class Task{
        // private:
        //     long    d_interval;
        //     long    d_total_msg;
        //     long    d_nmsg_left;

        //     std::string  d_msg;
        //     size_t       d_msg_len;

        //     boost::mutex* d_mutex;

        // public:
        //     Task (long interval, long total_msg, long nmsg_left, std::string msg, size_t msg_len, gr::thread::mutex* mutex);
        //         :   d_interval(interval),
        //             d_total_msg(total_msg),
        //             d_nmsg_left(total_msg),
        //             d_msg(msg),
        //             d_msg_len(msg_len),
        //             d_mutex(mutex)
        //     {

        //     }
            
        //     void run();
        // };


    } // namespace IEEE_802_15_4_CustomBlocks
} // namespace gr

#endif /* INCLUDED_IEEE_802_15_4_CUSTOMBLOCKS_BEACON_IMPL_H */

