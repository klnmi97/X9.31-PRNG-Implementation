/*
---------------------------------------------------------------------------
Copyright (c) 2020, Mykhailo Klunko.

The redistribution and use of this software (with or without changes)
is allowed without the payment of fees or royalties provided that:

  source code distributions include the above copyright notice, this
  list of conditions and the following disclaimer;

  binary distributions include the above copyright notice, this list
  of conditions and the following disclaimer in their documentation.

This software is provided 'as is' with no explicit or implied warranties
in respect of its operation, including, but not limited to, correctness
and fitness for purpose.
---------------------------------------------------------------------------

 This is an implementation of X9.31 Pseudorandom number generator (PRNG)
 which is based on the AES implementation by Brian Gladman.
 The mentioned AES project can be found here: https://github.com/BrianGladman/aes.
*/

#include <iostream>
#include "math.h"
#include "deps/include/aes.h"

#define BLOCK_SIZE 16

static unsigned char *I;
static unsigned char *R;
static unsigned char *DT;
static unsigned char V[BLOCK_SIZE];
static unsigned char K[BLOCK_SIZE];

/**
 * @brief Converts char numbers to hex numbers with the same value
 * @param value pointer to array of char numbers to be changed
 */
void char_to_hex(unsigned char* value) {
    for (int i = 0; i < strlen((char*)value); i++) {
        value[i] = value[i] - '0';
    }
}

/**
 * @brief xor to values. All the input values should be the same size
 * @param a input value
 * @param b input value
 * @param out output array
 * @param len size of input and output values
 */
void xor_char(unsigned char* a, unsigned char* b, unsigned char* out, int len) {
    for (int i = len - 1; i >= 0; i--) {
        out[i] = a[i] ^ b[i];
    }
}

/**
 * @brief Increment DT vector value
 */
void increment_DT() {
    int current_block = BLOCK_SIZE - 1;
    while (current_block >= 0) {
        DT[current_block]++;
        if (DT[current_block] == 0) {
            current_block--;
        }
        else {
            break;
        }

    }
}

/**
 * @brief Initialize inner state of the X9.31 PRNG
 * @param seed 128-bit value of seed
 * @param key 128-bit value for key
 */
void init_prng(unsigned char* seed, unsigned char* key) {
    memcpy(V, seed, BLOCK_SIZE);
    memcpy(K, key, BLOCK_SIZE);
    DT = (unsigned char*) malloc(16 * sizeof(unsigned char));
    memset(DT, 0, 16);
    I = (unsigned char*) malloc(16 * sizeof(unsigned char));
    R = (unsigned char*) malloc(16 * sizeof(unsigned char));
}

/**
 * @brief generate 128 bits of pseudo random data
 * @return 128 bits of random data contained in the array
 */
unsigned char* x931_rand() {

    unsigned char IXV[16], RXI[16];


    aes_encrypt_ctx ctx[1];
    aes_encrypt_key128(K, ctx);

    // I = E_K(DT)
    aes_ecb_encrypt(DT, I, 16, ctx);
    // R = E_K(I xor V)
    xor_char(I, V, IXV, 16);
    aes_ecb_encrypt(IXV, R, 16, ctx);
    // V = E_K(R xor I)
    xor_char(R, I, RXI, 16);
    aes_ecb_encrypt(RXI, V, 16, ctx);
    // update DT for the next iteration
    increment_DT();

    return R;
}

int main()
{

    // TODO: provide custom seed+key
    char seed[17] = "2328862328862328";
    char key[17] = "8232688232688232";

    char_to_hex((unsigned char*)seed);
    char_to_hex((unsigned char*)key);

    init_prng((unsigned char*)seed, (unsigned char*)key);

    FILE* file_ptr;
    file_ptr = fopen("F.bin", "wb");

    int file_size = (int)pow(10, 9) / (BLOCK_SIZE * 8);

    for (int i = 0; i < file_size; i++) {
        auto res = x931_rand();
        fwrite(res, sizeof(res[0]), BLOCK_SIZE, file_ptr);
    }
    fclose(file_ptr);
}