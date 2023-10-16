#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>

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
word SP[2*WSIZE];

word ONE[WSIZE];
word ZERO[WSIZE];
word ONES[WSIZE];
word ONES16[2*WSIZE];

short PC;
char HLTF=0;

word *M[MSIZE];

char isz(word *r)
{
    for(int i=0;i<WSIZE;i++)
        if(r[i]!=0) return 0;
    return 1;
}

word cnt1(word *r)
{
    int s=0;
    for(int i=0;i<WSIZE;i++)
        if(r[i]==1) s++;
    
    return s;
}

void mv(word *d, word *s)
{
    for(int i=0;i<WSIZE;i++) d[i]=s[i];
}

void mvn(word *d, word *s, int ss, int se)
{
    for(int i=ss,j=0;i<=se;i++,j++) d[j]=s[i];
}

void mvnd(word *d, word *s, int ss, int se, int ds)
{
    for(int i=ss,j=ds;i<=se;i++,j++) d[j]=s[i];
}

int b2u(word *);
void pw(word *v)
{
    printf("%02XH (",b2u(v));
    for(int i=0;i<WSIZE;i++) printf("%d",v[i]);
    printf(")\n");
}

word ab(word a, word b, word *c)
{
    word s = a+b+*c;
    if(s==3) {s=1; *c=1;}
    else if(s==2) {s=0; *c=1;}
    else *c=0;
    return s;
}

void cmp(word *a)
{
    word c = 0;
    for(int i=0;i<WSIZE;i++) a[i]=a[i]^1;
    for(int i=WSIZE-1;i>=0;i--) a[i]=ab(a[i],ONE[i],&c);
}

int b2u(word *v)
{
    int r=0;
    r=v[0]*128+v[1]*64+v[2]*32;
    r+=v[3]*16+v[4]*8+v[5]*4;
    r+=v[6]*2+v[7];
    return r;
}

int b2u16(word *h, word *l)
{
    int r=0;
    r=h[0]*32768+h[1]*16384+h[2]*8192;
    r+=h[3]*4096+h[4]*2048+h[5]*1024;
    r+=h[6]*512+h[7]*256;
    r+=l[0]*128+l[1]*64+l[2]*32;
    r+=l[3]*16+l[4]*8+l[5]*4;
    r+=l[6]*2+l[7];
    return r;
}

void i0(void)
{
    for(int i=0;i<WSIZE;i++) {
        A[i]=0;
        B[i]=0;C[i]=0;
        D[i]=0;E[i]=0;
        H[i]=0;L[i]=0;
        ONE[i]=0;
        FL[i]=0;
        ZERO[i]=0;
        ONES[i] = 1;
    }
    for(int i=0;i<2*WSIZE;i++) {
        SP[i]=0;
        ONE16[i]=0;
        ONES16[i]=1;
    }
    for(int i=0;i<MSIZE;i++) {
        M[i]=calloc(WSIZE,sizeof(char));
    }
    ONE[WSIZE-1]=1;
    ONE16[2*WSIZE-1]=1;
    PC=0;
    FL[6]=1;
}

void _add(word *r, word* s, word *c)
{
    for(int i=WSIZE-1;i>=0;i--) {
        r[i]=ab(r[i],s[i],c);
        if(i==3) FL[3]=*c;
    }
    FL[7]=*c;
    FL[0]=r[0];
    if(isz(r)) FL[1]=1; else FL[1]=0;
    FL[5]=cnt1(r);
}

void _sub(word *r, word* ss, word *c)
{
    word *s;
    s=calloc(WSIZE,sizeof(char));
    mv(s,ss);
    cmp(s);
    for(int i=WSIZE-1;i>=0;i--) {
        r[i]=ab(r[i],s[i],c);
        if(i==3) FL[3]=*c;
    }
    FL[7]=(*c)^1;
    FL[0]=r[0];
    if(isz(r)) FL[1]=1; else FL[1]=0;
    FL[5]=cnt1(r);
}

void _add16(word *r, word *sh, word *sl, word *c)
{
    word s[2*WSIZE];
    s[0]=sh[0];
    s[1]=sh[1];
    s[2]=sh[2];
    s[3]=sh[3];
    s[4]=sh[4];
    s[5]=sh[5];
    s[6]=sh[6];
    s[7]=sh[7];
    s[8]=sl[0];
    s[9]=sl[1];
    s[10]=sl[2];
    s[11]=sl[3];
    s[12]=sl[4];
    s[13]=sl[5];
    s[14]=sl[6];
    s[15]=sl[7];

    for(int i=2*WSIZE-1;i>=0;i--) {
        s[i]=ab(r[i],s[i],c);
        if(i==3) FL[3]=*c;
    }

    FL[7]=*c;
    FL[0]=r[0];
    if(isz(r)) FL[1]=1; else FL[1]=0;
    FL[5]=cnt1(r);

    sh[0]=s[0];
    sh[1]=s[1];
    sh[2]=s[2];
    sh[3]=s[3];
    sh[4]=s[4];
    sh[5]=s[5];
    sh[6]=s[6];
    sh[7]=s[7];
    sl[0]=s[8];
    sl[1]=s[9];
    sl[2]=s[10];
    sl[3]=s[11];
    sl[4]=s[12];
    sl[5]=s[13];
    sl[6]=s[14];
    sl[7]=s[15];
}

void _daa(void)
{
    word c=0;
    word sl[]={0,0,0,0,0,1,1,0};
    word sh[]={0,1,1,0,0,0,0,0};
    int r=A[7]+A[6]*2+A[5]*4+A[4]*8;
    if (r>9) {
        _add(A,sl,&c);
    }
    r=A[3]+A[2]*2+A[1]*4+A[0]*8;
    if(r>9) {
        _add(A,sh,&c);
    }
}

void exc(word *a)
{
    int ad = 0;
    word c = 0;
    word oFL[WSIZE];
    switch(b2u(a))
    {
        case 0:
        case 8:
        case 0x10:
        case 0x20:
        case 0x30:
            PC++;
            break;
        case 1:
            // LXI
            mv(C,M[++PC]);
            mv(B,M[++PC]);
            PC+=3;
            break;
        case 0x11:
            // LXI
            mv(E,M[++PC]);
            mv(D,M[++PC]);
            PC++;
            break;
        case 0x21:
            // LXI
            mv(L,M[++PC]);
            mv(H,M[++PC]);
            PC++;
            break;
        case 0x31:
            // LXI
            SP[0]=M[PC+2][0];
            SP[1]=M[PC+2][1];
            SP[2]=M[PC+2][2];
            SP[3]=M[PC+2][3];
            SP[4]=M[PC+2][4];
            SP[5]=M[PC+2][5];
            SP[6]=M[PC+2][6];
            SP[7]=M[PC+2][7];
            
            SP[8]=M[PC+1][0];
            SP[9]=M[PC+1][1];
            SP[10]=M[PC+1][2];
            SP[11]=M[PC+1][3];
            SP[12]=M[PC+1][4];
            SP[13]=M[PC+1][5];
            SP[14]=M[PC+1][6];
            SP[15]=M[PC+1][7];

            PC+=3;
            break;
        case 2:
            // STAX
            ad=b2u16(B,C);
            mv(M[ad],A);
            PC++;
            break;
        case 0x12:
            // STAX
            ad=b2u16(D,E);
            mv(M[ad],A);
            PC++;
            break;
        case 3:
            // INX
            mv(oFL,FL);
            _add(C,ONE,&c);
            _add(B,ZERO,&c);
            mv(FL,oFL);
            PC++;
            break;
        case 0x13:
            // INX
            mv(oFL,FL);
            _add(D,ONE,&c);
            _add(E,ZERO,&c);
            mv(FL,oFL);
            PC++;
            break;
        case 0x23:
            // INX
            mv(oFL,FL);
            _add(H,ONE,&c);
            _add(L,ZERO,&c);
            mv(FL,oFL);
            PC++;
            break;
        case 0x33:
            // INX
            mv(oFL,FL);
            _add16(SP,ONE16,&c);
            mv(FL,oFL);
            PC++;
            break;
        case 4:
            // INR
            mv(oFL,FL);
            _add(B,ONE,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x14:
            // INR
            mv(oFL,FL);
            _add(D,ONE,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x24:
            // INR
            mv(oFL,FL);
            _add(H,ONE,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x34:
            // INR M (HL)
            mv(oFL,FL);
            _add(M[b2u16(H,L)],ONE,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 5:
            // DCR
            mv(oFL,FL);
            _add(B,ONES,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x15:
            // DCR
            mv(oFL,FL);
            _add(D,ONES,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x25:
            // DCR
            mv(oFL,FL);
            _add(H,ONES,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0x35:
            // DCR
            mv(oFL,FL);
            _add(M[b2u16(H,L)],ONES,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 6:
            // MVI
            mv(B,M[++PC]);
            PC++;
            break;
        case 0x16:
            // MVI
            mv(D,M[++PC]);
            PC++;
            break;
        case 0x26:
            // MVI
            mv(H,M[++PC]);
            PC++;
            break;
        case 0x36:
            // MVI
            mv(M[b2u16(H,L)],M[++PC]);
            PC++;
            break;
        case 7:
            // RLC
            mv(oFL,FL);
            FL[7]=A[0];
            for(int i=0;i<WSIZE-1;i++) A[i]=A[i+1];
            A[7]=FL[7];
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x17:
            // RAL
            mv(oFL,FL);
            FL[7]=A[0];
            for(int i=0;i<WSIZE-1;i++) A[i]=A[i+1];
            A[7]=oFL[7];
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x27:
            // DAA
            _daa();
            PC++;
            break;
        case 0x37:
            // STC
            FL[7]=1;
            PC++;
            break;
        case 9:
            // DAD
            mv(oFL,FL);
            _add(L,C,&c);
            _add(H,B,&c);
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x19:
            // DAD
            mv(oFL,FL);
            _add(L,E,&c);
            _add(H,D,&c);
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x29:
            // DAD
            mv(oFL,FL);
            _add(L,L,&c);
            _add(H,H,&c);
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x39:
            // DAD
            mv(oFL,FL);
            _add16(SP,H,L,&c);
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0xA:
            // LDAX
            mv(oFL,FL);
            ad=b2u16(B,C);
            mv(A,M[ad]);
            mv(FL,oFL);
            PC++;
            break;
        case 0xB:
            // DCX
            mv(oFL,FL);
            _add(C,ONES,&c);
            _add(B,ONES,&c);
            mv(FL,oFL);
            PC++;
            break;
        case 0xC:
            // INR
            mv(oFL,FL);
            _add(C,ONE,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0xD:
            // DCR
            // DCR
            mv(oFL,FL);
            _add(C,ONES,&c);
            FL[7]=oFL[7];
            PC++;
            break;
        case 0xE:
            // MVI
            mv(C,M[++PC]);
            PC++;
            break;
        case 0xF:
            // RRC
            mv(oFL,FL);
            FL[7]=A[7];
            for(int i=WSIZE-1;i>0;i--) A[i]=A[i-1];
            A[0]=FL[7];
            FL[0]=oFL[0];
            FL[1]=oFL[1];
            FL[3]=oFL[3];
            FL[5]=oFL[5];
            PC++;
            break;
        case 0x76:
            HLTF=1;
            break;
        default:
            break;
    }
}

void set(word *m, word v)
{
    for(int i=WSIZE-1;i>=0;i--) {
        m[i]=v%2;
        v/=2;
    }
}

int main(int n, char **a)
{
    i0();

    PC=100;

    set(M[100],0x6);
    set(M[101],0x3);
    set(M[102],0xe);
    set(M[103],0x2a);
    set(M[104],0xa);

    set(M[105],0x27);


    set(M[110],0x76);



    set(M[0x32a],0x9b);

    while(HLTF==0) {
        exc(M[PC]);
    }

    pw(A);
    pw(FL);
}