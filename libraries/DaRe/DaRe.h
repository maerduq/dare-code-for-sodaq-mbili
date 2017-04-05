/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2017 Semtech
	
Description: General functions for DaRe
License: Revised BSD License, see LICENSE file included in the project
By: Paul Marcelis
*/

#ifndef __DARE_H
#define __DARE_H

#define DARE_MAX_W 64
//#define CONVENTIONAL_CODING

class DaRe {
public:
  enum R_VALUE { R_1_2, R_1_3, R_1_4, R_1_5 }; // Coding rate
  enum W_VALUE { W_0, W_1, W_2, W_4, W_8, W_16, W_32, W_64 }; // Window size
  struct Payload {
    uint8_t *payload;
    uint8_t payloadSize;
  };

  static bool *prlg(uint8_t W, uint32_t fcntup, uint8_t R);
  static uint8_t prng(uint8_t max, uint32_t index, uint32_t seed);
  static uint8_t getW(W_VALUE);
  static uint8_t getR(R_VALUE);
  static double w2d(uint8_t W);
  static uint8_t getWindowSize(uint8_t W, uint32_t fcntup);
};

#define W2D_A 0.75
#define W2D_B -0.0625
#define W2D_C 0.25

#endif
