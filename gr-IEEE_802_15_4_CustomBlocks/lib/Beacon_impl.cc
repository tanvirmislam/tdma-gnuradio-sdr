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
#include "Beacon_impl.h"

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        Beacon::sptr
        Beacon::make(long interval, long total_msg)
        {
            return gnuradio::get_initial_sptr(new Beacon_impl(interval, total_msg));
        }

        /*
         * The private constructor
         */
        Beacon_impl::Beacon_impl(long interval, long total_msg)
            :   gr::block(  "Beacon",
                            gr::io_signature::make(0, 0, 0),
                            gr::io_signature::make(0, 0, 0)),
                d_interval(interval),
                d_total_msg(total_msg),
                d_nmsg_left(total_msg),
                d_msg("Send beacon"),
                d_msg_len(d_msg.length()),
                d_started(false)
        {
            // No in port
            
            // Out port
            message_port_register_out(pmt::mp("signal out"));

            // Create a thread for the timer
            // std::cout << "Initiating thread" << std::endl;
            d_thread  = gr::thread::thread(boost::bind(&Beacon_impl::run, this));
            d_started = true;
        }

        Beacon_impl::~Beacon_impl() {
            // std::cout << "Destructor called" << std::endl;
            if (d_started) {
                d_thread.interrupt();
                d_thread.join();
            }
            d_started = false;
        }

        void Beacon_impl::run() {
            // std::cout << "Thread Running" << std::endl;
            // flow graph delay
            boost::this_thread::sleep(boost::posix_time::milliseconds(FLOW_DELAY));

            {
                gr::thread::scoped_lock(d_mutex);
                // std::cout << "obtained lock" << std::endl;
                while(true) {
                    // If done
                    if(!d_nmsg_left) {
                        break;
                    }

                    // Convert d_msg to char array
                    char char_arr[d_msg_len+1];
                    strcpy(char_arr, d_msg.c_str());

                    // publish it to output port
                    pmt::pmt_t payload = pmt::make_blob(char_arr, d_msg_len);
                    message_port_pub(pmt::mp("signal out"), pmt::cons(pmt::PMT_NIL, payload));

                    
                    if(d_nmsg_left > 0) {
                        d_nmsg_left--;
                    }

                    // Wait
                    boost::this_thread::sleep(boost::posix_time::milliseconds(d_interval));
                }

            } // Scope lock released

            // boost::this_thread::sleep(boost::posix_time::milliseconds(FLOW_DELAY));   
        }
        
    } /* namespace IEEE_802_15_4_CustomBlocks */
} /* namespace gr */

