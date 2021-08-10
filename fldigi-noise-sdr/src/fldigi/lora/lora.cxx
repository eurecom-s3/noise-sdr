/* -*- c++ -*- */
/*
 * Copyright 2019 Joachim Tapparel TCL@EPFL.
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
 *
 * Modified by Giovanni Camurati to make it a library for fldigi-noise-sdr
 *
 */

#include "lora.h"
#include "AndFlmsg_Fldigi_Interface.h"
#include "misc.h"
#include "modem.h"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

void lora::tx_init() {}

void lora::rx_init() {}

void lora::restart() {}

void lora::init() { modem::init(); }

lora::~lora() {}

lora::lora(int loramode) : modem() {
  // state
  txstate = TX_STATE_GET_PAYLOAD;
  payload_cnt = 0;
  end = false;

  // config
  samplerate = 8000 * OVERSAMPLING;
  implicit_header = false;
  has_crc = true;
  simple = false;
  switch (loramode) {
  case MODE_LORA_8000_7_4:
    bw = 8000;
    sf = 7;
    cr = 4;
    break;
  case MODE_LORA_8000_8_4:
    bw = 8000;
    sf = 8;
    cr = 4;
    break;
  case MODE_LORA_8000_8_SIMPLE:
    bw = 8000;
    sf = 8;
    cr = 0;
    implicit_header = false;
    has_crc = true;
    simple = true;
    break;
  default:
    bw = 8000;
    sf = 7;
    cr = 4;
    break;
  }
}

//=============================================================================
//=========================== lora receive routines ==========================
//=============================================================================

int lora::rx_process(const short *buf, int len) { return 0; }

//=====================================================================
// transmit processes
//=====================================================================

void lora::whithen() {
  for (int i = 0; i < payload_cnt; i++) {
    payload_w[2 * i] = (payload[i] ^ whitening_seq[i]) & 0x0F;
    payload_w[2 * i + 1] = (payload[i] ^ whitening_seq[i]) >> 4;
  }
  len = 2*payload_cnt;
  // for(int i=0;i<2*payload_cnt;i++)
  // printf("%d ", payload_w[i]);
  // printf("\n");
}

void lora::compute_header(int* data) {
    data[0] = payload_cnt >> 4;
    data[1] = payload_cnt & 0x0F;
    data[2] = (cr << 1) | has_crc;

    // header checksum
    bool c4 = (data[0] & 0b1000) >> 3 ^ (data[0] & 0b0100) >> 2 ^
              (data[0] & 0b0010) >> 1 ^ (data[0] & 0b0001);
    bool c3 = (data[0] & 0b1000) >> 3 ^ (data[1] & 0b1000) >> 3 ^
              (data[1] & 0b0100) >> 2 ^ (data[1] & 0b0010) >> 1 ^
              (data[2] & 0b0001);
    bool c2 = (data[0] & 0b0100) >> 2 ^ (data[1] & 0b1000) >> 3 ^
              (data[1] & 0b0001) ^ (data[2] & 0b1000) >> 3 ^
              (data[2] & 0b0010) >> 1;
    bool c1 = (data[0] & 0b0010) >> 1 ^ (data[1] & 0b0100) >> 2 ^
              (data[1] & 0b0001) ^ (data[2] & 0b0100) >> 2 ^
              (data[2] & 0b0010) >> 1 ^ (data[2] & 0b0001);
    bool c0 = (data[0] & 0b0001) ^ (data[1] & 0b0010) >> 1 ^
              (data[2] & 0b1000) >> 3 ^ (data[2] & 0b0100) >> 2 ^
              (data[2] & 0b0010) >> 1 ^ (data[2] & 0b0001);

    data[3] = c4;
    data[4] = c3 << 3 | c2 << 2 | c1 << 1 | c0;
    //printf("HDR ");
    //for(int i=0; i<5; i++)
        //printf("%x ", data[i]);
    //printf("\n");
}

void lora::add_header() {
  //len = 2 * payload_cnt;
  if (implicit_header) {
    for (int i = 0; i < len; i++)
      data[i] = payload_w[i];
  } else {
    compute_header(data);
    for (int i = 0; i < len; i++)
      data[i + 5] = payload_w[i];
    len += 5;
  }
  // for(int i=0;i<len;i++)
  // printf("%d ", data[i]);
  // printf("\n");
}

unsigned int lora::crc16(unsigned int crcValue, unsigned char newByte) {
  unsigned char i;
  for (i = 0; i < 8; i++) {

    if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)) {
      crcValue = (crcValue << 1) ^ 0x1021;
    } else {
      crcValue = (crcValue << 1);
    }
    newByte <<= 1;
  }
  return crcValue;
}

void lora::add_crc() {
  if (has_crc) {
    unsigned int crc = 0x0000;
    // calculate CRC on the N-2 firsts data bytes using Poly=1021 Init=0000
    for (int i = 0; i < (int)payload_cnt - 2; i++)
      crc = crc16(crc, payload[i]);

    // XOR the obtained CRC with the last 2 data bytes
    crc = crc ^ payload[payload_cnt - 1] ^ (payload[payload_cnt - 2] << 8);

    data[len] = ((crc & 0x000F));
    data[len + 1] = ((crc & 0x00F0) >> 4);
    data[len + 2] = ((crc & 0x0F00) >> 8);
    data[len + 3] = ((crc & 0xF000) >> 12);
    //for(int i=0; i<4;i++){
      //printf("%x ", data[i+len]);
    //}
    //printf("\n");
    len += 4;
  }
}

/**
 *  \brief  return the modulus a%b between 0 and (b-1)
 */
inline long mod(long a, long b) { return (a % b + b) % b; }
/**
 *  \brief  Convert an integer into a MSB first vector of bool
 *
 *  \param  integer
 *          The integer to convert
 *  \param  n_bits
 *          The output number of bits
 */
inline std::vector<bool> int2bool(unsigned int integer, uint8_t n_bits) {
  std::vector<bool> vec(n_bits, 0);
  int j = n_bits;
  for (int i = 0; i < n_bits; i++) {
    vec[--j] = ((integer >> i) & 1);
  }
  return vec;
};
/**
 *  \brief  Convert a MSB first vector of bool to a integer
 *
 *  \param  b
 *          The boolean vector to convert
 */
inline uint32_t bool2int(std::vector<bool> b) {
  uint32_t integer = std::accumulate(b.begin(), b.end(), 0,
                                     [](int x, int y) { return (x << 1) + y; });
  return integer;
};

void lora::hamming_enc() {
  std::vector<bool> data_bin;
  bool p0, p1, p2, p3, p4;
  for (int i = 0; i < len; i++) {
    unsigned char cr_app = (i < sf - 2) ? 4 : cr;
    data_bin = int2bool(data[i], 4);

    // the data_bin is msb first
    if (cr_app != 1) { // need hamming parity bits
      p0 = data_bin[3] ^ data_bin[2] ^ data_bin[1];
      p1 = data_bin[2] ^ data_bin[1] ^ data_bin[0];
      p2 = data_bin[3] ^ data_bin[2] ^ data_bin[0];
      p3 = data_bin[3] ^ data_bin[1] ^ data_bin[0];
      // we put the data LSB first and append the parity bits
      data[i] = (data_bin[3] << 7 | data_bin[2] << 6 | data_bin[1] << 5 |
                 data_bin[0] << 4 | p0 << 3 | p1 << 2 | p2 << 1 | p3) >>
                (4 - cr_app);

    } else { // coding rate = 4/5 we add a parity bit
      p4 = data_bin[0] ^ data_bin[1] ^ data_bin[2] ^ data_bin[3];
      data[i] = (data_bin[3] << 4 | data_bin[2] << 3 | data_bin[1] << 2 |
                 data_bin[0] << 1 | p4);
    }
  }
  // for(int i=0; i<len; i++)
  // printf("%d ", data[i]);
  // printf("\n");
}

void lora::interleave() {
  int cw_cnt = 0;
  int consumed = 0;
  unsigned char ppm;
  unsigned char sf_app;
  len_i = 0;
  while (consumed < len) {
    ppm = 4 + ((cw_cnt < sf - 2) ? 4 : cr);
    sf_app = (cw_cnt < sf - 2) ? sf - 2 : sf;

    // Create the empty matrices
    std::vector<std::vector<bool>> cw_bin(sf_app);
    std::vector<bool> init_bit(sf, 0);
    std::vector<std::vector<bool>> inter_bin(ppm, init_bit);

    // convert to input codewords to binary vector of vector
    for (int i = 0; i < sf_app; i++) {
      if (i >= len)
        cw_bin[i] = int2bool(0, ppm);
      else
        cw_bin[i] = int2bool(data[consumed + i], ppm);
      // printf("%d ", consumed+i);
      cw_cnt++;
    }

    // std::cout<<"codewords---- "<<std::endl;
    // for (uint32_t i =0u ; i<sf_app ;i++){
    // for(int j=0;j<int(ppm);j++){
    // std::cout<<cw_bin[i][j];
    //}
    // std::cout<<" 0x"<<std::hex<<(int)data[consumed+i]<<std::dec<< std::endl;
    //}
    // std::cout<<std::endl;

    // Do the actual interleaving
    for (int i = 0; i < ppm; i++) {
      for (int j = 0; j < int(sf_app); j++) {
        inter_bin[i][j] = cw_bin[mod((i - j - 1), sf_app)][i];
      }
      // For the first bloc we add a parity bit and a zero in the end of the
      // lora symbol(reduced rate)
      if (cw_cnt == sf - 2)
        inter_bin[i][sf_app] =
            accumulate(inter_bin[i].begin(), inter_bin[i].end(), 0) % 2;

      data_i[len_i + i] = bool2int(inter_bin[i]);
    }

    // std::cout<<"interleaved------"  <<std::endl;
    // for (uint32_t i =0u ; i<ppm ;i++){
    // for(int j=0;j<int(sf);j++){
    // std::cout<<inter_bin[i][j];
    //}
    // std::cout<<" "<<data_i[len_i+i]<< std::endl;
    //}
    // std::cout<<std::endl;

    consumed = cw_cnt;
    len_i += ppm;
  }

  // printf("%d %d\n", len, len_i);
  len = len_i;
}

void lora::gray_enc() {
  int g;
  for (int i = 0; i < len_i; i++) {
    // g = (data_i[i] ^ (data_i[i] >> 1u));
    g = data_i[i];
    for (int j = 1; j < sf; j++) {
      g = g ^ (data_i[i] >> j);
    }
    // do the shift of 1
    g = mod(g + 1, (1 << sf));

    // printf("%d\n", g);
    //std::cout << std::hex << "0x" << data_i[i] << " ---> "
              //<< "0x" << g << std::dec << std::endl;
    data_i[i] = g;
  }
}

// https://arxiv.org/pdf/1905.11252.pdf
double lora::get_phase(unsigned int n, unsigned int s) {
  double N = 1 << sf;
  double x;
  double y = bw / samplerate; // 1 / samples_per_chip
  int n_fold = N - s;
  double phase = 0;

  if (y * n < n_fold)
    x = -1 / 2;
  else
    x = +3 / 2;

  phase = 2 * M_PI * y * n * (y * n / (2 * N) + (s / N - x));
  return phase;
}

/**
 *  \brief  Return the reference chirps using s_f=bw
 *
 *  \param  upchirp
 *          The pointer to the reference upchirp
 *  \param  downchirp
 *          The pointer to the reference downchirp
 * \param   sf
 *          The spreading factor to use
 */
// inline void build_ref_chirps(cmplx* upchirp, cmplx* downchirp, uint8_t sf){
inline void lora::build_ref_chirps(double *upchirp, double *downchirp) {
  double phase;
  double freq = get_txfreq() - bw / 2;
  for (int i = 0; i < samples_per_symbol; i++) {
    double offset = get_phase(i, 0);
    phase = 2 * M_PI * freq * i / samplerate + offset;
    upchirp[i] = cos(phase);
    phase = 2 * M_PI * (freq + bw) * i / samplerate - offset;
    downchirp[i] = cos(phase);
  }

  // for(unsigned int n = 0; n < N; n++){
  ////the scaling factor of 0.9 is here to avoid to saturate the USRP_SINK
  ////upchirp[n] =  cmplx(0.9f, 0.0f)*gr_expj(2.0 * M_PI * (n*n/(2*N)-0.5*n));
  ////downchirp[n] = cmplx(0.9f, 0.0f)*gr_expj(-2.0 * M_PI * (n*n/(2*N)-0.5*n));
  // phase = 2.0 * M_PI * (n*n/(2*N)-0.5*n);
  // upchirp[n] = cmplx(cos(phase), sin(phase));
  ////upchirp[n] = cos(phase);
  // phase = -2.0 * M_PI * (n*n/(2*N)-0.5*n);
  // downchirp[n] = cmplx(cos(phase), sin(phase));
  ////downchirp[n] = cos(phase);
  //}
}

/**
 *  \brief  Return an modulated upchirp using s_f=bw
 *
 *  \param  chirp
 *          The pointer to the modulated upchirp
 *  \param  id
 *          The number used to modulate the chirp
 * \param   sf
 *          The spreading factor to use
 */
// inline void build_upchirp(cmplx* chirp, uint32_t id, uint8_t sf){
inline void lora::build_upchirp(double *chirp, uint32_t id) {
  //double N = 1 << sf;
  double phase;
  // for(unsigned int n = 0; n < N; n++){
  // the scaling factor of 0.9 is here to avoid to saturate the USRP_SINK
  // chirp[n]=gr_complex(0.9f, 0.0f)*gr_expj(2.0 * M_PI
  // *(n*n/(2*N)+(id/N-0.5)*n));
  // phase = 2.0 * M_PI * (0 + (n*n/(2*N)+(id/N-0.5)*n));
  // chirp[n] = cmplx(cos(phase), sin(phase));

  double freq = get_txfreq() - bw / 2;
  for (int i = 0; i < samples_per_symbol; i++) {
    double offset = get_phase(i, id);
    phase = 2 * M_PI * freq * i / samplerate + offset;
    chirp[i] = cos(phase);
  }
  //}
}

void lora::modulate() {
  number_of_bins = (uint32_t)(1u << sf);
  symbols_per_second = (double)bw / number_of_bins;
  samples_per_symbol = (uint32_t)(samplerate / symbols_per_second);

  downchirp.resize(samples_per_symbol);
  upchirp.resize(samples_per_symbol);
  symbol.resize(samples_per_symbol);

  build_ref_chirps(&upchirp[0], &downchirp[0]);

  n_up = 8;
  symb_cnt = 0;

  int sample = 0;
  while (symb_cnt < len_i + 13) {
    if (sample >= OUTBUFSIZE - samples_per_symbol) {
      fprintf(stderr, "Error, buffer too small for LoRa samples");
      abort();
    }

    if (symb_cnt < n_up + 4.25) { // send preamble
      if (symb_cnt == 0) {        // offset
        unsigned int off = 0;
        memcpy(&outbuf[sample], &upchirp[0],
               samples_per_symbol * sizeof(double));
        sample += samples_per_symbol + off;
      } else if (symb_cnt < n_up) { // upchirps
        memcpy(&outbuf[sample], &upchirp[0],
               samples_per_symbol * sizeof(double));
        sample += samples_per_symbol;
      } else if (symb_cnt == n_up) { // network identifier 1
        build_upchirp(&outbuf[sample], 8);
        sample += samples_per_symbol;
      } else if (symb_cnt == n_up + 1) { // network identifier 2
        build_upchirp(&outbuf[sample], 16);
        sample += samples_per_symbol;
      } else if (symb_cnt < n_up + 4) { // downchirps
        memcpy(&outbuf[sample], &downchirp[0],
               samples_per_symbol * sizeof(double));
        sample += samples_per_symbol;
      } else { // quarter of downchirp
        memcpy(&outbuf[sample], &downchirp[0],
               samples_per_symbol / 4 * sizeof(double));
        sample += samples_per_symbol / 4;
      }
    } else { // payload
      //printf("Modulating %x\n", data_i[symb_cnt-13]);
      build_upchirp(&outbuf[sample], data_i[symb_cnt - 13]);
      sample += samples_per_symbol;
    }
    symb_cnt++;
  }

  // add short break before next packet
  for (int i = 0; i < floor(samplerate * 10e-3); i++) {
    outbuf[sample++] = 0;
  }

  ModulateXmtr(outbuf, sample);
}

void lora::simple_enc() {
  if(sf != 8){
    fprintf(stderr, "Error, sf !=8 not supported for now\n");
    abort();
  }
  
  for(int i=0; i<payload_cnt; i++)
      payload_w[i] = payload[i];
  len = payload_cnt;

  add_header();
  add_crc();

  if(has_crc){
      data[len-4] = data[len-4] + (data[len-3] << 4);
      data[len-3] = data[len-2] + (data[len-1] << 4);
      len = len - 2;
  }

  for(int i=0; i<len; i++){
    data_i[i] = data[i];
  }
  len_i = len;
  gray_enc();
}

int lora::tx_process() {
  int c;
  switch (txstate) {
  case TX_STATE_GET_PAYLOAD:
    if (payload_cnt > MAX_PKT_SIZE - 1 || end) {
      txstate = TX_STATE_PROCESS_PKT;
    } else {
      c = get_tx_char();
      if (c == GET_TX_CHAR_ETX) {
        end = true;
      } else {
        payload[payload_cnt] = c;
        payload_cnt++;
      }
    }
    break;
  case TX_STATE_PROCESS_PKT:
    printf("Start LoRa packet\n");
    // for(int i=0; i < payload_cnt; i++){
    // put_echo_char(payload[i]);
    //}
    if (simple) {
      simple_enc();
    } else {
      whithen();
      add_header();
      add_crc();
      hamming_enc();
      interleave();
      gray_enc();
    }
    modulate();
    if (end) {
      txstate = TX_STATE_END;
    } else {
      txstate = TX_STATE_GET_PAYLOAD;
    }
    payload_cnt = 0;
    break;
  case TX_STATE_END:
    return -1;
  }
  return 0;
}
