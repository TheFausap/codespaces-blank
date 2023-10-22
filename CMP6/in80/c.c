#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#define WSIZE 8
#define MSIZE 65536
typedef uint8_t word;

word A[WSIZE];
word B[WSIZE];
word C[WSIZE];
word D[WSIZE];
word E[WSIZE];
word H[WSIZE];
word L[WSIZE];
word FL[WSIZE];
word SP[2 * WSIZE];

word ONE[WSIZE];
word ZERO[WSIZE];
word ONES[WSIZE];
word ONES16[2 * WSIZE];

uint16_t PC;
char HLTF;
word dly;

word* M[MSIZE];

char isz(word* r)
{
    for (int i = 0; i < WSIZE; i++)
        if (r[i] != 0) return 0;
    return 1;
}

word cnt1(word* r)
{
    word s = 0;
    for (int i = 0; i < WSIZE; i++)
        if (r[i] == 1) s++;

    return s;
}

void mv(word* d, word* s)
{
    for (int i = 0; i < WSIZE; i++) d[i] = s[i];
}

void mvn(word* d, word* s, int ss, int se)
{
    for (int i = ss, j = 0; i <= se; i++, j++) d[j] = s[i];
}

void mvnd(word* d, word* s, int ss, int se, int ds)
{
    for (int i = ss, j = ds; i <= se; i++, j++) d[j] = s[i];
}

word b2u(word*);
void pw(word* v)
{
    printf("%02XH (", b2u(v));
    for (int i = 0; i < WSIZE; i++) printf("%d", v[i]);
    printf(")\n");
}

void pwf(void)
{
    printf("%02XH (", b2u(FL));
    for (int i = 0; i < WSIZE; i++) printf("%d", FL[i]);
    printf(")\n");
    printf("S: %d\tZ: %d\tA: %d\tP: %d\tC: %d\n", FL[0], FL[1], FL[3], FL[5], FL[7]);
}

word ab(word a, word b, word* c)
{
    word s;
    s = a + b + *c;
    if (s == 3) { s = 1; *c = 1; }
    else if (s == 2) { s = 0; *c = 1; }
    else *c = 0;
    return s;
}

void cmp(word* a)
{
    word c = 0;
    for (int i = 0; i < WSIZE; i++) a[i] = a[i] ^ 1;
    for (int i = WSIZE - 1; i >= 0; i--)
        a[i] = ab(a[i], ONE[i], &c);
}

word b2u(word* v)
{
    word r = 0;
    r = v[0] * 128U + v[1] * 64U + v[2] * 32U;
    r += v[3] * 16U + v[4] * 8U + v[5] * 4U;
    r += v[6] * 2U + v[7];
    return r;
}

uint16_t b2u16(word* h, word* l)
{
    uint16_t r = 0;
    r = h[0] * 32768 + h[1] * 16384 + h[2] * 8192;
    r += h[3] * 4096 + h[4] * 2048 + h[5] * 1024;
    r += h[6] * 512 + h[7] * 256;
    r += l[0] * 128 + l[1] * 64 + l[2] * 32;
    r += l[3] * 16 + l[4] * 8 + l[5] * 4;
    r += l[6] * 2 + l[7];
    return r;
}

uint16_t b162u16(word* h)
{
    uint16_t r = 0;
    r = h[0] * 32768 + h[1] * 16384 + h[2] * 8192;
    r += h[3] * 4096 + h[4] * 2048 + h[5] * 1024;
    r += h[6] * 512 + h[7] * 256;
    r += h[8] * 128 + h[9] * 64 + h[10] * 32;
    r += h[11] * 16 + h[12] * 8 + h[13] * 4;
    r += h[14] * 2 + h[15];
    return r;
}

void u16b(uint16_t v, word* h, word* l)
{
    for (int i = WSIZE - 1; i >= 0; i--) {
        l[i] = (word)v % 2;
        v /= 2;
    }
    for (int i = WSIZE - 1; i >= 0; i--) {
        h[i] = (word)v % 2;
        v /= 2;
    }
}

void u8b(uint8_t v, word* l)
{
    for (int i = WSIZE - 1; i >= 0; i--) {
        l[i] = (word)v % 2;
        v /= 2;
    }
}

void set(word* m, int v)
{
    word s = 0;
    if (v < 0) {
        v *= -1;
        s = 1;
    }
    for (int i = WSIZE - 1; i >= 0; i--) {
        m[i] = (word)v % 2;
        v /= 2;
    }
    if (s) cmp(m);
}

void set16(word* m, uint16_t v)
{
    for (int i = 2 * WSIZE - 1; i >= 0; i--) {
        m[i] = (word)v % 2;
        v /= 2;
    }
}

void push16(word* h, word* l)
{
    uint16_t r = 0;
    r = SP[0] * 32768 + SP[1] * 16384 + SP[2] * 8192;
    r += SP[3] * 4096 + SP[4] * 2048 + SP[5] * 1024;
    r += SP[6] * 512 + SP[7] * 256;
    r += SP[8] * 128 + SP[9] * 64 + SP[10] * 32;
    r += SP[11] * 16 + SP[12] * 8 + SP[13] * 4;
    r += SP[14] * 2 + SP[15];

    r--; mv(M[r], h);
    r--; mv(M[r], l);
    set16(SP, r);
}

void pop16(word* h, word* l)
{
    uint16_t r = 0;
    r = SP[0] * 32768U + SP[1] * 16384U + SP[2] * 8192U;
    r += SP[3] * 4096 + SP[4] * 2048 + SP[5] * 1024;
    r += SP[6] * 512 + SP[7] * 256;
    r += SP[8] * 128 + SP[9] * 64 + SP[10] * 32;
    r += SP[11] * 16 + SP[12] * 8 + SP[13] * 4;
    r += SP[14] * 2 + SP[15];
    mv(l, M[r]); r++;
    mv(h, M[r]); r++;
    set16(SP, r);
}

void i0(void)
{
    for (int i = 0; i < WSIZE; i++) {
        A[i] %= 2;
        B[i] %= 2; C[i] %= 2;
        D[i] %= 2; E[i] %= 2;
        H[i] %= 2; L[i] %= 2;
        ONE[i] = 0;
        FL[i] %= 2;
        ZERO[i] = 0;
        ONES[i] = 1;
    }
    for (int i = 0; i < 2 * WSIZE; i++) {
        SP[i] %= 2;
        ONES16[i] = 0;
        ONES16[i] = 1;
    }
    for (int i = 0; i < MSIZE; i++) {
        M[i] = calloc(WSIZE, sizeof(char));
    }
    ONE[WSIZE - 1] = 1;
    ONES16[2 * WSIZE - 1] = 1;
    PC = 0;
    FL[6] = 1; FL[2] = 0; FL[4] = 0;
    HLTF = 0; dly = 1;
    srand(time(0));
}

void _add(word* r, word* s, word* c)
{
    for (int i = WSIZE - 1; i >= 0; i--) {
        r[i] = ab(r[i], s[i], c);
        if (i == 3) FL[3] = *c;
    }
    FL[7] = *c;
    FL[0] = r[0];
    if (isz(r)) FL[1] = 1; else FL[1] = 0;
    FL[5] = (cnt1(r) % 2) ^ 1;
}

void _sub(word* r, word* ss, word* c)
{
    word* s, * oFL;

    if (isz(ss)) {
        FL[0] = r[0]; FL[3] = 0; FL[7] = 0;
        if (isz(r)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(r) % 2) ^ 1;
        return;
    }
    s = calloc(WSIZE, sizeof(char));
    oFL = calloc(WSIZE, sizeof(char));
    mv(s, ss);

    if (*c) {
        mv(oFL, FL);
        _add(s, ZERO, &c);
        *c = 0;
        mv(FL, oFL);
    }
    cmp(s);
    for (int i = WSIZE - 1; i >= 0; i--) {
        r[i] = ab(r[i], s[i], c);
        if (i == 3) FL[3] = *c;
    }
    FL[7] = (*c) ^ 1;
    FL[0] = r[0];
    if (isz(r)) FL[1] = 1; else FL[1] = 0;
    FL[5] = (cnt1(r) % 2) ^ 1;
}

void _add16(word* r, word* sh, word* sl, word* c, int rv)
{
    word s[2 * WSIZE] = {};
    s[0] = sh[0];
    s[1] = sh[1];
    s[2] = sh[2];
    s[3] = sh[3];
    s[4] = sh[4];
    s[5] = sh[5];
    s[6] = sh[6];
    s[7] = sh[7];
    s[8] = sl[0];
    s[9] = sl[1];
    s[10] = sl[2];
    s[11] = sl[3];
    s[12] = sl[4];
    s[13] = sl[5];
    s[14] = sl[6];
    s[15] = sl[7];

    for (int i = 2 * WSIZE - 1; i >= 0; i--) {
        s[i] = ab(r[i], s[i], c);
        if (i == 3) FL[3] = *c;
    }

    FL[7] = *c;
    FL[0] = r[0];
    if (isz(r)) FL[1] = 1; else FL[1] = 0;
    FL[5] = (cnt1(r) % 2) ^ 1;

    if (rv) {
        sh[0] = s[0];
        sh[1] = s[1];
        sh[2] = s[2];
        sh[3] = s[3];
        sh[4] = s[4];
        sh[5] = s[5];
        sh[6] = s[6];
        sh[7] = s[7];
        sl[0] = s[8];
        sl[1] = s[9];
        sl[2] = s[10];
        sl[3] = s[11];
        sl[4] = s[12];
        sl[5] = s[13];
        sl[6] = s[14];
        sl[7] = s[15];
    }
}

void _daa(void)
{
    word c = 0;
    word sl[] = { 0,0,0,0,0,1,1,0 };
    word sh[] = { 0,1,1,0,0,0,0,0 };
    int r = A[7] + A[6] * 2 + A[5] * 4 + A[4] * 8;
    if (r > 9) {
        _add(A, sl, &c);
    }
    r = A[3] + A[2] * 2 + A[1] * 4 + A[0] * 8;
    if (r > 9) {
        _add(A, sh, &c);
    }
}

void in(word t)
{
    int rr = 0;
    switch (t) {
    case 0:
        if (dly) {
            dly = 0;
            for (int i = 0; i < 1000000; i++) {}
            rr = rand();
            if (rr > 0x3ef) mv(A, (word*)"01100111");
            else mv(A, (word*)"10011100");
        }
        break;
    case 1:
        u8b(getc(stdin), A);
        dly = 1;
        break;
    default:
        break;
    }
}

void out(word t)
{
    switch (t) {
    case 1:
        if (dly == 0) {
            fprintf(stdout, "%c", b2u(A));
            fflush(stdout);
        }
        dly = 1;
        break;
    default:
        break;
    }
}

void exc(word* a)
{
    uint16_t ad = 0;
    word c = 0;
    word oFL[WSIZE];
    word t[WSIZE], th[WSIZE], tl[WSIZE];
    switch (b2u(a))
    {
    case 0x00:
    case 0x08:
    case 0x10:
    case 0x20:
    case 0x30:
    case 0x40:
    case 0x49:
    case 0x52:
    case 0x5B:
    case 0x64:
    case 0x6D:
    case 0x7F:
        PC++;
        break;
    case 0x50:
        mv(D, B);
        PC++;
        break;
    case 0x60:
        mv(H, B);
        PC++;
        break;
    case 0x70:
        ad = b2u16(H, L);
        mv(M[ad], B);
        PC++;
        break;
    case 0x80:
        _add(A, B, &c);
        PC++;
        break;
    case 0x90:
        _sub(A, B, &c);
        PC++;
        break;
    case 0xA0:
        for (int i = 0; i < WSIZE; i++) A[i] &= B[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB0:
        for (int i = 0; i < WSIZE; i++) A[i] |= B[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC0:
        // RNZ
        if (FL[1] == 0) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xD0:
        // RNC
        if (FL[7] == 0) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xE0:
        // RPO
        if (FL[5] == 0) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xF0:
        // RP
        if (FL[0] == 0) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 1:
        // LXI
        mv(C, M[++PC]);
        mv(B, M[++PC]);
        PC++;
        break;
    case 0x11:
        // LXI
        mv(E, M[++PC]);
        mv(D, M[++PC]);
        PC++;
        break;
    case 0x21:
        // LXI
        mv(L, M[++PC]);
        mv(H, M[++PC]);
        PC++;
        break;
    case 0x31:
        // LXI
        SP[0] = M[PC + 2][0];
        SP[1] = M[PC + 2][1];
        SP[2] = M[PC + 2][2];
        SP[3] = M[PC + 2][3];
        SP[4] = M[PC + 2][4];
        SP[5] = M[PC + 2][5];
        SP[6] = M[PC + 2][6];
        SP[7] = M[PC + 2][7];

        SP[8] = M[PC + 1][0];
        SP[9] = M[PC + 1][1];
        SP[10] = M[PC + 1][2];
        SP[11] = M[PC + 1][3];
        SP[12] = M[PC + 1][4];
        SP[13] = M[PC + 1][5];
        SP[14] = M[PC + 1][6];
        SP[15] = M[PC + 1][7];

        PC += 3;
        break;
    case 0x41:
        mv(B, C);
        PC++;
        break;
    case 0x51:
        mv(D, C);
        PC++;
        break;
    case 0x61:
        mv(H, C);
        PC++;
        break;
    case 0x71:
        ad = b2u16(H, L);
        mv(M[ad], C);
        PC++;
        break;
    case 0x81:
        _add(A, C, &c);
        PC++;
        break;
    case 0x91:
        _sub(A, C, &c);
        PC++;
        break;
    case 0xA1:
        for (int i = 0; i < WSIZE; i++) A[i] &= C[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB1:
        for (int i = 0; i < WSIZE; i++) A[i] |= C[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC1:
        // POP
        pop16(B, C);
        PC++;
        break;
    case 0xD1:
        // POP
        pop16(D, E);
        PC++;
        break;
    case 0xE1:
        // POP
        pop16(H, L);
        PC++;
        break;
    case 0xF1:
        // POP
        pop16(FL, A);
        PC++;
        break;
    case 2:
        // STAX
        ad = b2u16(B, C);
        mv(M[ad], A);
        PC++;
        break;
    case 0x12:
        // STAX
        ad = b2u16(D, E);
        mv(M[ad], A);
        PC++;
        break;
    case 0x22:
        // SHLD
        ad = b2u16(M[PC + 2], M[PC + 1]);
        mv(M[ad], L);
        mv(M[ad + 1], H);
        PC += 3;
        break;
    case 0x32:
        // STA
        ad = b2u16(M[PC + 2], M[PC + 1]);
        mv(A, M[ad]);
        PC += 3;
        break;
    case 0x42:
        mv(B, D);
        PC++;
        break;
    case 0x62:
        mv(H, D);
        PC++;
        break;
    case 0x72:
        ad = b2u16(H, L);
        mv(M[ad], D);
        PC++;
        break;
    case 0x82:
        _add(A, D, &c);
        PC++;
        break;
    case 0x92:
        _sub(A, D, &c);
        PC++;
        break;
    case 0xA2:
        for (int i = 0; i < WSIZE; i++) A[i] &= D[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB2:
        for (int i = 0; i < WSIZE; i++) A[i] |= D[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC2:
        if (FL[1] == 0) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xD2:
        if (FL[7] == 0) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xE2:
        if (FL[5] == 0) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xF2:
        if (FL[0] == 0) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 3:
        // INX
        mv(oFL, FL);
        _add(C, ONE, &c);
        _add(B, ZERO, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x13:
        // INX
        mv(oFL, FL);
        _add(E, ONE, &c);
        _add(D, ZERO, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x23:
        // INX
        mv(oFL, FL);
        _add(L, ONE, &c);
        _add(H, ZERO, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x33:
        // INX
        mv(oFL, FL);
        _add16(SP, ZERO, ONES, &c, 0);
        mv(FL, oFL);
        PC++;
        break;
    case 0x43:
        mv(B, E);
        PC++;
        break;
    case 0x53:
        mv(D, E);
        PC++;
        break;
    case 0x63:
        mv(H, E);
        PC++;
        break;
    case 0x73:
        ad = b2u16(H, L);
        mv(M[ad], E);
        PC++;
        break;
    case 0x83:
        _add(A, E, &c);
        PC++;
        break;
    case 0x93:
        _sub(A, E, &c);
        PC++;
        break;
    case 0xA3:
        for (int i = 0; i < WSIZE; i++) A[i] &= E[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB3:
        for (int i = 0; i < WSIZE; i++) A[i] |= E[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC3:
    case 0xCB:
        PC = b2u16(M[PC + 2], M[PC + 1]);
        break;
    case 0xD3:
        // OUT
        // TBD
        out(b2u(M[++PC]));
        PC++;
        break;
    case 0xE3:
        // XTHL
        ad = SP[0] * 32768 + SP[1] * 16384 + SP[2] * 8192;
        ad += SP[3] * 4096 + SP[4] * 2048 + SP[5] * 1024;
        ad += SP[6] * 512 + SP[7] * 256;
        ad += SP[8] * 128 + SP[9] * 64 + SP[10] * 32;
        ad += SP[11] * 16 + SP[12] * 8 + SP[13] * 4;
        ad += SP[14] * 2 + SP[15];
        mv(tl, M[ad]);
        mv(th, M[ad + 1]);
        mv(M[ad], L);
        mv(M[ad + 1], H);
        mv(L, tl);
        mv(H, th);
        PC++;
        break;
    case 0xF3:
        // DI
        // TBD
        PC++;
        break;
    case 4:
        // INR
        mv(oFL, FL);
        _add(B, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x14:
        // INR
        mv(oFL, FL);
        _add(D, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x24:
        // INR
        mv(oFL, FL);
        _add(H, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x34:
        // INR M (HL)
        mv(oFL, FL);
        _add(M[b2u16(H, L)], ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x44:
        mv(B, H);
        PC++;
        break;
    case 0x54:
        mv(D, H);
        PC++;
        break;
    case 0x74:
        ad = b2u16(H, L);
        mv(M[ad], H);
        PC++;
        break;
    case 0x84:
        _add(A, H, &c);
        PC++;
        break;
    case 0x94:
        _sub(A, H, &c);
        PC++;
        break;
    case 0xA4:
        for (int i = 0; i < WSIZE; i++) A[i] &= H[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB4:
        for (int i = 0; i < WSIZE; i++) A[i] |= H[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC4:
        if (FL[1] == 0) {
            u16b(PC + 1, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xD4:
        if (FL[7] == 0) {
            u16b(PC + 1, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xE4:
        if (FL[5] == 0) {
            u16b(PC + 1, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xF4:
        if (FL[0] == 0) {
            u16b(PC + 1, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 5:
        // DCR
        mv(oFL, FL);
        _add(B, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x15:
        // DCR
        mv(oFL, FL);
        _add(D, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x25:
        // DCR
        mv(oFL, FL);
        _add(H, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x35:
        // DCR
        mv(oFL, FL);
        _add(M[b2u16(H, L)], ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x45:
        mv(B, L);
        PC++;
        break;
    case 0x55:
        mv(D, L);
        PC++;
        break;
    case 0x65:
        mv(H, L);
        PC++;
        break;
    case 0x75:
        ad = b2u16(H, L);
        mv(M[ad], L);
        PC++;
        break;
    case 0x85:
        _add(A, L, &c);
        PC++;
        break;
    case 0x95:
        _sub(A, L, &c);
        PC++;
        break;
    case 0xA5:
        for (int i = 0; i < WSIZE; i++) A[i] &= L[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB5:
        for (int i = 0; i < WSIZE; i++) A[i] |= L[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC5:
        push16(B, C);
        PC++;
        break;
    case 0xD5:
        push16(D, E);
        PC++;
        break;
    case 0xE5:
        push16(H, L);
        PC++;
        break;
    case 0xF5:
        push16(FL, A);
        PC++;
        break;
    case 6:
        // MVI
        mv(B, M[++PC]);
        PC++;
        break;
    case 0x16:
        // MVI
        mv(D, M[++PC]);
        PC++;
        break;
    case 0x26:
        // MVI
        mv(H, M[++PC]);
        PC++;
        break;
    case 0x36:
        // MVI
        mv(M[b2u16(H, L)], M[++PC]);
        PC++;
        break;
    case 0x46:
        ad = b2u16(H, L);
        mv(B, M[ad]);
        PC++;
        break;
    case 0x56:
        ad = b2u16(H, L);
        mv(D, M[ad]);
        PC++;
        break;
    case 0x66:
        ad = b2u16(H, L);
        mv(H, M[ad]);
        PC++;
        break;
    case 0x76:
        HLTF = 1;
        PC++;
        break;
    case 0x86:
        ad = b2u16(H, L);
        _add(A, M[ad], &c);
        PC++;
        break;
    case 0x96:
        ad = b2u16(H, L);
        _sub(A, M[ad], &c);
        PC++;
        break;
    case 0xA6:
        ad = b2u16(H, L);
        for (int i = 0; i < WSIZE; i++) A[i] &= M[ad][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB6:
        ad = b2u16(H, L);
        for (int i = 0; i < WSIZE; i++) A[i] |= M[ad][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC6:
        _add(A, M[++PC], &c);
        PC++;
        break;
    case 0xD6:
        _sub(A, M[++PC], &c);
        PC++;
        break;
    case 0xE6:
        PC++;
        for (int i = 0; i < WSIZE; i++) A[i] &= M[PC][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xF6:
        PC++;
        for (int i = 0; i < WSIZE; i++) A[i] |= M[PC][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 7:
        // RLC
        mv(oFL, FL);
        FL[7] = A[0];
        for (int i = 0; i < WSIZE - 1; i++) A[i] = A[i + 1];
        A[7] = FL[7];
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        PC++;
        break;
    case 0x17:
        // RAL
        mv(oFL, FL);
        FL[7] = A[0];
        for (int i = 0; i < WSIZE - 1; i++) A[i] = A[i + 1];
        A[7] = oFL[7];
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        PC++;
        break;
    case 0x27:
        // DAA
        _daa();
        PC++;
        break;
    case 0x37:
        // STC
        FL[7] = 1;
        PC++;
        break;
    case 0x47:
        mv(B, A);
        PC++;
        break;
    case 0x57:
        mv(D, A);
        PC++;
        break;
    case 0x67:
        mv(H, A);
        PC++;
        break;
    case 0x77:
        ad = b2u16(H, L);
        mv(M[ad], A);
        PC++;
        break;
    case 0x87:
        _add(A, A, &c);
        PC++;
        break;
    case 0x97:
        _sub(A, A, &c);
        PC++;
        break;
    case 0xA7:
        for (int i = 0; i < WSIZE; i++) A[i] &= A[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB7:
        for (int i = 0; i < WSIZE; i++) A[i] |= A[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xC7:
        // RST
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 0;
        break;
    case 0xD7:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 16;
        break;
    case 0xE7:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 32;
        break;
    case 0xF7:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 48;
        break;
    case 0x48:
        mv(C, B);
        PC++;
        break;
    case 0x58:
        mv(E, B);
        PC++;
        break;
    case 0x68:
        mv(L, B);
        PC++;
        break;
    case 0x78:
        mv(A, B);
        PC++;
        break;
    case 0x88:
        c = FL[7];
        _add(A, B, &c);
        PC++;
        break;
    case 0x98:
        c = FL[7];
        _sub(A, B, &c);
        PC++;
        break;
    case 0xA8:
        for (int i = 0; i < WSIZE; i++) A[i] ^= B[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB8:
        mv(t, A);
        _sub(A, B, &c);
        mv(A, t);
        PC++;
        break;
    case 0xC8:
        // RZ
        if (FL[1] == 1) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xD8:
        // RC
        if (FL[7] == 1) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xE8:
        // RPE
        if (FL[5] == 1) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 0xF8:
        // RM
        if (FL[0] == 1) {
            pop16(th, tl);
            PC = b2u16(th, tl);
        }
        else PC++;
        break;
    case 9:
        // DAD
        mv(oFL, FL);
        _add(L, C, &c);
        _add(H, B, &c);
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        FL[7] = c;
        PC++;
        break;
    case 0x19:
        // DAD
        mv(oFL, FL);
        _add(L, E, &c);
        _add(H, D, &c);
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        FL[7] = c;
        PC++;
        break;
    case 0x29:
        // DAD
        mv(oFL, FL);
        _add(L, L, &c);
        _add(H, H, &c);
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        FL[7] = c;
        PC++;
        break;
    case 0x39:
        // DAD
        mv(oFL, FL);
        _add16(SP, H, L, &c, 1);
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        FL[7] = c;
        PC++;
        break;
    case 0x59:
        mv(E, C);
        PC++;
        break;
    case 0x69:
        mv(L, C);
        PC++;
        break;
    case 0x79:
        mv(A, C);
        PC++;
        break;
    case 0x89:
        c = FL[7];
        _add(A, C, &c);
        PC++;
        break;
    case 0x99:
        c = FL[7];
        _sub(A, C, &c);
        PC++;
        break;
    case 0xA9:
        for (int i = 0; i < WSIZE; i++) A[i] ^= C[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xB9:
        mv(t, A);
        _sub(A, C, &c);
        mv(A, t);
        PC++;
        break;
    case 0xC9:
    case 0xD9:
        // RET
        pop16(th, tl);
        PC = b2u16(th, tl);
        break;
    case 0xE9:
        // PCHL
        PC = b2u16(H, L);
        break;
    case 0xF9:
        // SPHL
        SP[0] = H[0]; SP[1] = H[1];
        SP[2] = H[2]; SP[3] = H[3];
        SP[4] = H[4]; SP[5] = H[5];
        SP[6] = H[6]; SP[7] = H[7];
        SP[8] = L[0]; SP[9] = L[1];
        SP[10] = L[2]; SP[11] = L[3];
        SP[12] = L[4]; SP[13] = L[5];
        SP[14] = L[6]; SP[15] = L[7];
        break;
    case 0xA:
        // LDAX
        ad = b2u16(B, C);
        mv(A, M[ad]);
        PC++;
        break;
    case 0x1A:
        // LDAX
        ad = b2u16(D, E);
        mv(A, M[ad]);
        PC++;
        break;
    case 0x2A:
        // LHLD
        ad = b2u16(M[PC + 2], M[PC + 1]);
        mv(L, M[ad]);
        mv(H, M[ad + 1]);
        PC += 3;
        break;
    case 0x3A:
        // LDA
        ad = b2u16(M[PC + 2], M[PC + 1]);
        mv(A, M[ad]);
        PC += 3;
        break;
    case 0x4A:
        mv(C, D);
        PC++;
        break;
    case 0x5A:
        mv(E, D);
        PC++;
        break;
    case 0x6A:
        mv(L, D);
        PC++;
        break;
    case 0x7A:
        mv(A, D);
        PC++;
        break;
    case 0x8A:
        c = FL[7];
        _add(A, D, &c);
        PC++;
        break;
    case 0x9A:
        c = FL[7];
        _sub(A, D, &c);
        PC++;
        break;
    case 0xAA:
        for (int i = 0; i < WSIZE; i++) A[i] ^= D[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBA:
        mv(t, A);
        _sub(A, D, &c);
        mv(A, t);
        PC++;
        break;
    case 0xCA:
        if (FL[1] == 1) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xDA:
        if (FL[7] == 1) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xEA:
        if (FL[5] == 1) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xFA:
        if (FL[0] == 1) {
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xB:
        // DCX
        mv(oFL, FL);
        _add(C, ONES, &c);
        _add(B, ONES, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x1B:
        // DCX
        mv(oFL, FL);
        _add(E, ONES, &c);
        _add(D, ONES, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x2B:
        // DCX
        mv(oFL, FL);
        _add(L, ONES, &c);
        _add(H, ONES, &c);
        mv(FL, oFL);
        PC++;
        break;
    case 0x3B:
        // DCX
        mv(oFL, FL);
        _add16(SP, ONES, ONES, &c, 0);
        mv(FL, oFL);
        PC++;
        break;
    case 0x4B:
        mv(C, E);
        PC++;
        break;
    case 0x6B:
        mv(L, E);
        PC++;
        break;
    case 0x7B:
        mv(A, E);
        PC++;
        break;
    case 0x8B:
        c = FL[7];
        _add(A, E, &c);
        PC++;
        break;
    case 0x9B:
        c = FL[7];
        _sub(A, E, &c);
        PC++;
        break;
    case 0xAB:
        for (int i = 0; i < WSIZE; i++) A[i] ^= E[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBB:
        mv(t, A);
        _sub(A, E, &c);
        mv(A, t);
        PC++;
        break;
    case 0xDB:
        // IN
        // TBD
        in(b2u(M[++PC]));
        PC++;
        break;
    case 0xEB:
        // XCHG
        mv(th, H);
        mv(tl, L);
        mv(H, D);
        mv(L, E);
        mv(D, th);
        mv(E, tl);
        PC++;
        break;
    case 0xFB:
        // EI
        // TBD
        PC++;
        break;
    case 0xC:
        // INR
        mv(oFL, FL);
        _add(C, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x1C:
        // INR
        mv(oFL, FL);
        _add(E, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x2C:
        // INR
        mv(oFL, FL);
        _add(L, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x3C:
        // INR
        mv(oFL, FL);
        _add(A, ONE, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x4C:
        mv(C, H);
        PC++;
        break;
    case 0x5C:
        mv(E, H);
        PC++;
        break;
    case 0x6C:
        mv(L, H);
        PC++;
        break;
    case 0x7C:
        mv(A, H);
        PC++;
        break;
    case 0x8C:
        c = FL[7];
        _add(A, H, &c);
        PC++;
        break;
    case 0x9C:
        c = FL[7];
        _sub(A, H, &c);
        PC++;
        break;
    case 0xAC:
        for (int i = 0; i < WSIZE; i++) A[i] ^= H[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBC:
        mv(t, A);
        _sub(A, H, &c);
        mv(A, t);
        PC++;
        break;
    case 0xCC:
        if (FL[1] == 1) {
            u16b(PC + 3, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xDC:
        if (FL[7] == 1) {
            u16b(PC + 3, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xEC:
        if (FL[5] == 1) {
            u16b(PC + 3, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xFC:
        if (FL[0] == 1) {
            u16b(PC + 3, th, tl);
            push16(th, tl);
            PC = b2u16(M[PC + 2], M[PC + 1]);
        }
        else PC += 3;
        break;
    case 0xD:
        // DCR
        mv(oFL, FL);
        _add(C, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x1D:
        // DCR
        mv(oFL, FL);
        _add(E, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x2D:
        // DCR
        mv(oFL, FL);
        _add(L, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x3D:
        // DCR
        mv(oFL, FL);
        _add(A, ONES, &c);
        FL[7] = oFL[7];
        PC++;
        break;
    case 0x4D:
        mv(C, L);
        PC++;
        break;
    case 0x5D:
        mv(E, L);
        PC++;
        break;
    case 0x7D:
        mv(A, L);
        PC++;
        break;
    case 0x8D:
        c = FL[7];
        _add(A, L, &c);
        PC++;
        break;
    case 0x9D:
        c = FL[7];
        _sub(A, L, &c);
        PC++;
        break;
    case 0xAD:
        for (int i = 0; i < WSIZE; i++) A[i] ^= L[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBD:
        mv(t, A);
        _sub(A, L, &c);
        mv(A, t);
        PC++;
        break;
    case 0xCD:
    case 0xDD:
    case 0xED:
    case 0xFD:
        u16b(PC + 3, th, tl);
        push16(th, tl);
        PC = b2u16(M[PC + 2], M[PC + 1]);
        break;
    case 0xE:
        // MVI
        mv(C, M[++PC]);
        PC++;
        break;
    case 0x1E:
        // MVI
        mv(E, M[++PC]);
        PC++;
        break;
    case 0x2E:
        // MVI
        mv(L, M[++PC]);
        PC++;
        break;
    case 0x3E:
        // MVI
        mv(A, M[++PC]);
        PC++;
        break;
    case 0x4E:
        ad = b2u16(H, L);
        mv(C, M[ad]);
        PC++;
        break;
    case 0x5E:
        ad = b2u16(H, L);
        mv(E, M[ad]);
        PC++;
        break;
    case 0x6E:
        ad = b2u16(H, L);
        mv(L, M[ad]);
        PC++;
        break;
    case 0x7E:
        ad = b2u16(H, L);
        mv(A, M[ad]);
        PC++;
        break;
    case 0x8E:
        ad = b2u16(H, L);
        c = FL[7];
        _add(A, M[ad], &c);
        PC++;
        break;
    case 0x9E:
        ad = b2u16(H, L);
        c = FL[7];
        _sub(A, M[ad], &c);
        PC++;
        break;
    case 0xAE:
        ad = b2u16(H, L);
        for (int i = 0; i < WSIZE; i++) A[i] ^= M[ad][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBE:
        ad = b2u16(H, L);
        mv(t, A);
        _sub(A, M[ad], &c);
        mv(A, t);
        PC++;
        break;
    case 0xCE:
        // ACI
        c = FL[7];
        _add(A, M[++PC], &c);
        PC++;
        break;
    case 0xDE:
        // SBI
        c = FL[7];
        _sub(A, M[++PC], &c);
        PC++;
        break;
    case 0xEE:
        PC++;
        for (int i = 0; i < WSIZE; i++) A[i] ^= M[PC][i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xFE:
        mv(t, A);
        _sub(A, M[++PC], &c);
        mv(A, t);
        PC++;
        break;
    case 0xF:
        // RRC
        mv(oFL, FL);
        FL[7] = A[7];
        for (int i = WSIZE - 1; i > 0; i--) A[i] = A[i - 1];
        A[0] = FL[7];
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        PC++;
        break;
    case 0x1F:
        // RAR
        mv(oFL, FL);
        FL[7] = A[7];
        for (int i = WSIZE - 1; i > 0; i--) A[i] = A[i - 1];
        A[0] = oFL[7];
        FL[0] = oFL[0];
        FL[1] = oFL[1];
        FL[3] = oFL[3];
        FL[5] = oFL[5];
        PC++;
        break;
    case 0x2F:
        // CMA
        for (int i = 0; i < WSIZE; i++) A[i] ^= 1;
        PC++;
        break;
    case 0x3F:
        // CMC
        FL[7] ^= 1;
        PC++;
        break;
    case 0x4F:
        mv(C, A);
        PC++;
        break;
    case 0x5F:
        mv(E, A);
        PC++;
        break;
    case 0x6F:
        mv(L, A);
        PC++;
        break;
    case 0x8F:
        c = FL[7];
        _add(A, A, &c);
        PC++;
        break;
    case 0x9F:
        c = FL[7];
        _sub(A, A, &c);
        PC++;
        break;
    case 0xAF:
        for (int i = 0; i < WSIZE; i++) A[i] ^= A[i];
        FL[7] = 0; FL[3] = 0;
        FL[0] = A[0];
        if (isz(A)) FL[1] = 1; else FL[1] = 0;
        FL[5] = (cnt1(A) % 2) ^ 1;
        PC++;
        break;
    case 0xBF:
        mv(t, A);
        _sub(A, A, &c);
        mv(A, t);
        PC++;
        break;
    case 0xCF:
        // RST 1
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 8;
        break;
    case 0xDF:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 24;
        break;
    case 0xEF:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 40;
        break;
    case 0xFF:
        u16b(PC + 1, th, tl);
        push16(th, tl);
        PC = 56;
        break;
    default:
        printf("???\n");
        PC++;
        break;
    }
}

word* x2b(char* s)
{
    word* r;
    r = calloc(8, sizeof(char));
    r += 7;
    int v = strtol(s, NULL, 16);
    for (int i = 0; i < 8; i++) {
        *r = v % 2;
        v /= 2;
        r--;
    }
    return r + 1;
}

void lhex(char* n)
{
    char* bf, * p, * du2, * du4, * da, * oda;
    int bc, ad, rc, ck;

    FILE* fi;

    fi = fopen(n, "r+");
    if (!fi) { printf("HEX not found\n"); exit(2); }
    PC = 0;

    du2 = calloc(3, sizeof(du2));
    du4 = calloc(5, sizeof(du4));
    bf = calloc(80, sizeof(bf));
    da = calloc(512, sizeof(da));
    oda = da;
    while (fgets(bf, 79, fi)) {
        da = oda;
        p = strchr(bf, '\n');
        *p = 0;
        if (*bf == ' ') bf++; // remove initial space from weird PL/M output
        if (*bf != ':') continue;
        if (strcmp(bf, ":00000001FF") == 0) break;
        if (strcmp(bf, ":0000000000") == 0) break;
        bf++;
        bc = (int)strtol(strncpy(du2, bf, 2), NULL, 16);
        bf += 2;
        ad = (int)strtol(strncpy(du4, bf, 4), NULL, 16);
        if (!PC) PC = ad;
        bf += 4;
        rc = (int)strtol(strncpy(du2, bf, 2), NULL, 16);
        bf += 2;
        memcpy(da, bf, bc * 2);
        bf += (bc * 2);
        ck = (int)strtol(strncpy(du2, bf, 2), NULL, 16);
        for (int i = 0; i < 2 * bc; i += 2) {
            du2[0] = da[0]; du2[1] = da[1];
            du2[2] = 0; da += 2;
            mv(M[ad++], x2b(du2));
        }
    }

    if (fi) fclose(fi);
}

int main(int n, char** a)
{
    i0();
    lhex("3809.hex");

    if (n > 1) lhex(a[1]);

    printf("Starting at %04XH\n", PC);

    while (HLTF == 0) {
        exc(M[PC]);
    }

    printf("\n\nPC: %04XH\n", PC);
    pw(A);
    pw(B);
    pw(C);
    pw(D);
    pw(E);
    pw(H);
    pw(L);
    pwf();

    return 0;
}