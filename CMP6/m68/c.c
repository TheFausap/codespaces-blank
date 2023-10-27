#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RSIZE 32
#define WSIZE 8

uint8_t D0[32], D1[32];
uint8_t D2[32], D3[32];
uint8_t D4[32], D5[32];
uint8_t D6[32], D7[32];

uint8_t A0[32], A1[32];
uint8_t A2[32], A3[32];
uint8_t A4[32], A5[32];
uint8_t A6[32], A7[32];

uint8_t PC[32];
uint8_t SR[16];

void i0(void)
{
    for(int i=0;i<RSIZE;i++) {
        A0[i]=0;A1[i]=0;
        A2[i]=0;A3[i]=0;
        A4[i]=0;A5[i]=0;
        A6[i]=0;A7[i]=0;

        D0[i]=0;D1[i]=0;
        D2[i]=0;D3[i]=0;
        D4[i]=0;D5[i]=0;
        D6[i]=0;D7[i]=0;

        PC[i]=0;
    }
    for(int i=0;i<16;i++) SR[i]=0;
}