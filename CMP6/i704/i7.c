#include <stdio.h>

unsigned long AC; // 38bit (37+Sign)
unsigned long MQ;
unsigned long SR;
unsigned long M[8192];
short A,B,C;
char OVF=0;

#define S(x) ((x&0x2000000000)>>37)
#define SV(x) (x&0x2000000000)
#define Q(x) ((x&0x1000000000)>>36)
#define P(x) ((x&0x800000000)>>35)
#define V(x) (x&0x7FFFFFFFF)

#define SGNV (1LL<<37)
#define QV (1LL<<36)
#define PV (1LL<<35)

#define SAS (AC|=(1LL<<37))
#define SAQ (AC|=(1LL<<36))
#define SAP (AC|=(1LL<<35))
#define CAS (AC&=~(1LL<<37))
#define CAQ (AC&=~(1LL<<36))
#define CAP (AC&=~(1LL<<35))

// TYPE-A

#define PRE(x) ((x&0xE00000000)>>33)
#define DEC(x) ((x&0x1FFFC0000)>>18)
#define TAG(x) ((x&0x38000)>>15)
#define ADR(x) (x&0x7FFF)

// TYPE-B

#define OPR(x) ((x&0xFFF000000)>>24)

// FLOATING POINT

#define CHR(x) ((x&0x7F8000000)>>27)
#define MAN(x) (x&0x7FFFFFF)

// OPERATIONS

int tag()
{
    switch(TAG(SR)) {
        case 1:
        return A;
        break;
        case 2:
        return B;
        break;
        case 3:
        return A|B;
        break;
        case 4:
        return C;
        break;
        case 5:
        return A|C;
        break;
        case 6:
        return B|C;
        break;
        case 7:
        return A|B|C;
        break;
        default:
        return 0;
    }
}

void stm(int a, int v)
{
    char s = (v>=0) ? 0 : 1;
    M[a] = 0;
    if(s) { v*=-1; M[a]|=SGNV; }
    M[a]|=v;
}

void cla()
{
    int ad = ADR(SR)-tag();
    AC=0;
    AC|= SV(M[ad]);
    CAQ; CAP;
    AC|= (S(M[ad])==0) ? V(M[ad]) : V(-V(M[ad]));
}

void add()
{
    int ad = ADR(SR)-tag();
    int p=P(AC);
    int ac = (S(AC)==0) ? V(AC) : -V(AC);
    int y = (S(M[ad])==0) ? V(M[ad]) : -V(M[ad]);
    
    ac+=y;
    if(ac<0) {
        SAS;
        AC|=V(-ac);
    } else {
        CAS;
        AC|=V(ac);
    }
    if(p!=P(AC)) OVF=1; else OVF=0;
}

void adm()
{
    int ad = ADR(SR)-tag();
    int p=P(AC);
    int ac = (S(AC)==0) ? V(AC) : -V(AC);
    int y = V(M[ad]);

    ac+=y;
    if(ac<0) {
        SAS;
        AC|=V(-ac);
    } else {
        CAS;
        AC|=V(ac);
    }
    if(p!=P(AC)) OVF=1; else OVF=0;
}

int main(int n, char **a)
{
    stm(1000,-2342);
    stm(1010,3000);
    SR=1010;
    cla();
    SR=1000;
    add();
    return 0;
}