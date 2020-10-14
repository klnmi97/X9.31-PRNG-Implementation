// PRNG.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "math.h"
#include "deps/include/aes.h"

#define BLOCK_SIZE 16

static unsigned char* I;
static unsigned char* R;
static unsigned char* V;
static unsigned char* DT;
static unsigned char* K;


void char_to_hex(unsigned char* value) {
    for (int i = 0; i < strlen((char*)value); i++) {
        value[i] = value[i] - '0';
    }
}

void xor_char(unsigned char* a, unsigned char* b, unsigned char* out, int len) {
    for (int i = len - 1; i >= 0; i--) {
        out[i] = a[i] ^ b[i];
    }
}

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

void init_prng(unsigned char* seed, unsigned char* key) {
    V = seed;
    K = key;
    unsigned char init_value[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    DT = new unsigned char[16];
    memset(DT, 0, 16);
    I = new unsigned char[16];
    R = new unsigned char[16];
}

unsigned char* x931_rand() {

    aes_encrypt_ctx ctx[1];
    aes_encrypt_key128(K, ctx);

    // I = E_K(DT)
    aes_ecb_encrypt(DT, I, 16, ctx);
    // R = E_K(I xor V)
    unsigned char* I_xor_V = new unsigned char[16];
    xor_char(I, V, I_xor_V, 16);
    aes_ecb_encrypt(I_xor_V, R, 16, ctx);
    // V = E_K(R xor I)
    unsigned char* R_x_I = new unsigned char[16];
    xor_char(R, I, R_x_I, 16);
    aes_ecb_encrypt(R_x_I, V, 16, ctx);
    // update DT for the next iteration
    increment_DT();

    return R;
}

int main()
{
    unsigned long l_seed = 0x2328862328862328;
    unsigned long l_key = 0x8232688232688232;

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