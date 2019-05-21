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
#include "AES_impl.h"

namespace gr {
    namespace IEEE_802_15_4_CustomBlocks {

        AES::sptr AES::make() {
            return gnuradio::get_initial_sptr (new AES_impl());
        }

        /*
         * The private constructor
         */
        AES_impl::AES_impl() : gr::block("AES", 
                                        gr::io_signature::make(0, 0, 0),
                                        gr::io_signature::make(0, 0, 0)) 
        {

        }

        
        AES_impl::~AES_impl()
        {

        }
        
        void AES_impl::printCharArray(char* arr, int len) {
            for (int i = 0; i < len; i++) {
                std::cout << arr[i];
            }
            std::cout << std::endl;
        }

        // This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
        void AES_impl::KeyExpansion(uint8_t* RoundKey, const uint8_t* Key) {
            unsigned i, j, k;
            uint8_t tempa[4]; // Used for the column/row operations

            // The first round key is the key itself.
            for (i = 0; i < Nk; ++i) {
                RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
                RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
                RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
                RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
            }

            // All other round keys are found from the previous round keys.
            for (i = Nk; i < Nb * (Nr + 1); ++i) {
                {
                    k = (i - 1) * 4;
                    tempa[0]=RoundKey[k + 0];
                    tempa[1]=RoundKey[k + 1];
                    tempa[2]=RoundKey[k + 2];
                    tempa[3]=RoundKey[k + 3];
                }

                if (i % Nk == 0) {
                    // This function shifts the 4 bytes in a word to the left once.
                    // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

                    // Function RotWord()
                    {
                        const uint8_t u8tmp = tempa[0];
                        tempa[0] = tempa[1];
                        tempa[1] = tempa[2];
                        tempa[2] = tempa[3];
                        tempa[3] = u8tmp;
                    }

                    // SubWord() is a function that takes a four-byte input word and 
                    // applies the S-box to each of the four bytes to produce an output word.

                    // Function Subword()
                    {
                        tempa[0] = getSBoxValue(tempa[0]);
                        tempa[1] = getSBoxValue(tempa[1]);
                        tempa[2] = getSBoxValue(tempa[2]);
                        tempa[3] = getSBoxValue(tempa[3]);
                    }

                    tempa[0] = tempa[0] ^ Rcon[i/Nk];
                }

                j = i * 4; k=(i - Nk) * 4;
                RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
                RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
                RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
                RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
            }

        }

        // This function adds the round key to state.
        // The round key is added to the state by an XOR function.
        void AES_impl::AddRoundKey(uint8_t round, state_t* state, const uint8_t* RoundKey) {
            uint8_t i,j;
            for (i = 0; i < 4; ++i) {
                for (j = 0; j < 4; ++j) {
                    (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
                }
            }
        }

        // The SubBytes Function Substitutes the values in the
        // state matrix with values in an S-box.
        void AES_impl::SubBytes(state_t* state) {
            uint8_t i, j;
            for (i = 0; i < 4; ++i) {
                for (j = 0; j < 4; ++j) {
                    (*state)[j][i] = getSBoxValue((*state)[j][i]);
                }
            }
        }

        // The ShiftRows() function shifts the rows in the state to the left.
        // Each row is shifted with different offset.
        // Offset = Row number. So the first row is not shifted.
        void AES_impl::ShiftRows(state_t* state) {
            uint8_t temp;

            // Rotate first row 1 columns to left  
            temp           = (*state)[0][1];
            (*state)[0][1] = (*state)[1][1];
            (*state)[1][1] = (*state)[2][1];
            (*state)[2][1] = (*state)[3][1];
            (*state)[3][1] = temp;

            // Rotate second row 2 columns to left  
            temp           = (*state)[0][2];
            (*state)[0][2] = (*state)[2][2];
            (*state)[2][2] = temp;

            temp           = (*state)[1][2];
            (*state)[1][2] = (*state)[3][2];
            (*state)[3][2] = temp;

            // Rotate third row 3 columns to left
            temp           = (*state)[0][3];
            (*state)[0][3] = (*state)[3][3];
            (*state)[3][3] = (*state)[2][3];
            (*state)[2][3] = (*state)[1][3];
            (*state)[1][3] = temp;
        }

        uint8_t AES_impl::xtime(uint8_t x) {
            return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
        }

        // MixColumns function mixes the columns of the state matrix
        void AES_impl::MixColumns(state_t* state) {
            uint8_t i;
            uint8_t Tmp, Tm, t;

            for (i = 0; i < 4; ++i) {  
                t   = (*state)[i][0];
                Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
                Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
                Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
                Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
                Tm  = (*state)[i][3] ^ t ;              Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
            }
        }

        // Declaring Multiply as a function reduces code size considerably with the Keil ARM compiler.
        // See this link for more information: https://github.com/kokke/tiny-AES-C/pull/3
        // Multiply is used to multiply numbers in the field GF(2^8)
        // Note: The last call to xtime() is unneeded, but often ends up generating a smaller binary
        //       The compiler seems to be able to vectorize the operation better this way.
        //       See https://github.com/kokke/tiny-AES-c/pull/34
        uint8_t AES_impl::Multiply(uint8_t x, uint8_t y) {
            return  (((y & 1) * x) ^
                    ((y>>1 & 1) * xtime(x)) ^
                    ((y>>2 & 1) * xtime(xtime(x))) ^
                    ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
                    ((y>>4 & 1) * xtime(xtime(xtime(xtime(x)))))); /* this last call to xtime() can be omitted */
        }

        // MixColumns function mixes the columns of the state matrix.
        // The method used to multiply may be difficult to understand for the inexperienced.
        // Please use the references to gain more information.
        void AES_impl::InvMixColumns(state_t* state) {
            int i;
            uint8_t a, b, c, d;
            
            for (i = 0; i < 4; ++i) { 
                a = (*state)[i][0];
                b = (*state)[i][1];
                c = (*state)[i][2];
                d = (*state)[i][3];

                (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
                (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
                (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
                (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
            }
        }

        // The SubBytes Function Substitutes the values in the
        // state matrix with values in an S-box.
        void AES_impl::InvSubBytes(state_t* state) {
            uint8_t i, j;
            for (i = 0; i < 4; ++i) {
                for (j = 0; j < 4; ++j) {
                    (*state)[j][i] = getSBoxInvert((*state)[j][i]);
                }
            }
        }


        void AES_impl::InvShiftRows(state_t* state) {
            uint8_t temp;

            // Rotate first row 1 columns to right  
            temp = (*state)[3][1];
            (*state)[3][1] = (*state)[2][1];
            (*state)[2][1] = (*state)[1][1];
            (*state)[1][1] = (*state)[0][1];
            (*state)[0][1] = temp;

            // Rotate second row 2 columns to right 
            temp = (*state)[0][2];
            (*state)[0][2] = (*state)[2][2];
            (*state)[2][2] = temp;

            temp = (*state)[1][2];
            (*state)[1][2] = (*state)[3][2];
            (*state)[3][2] = temp;

            // Rotate third row 3 columns to right
            temp = (*state)[0][3];
            (*state)[0][3] = (*state)[1][3];
            (*state)[1][3] = (*state)[2][3];
            (*state)[2][3] = (*state)[3][3];
            (*state)[3][3] = temp;
        }

        // Cipher is the main function that encrypts the PlainText.
        void AES_impl::Cipher(state_t* state, const uint8_t* RoundKey) {
            uint8_t round = 0;

            // Add the First round key to the state before starting the rounds.
            AddRoundKey(0, state, RoundKey); 

            // There will be Nr rounds.
            // The first Nr-1 rounds are identical.
            // These Nr-1 rounds are executed in the loop below.
            for (round = 1; round < Nr; ++round) {
                SubBytes(state);
                ShiftRows(state);
                MixColumns(state);
                AddRoundKey(round, state, RoundKey);
            }

            // The last round is given below.
            // The MixColumns function is not here in the last round.
            SubBytes(state);
            ShiftRows(state);
            AddRoundKey(Nr, state, RoundKey);
        }

        void AES_impl::InvCipher(state_t* state, const uint8_t* RoundKey) {
            uint8_t round = 0;

            // Add the First round key to the state before starting the rounds.
            AddRoundKey(Nr, state, RoundKey); 

            // There will be Nr rounds.
            // The first Nr-1 rounds are identical.
            // These Nr-1 rounds are executed in the loop below.
            for (round = (Nr - 1); round > 0; --round) {
                InvShiftRows(state);
                InvSubBytes(state);
                AddRoundKey(round, state, RoundKey);
                InvMixColumns(state);
            }

            // The last round is given below.
            // The MixColumns function is not here in the last round.
            InvShiftRows(state);
            InvSubBytes(state);
            AddRoundKey(0, state, RoundKey);
        }

        void AES_impl::XorWithIv(uint8_t* buf, const uint8_t* Iv) {
            uint8_t i;

            for (i = 0; i < AES_BLOCKLEN; ++i) { // The block in AES is always 128bit no matter the key size
                buf[i] ^= Iv[i];
            }
        }

        // Char array -> byte array conversion
        std::pair<uint8_t*, int> AES_impl::chararray_to_bytearray(const char* arr, int len) {
            uint8_t* bytearr = new uint8_t[len];

            for (int i = 0; i < len; i++) {
                bytearr[i] = (arr[i] & 0xFF);
            }

            return std::make_pair(bytearr, len);
        }

        // Byte array -> char array conversion
        std::pair<char*, int> AES_impl::bytearray_to_chararray(const uint8_t* bytearr, int len) {
            char* arr = new char[len];

            for (int i = 0; i < len; i++) {
                arr[i] = static_cast<char>(bytearr[i] & 0xFF);
            }

            return std::make_pair(arr, len);
        }


        void AES_impl::AES_init(const uint8_t* key) {
            KeyExpansion(d_round_key, key);
        }

        void AES_impl::AES_init_iv(const uint8_t* key, const uint8_t* iv) {
            KeyExpansion(d_round_key, key);
            memcpy(d_iv, iv, AES_BLOCKLEN);
        }

        void AES_impl::AES_set_iv(const uint8_t* iv) {
            memcpy (d_iv, iv, AES_BLOCKLEN);
        }


        void AES_impl::AES_CBC_encrypt_buffer(uint8_t* buf, uint32_t length) {
            uintptr_t i;
            uint8_t *Iv = d_iv;
            
            for (i = 0; i < length; i += AES_BLOCKLEN) {
                XorWithIv(buf, Iv);
                Cipher((state_t*)buf, d_round_key);
                Iv = buf;
                buf += AES_BLOCKLEN;
                //printf("Step %d - %d", i/16, i);
            }

            /* store Iv in ctx for next call */
            memcpy(d_iv, Iv, AES_BLOCKLEN);
        }

        void AES_impl::AES_CBC_decrypt_buffer(uint8_t* buf,  uint32_t length) {
            uintptr_t i;
            uint8_t storeNextIv[AES_BLOCKLEN];
            
            for (i = 0; i < length; i += AES_BLOCKLEN) {
                memcpy(storeNextIv, buf, AES_BLOCKLEN);
                InvCipher((state_t*)buf, d_round_key);
                XorWithIv(buf, d_iv);
                memcpy(d_iv, storeNextIv, AES_BLOCKLEN);
                buf += AES_BLOCKLEN;
            }

        }


        std::pair<uint8_t*, int> AES_impl::encrypt(const char* msg, int len) {
            // String length has to be a multiple of Block length (16)
            int rem = 16 - (len % 16);

            // Last two characters tell how many zeros are added
            // If the number of zeros added are less than two, make room for two numbers by increasing padding
            if (rem == 0) {
                rem = 16;
            }
            if (rem == 1) {
                rem = 17;
            }

            // Create a new array with the proper text length
            // Added 0s at the end to make string length a multiple of 16
            // Last two characters contain the amount of 0s added
            int new_msg_size = len + rem;
            char new_msg[new_msg_size];

            memcpy(new_msg, msg, len);
            memset((new_msg + len), '0', rem);

            // for (int i = 0; i < len; i++) {
            //     new_msg[i] = msg[i];
            // }

            // for (int i = len; i < (len + rem); i++) {
            //     new_msg[i] = '0';
            // }

            // Last two chars indicate the number of padding added
            // Convert rem to strings so that it can be added to the char array
            std::string rem_str = std::to_string(rem);
            
            if (rem < 10) {
                new_msg[new_msg_size-1] = rem_str[0];
            }
            else {
                new_msg[new_msg_size-2] = rem_str[0];
                new_msg[new_msg_size-1] = rem_str[1];   
            }

            // std::cout << "rem = " << rem << std::endl;
            // std::cout << "From encrypt(): modified size: " << new_msg_size << ", msg: ";
            // printCharArray(new_msg, new_msg_size);
            // std::cout << std::endl;

            // Encrypt
            std::pair<uint8_t*, int> in = chararray_to_bytearray(new_msg, new_msg_size);

            AES_init_iv(KEY, IV);

            AES_CBC_encrypt_buffer(in.first, in.second);

            // Output
            return std::make_pair(in.first, in.second);
        }

        std::pair<char*, int> AES_impl::decrypt(const char* in, int len) {
            // Convert to byte array
            // uint8_t array will be decrypted
            std::pair<uint8_t*, int> dec_buf = chararray_to_bytearray(in, len);

            // Decrypt
            AES_init_iv(KEY, IV);
            AES_CBC_decrypt_buffer(dec_buf.first, dec_buf.second);

            std::pair<char*, int> dec_msg_pair = bytearray_to_chararray(dec_buf.first, dec_buf.second);

            // Free dec_buf bytearray
            delete dec_buf.first;

            // Last two chars contain the number of 0s added to make the string length a multiple of 16
            // Find the padding amount
            std::string padding_amount_str = "";
            padding_amount_str += dec_msg_pair.first[dec_msg_pair.second - 2];
            padding_amount_str += dec_msg_pair.first[dec_msg_pair.second - 1];

            //std::cout << "padding_amount_str = " << padding_amount_str << std::endl;

            int padding_amount = std::stoi(padding_amount_str);

            // Retrieve the original string
            int msg_size = dec_msg_pair.second - padding_amount;

            char* dec_msg = new char[msg_size];
            for (int i = 0; i < msg_size; i++) {
                dec_msg[i] = dec_msg_pair.first[i];
            }

            // Free dec_msg_pair array
            delete dec_msg_pair.first;

            return std::make_pair(dec_msg, msg_size);
        }

    } /* namespace IEEE_802_15_4_CustomBlocks */
} /* namespace gr */

