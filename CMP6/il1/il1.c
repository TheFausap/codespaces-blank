#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// 1bit (sign) + 39bits

char A[40];
char R3[40];
char Q[40];
char ADDR[40];
char CMPG[40];
char BF[9];
char DBF[11];
char M[1024][40];
char ONE[40];
char HALF[40];
int PC = 0;

char HLTF = 0;
int bPC = -1;
char lfto = 1;

FILE *fi, *fo, *dr;

//               0  1  2   3   4   5   6  7   8  9        
char sex[] = {   0, 0, 0,  0,  0,  0,  0, 0,  0, 0, // 0 
                 0, 0, 0,  0,  0,  0,  0, 0,  0, 0, // 1
                 0, 0, 0,  0,  0,  0,  0, 0,  0, 0, // 2
                 0, 0, 0,  0,  0,  0,  0, 0,  0, 0, // 3
                 0, 0, 0,  0,  0,  0,  0, 0,  0, 1, // 4
                 2, 3, 4,  5,  6,  7,  8, 9,  0, 0, // 5
                 0, 0, 0,  0,  0,  0,  0, 0,  0, 0, // 6
                14, 0, 0,  0, 13, 10, 15, 0, 12, 0, // 7
                 0, 0, 0, 11 };                     // 8

char chas[] = { '0', '1', '2', '3', '4', '5', '6', 
                '7', '8', '9', 'K', 'S', 'N', 'J', 
                'F', 'L' };

char *cel[] = {  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0, "0000", "0001",
                 "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 "1110", 0, 0, 0, "1101", "1010", "1111", 0, "1100", 0, 
                 0, 0, 0, "1011" };

char *restr[] = {  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0, "00", "01",
                 "10", "11" };

void mvn(char *, char *, int, int);
char luxT(char *st) {
    char *s;
    s=calloc(5,sizeof(char));
    mvn(s,st,0,3);
    s[0]+=48;s[1]+=48;s[2]+=48;s[3]+=48;
    if (strcmp(s,"0000")==0) return '0';
    else if (strcmp(s,"0001")==0) return '1';
    else if (strcmp(s,"0010")==0) return '2';
    else if (strcmp(s,"0011")==0) return '3';
    else if (strcmp(s,"0100")==0) return '4';
    else if (strcmp(s,"0101")==0) return '5';
    else if (strcmp(s,"0110")==0) return '6';
    else if (strcmp(s,"0111")==0) return '7';
    else if (strcmp(s,"1000")==0) return '8';
    else if (strcmp(s,"1001")==0) return '9';
    else if (strcmp(s,"1010")==0) return 'K';
    else if (strcmp(s,"1011")==0) return 'S';
    else if (strcmp(s,"1100")==0) return 'N';
    else if (strcmp(s,"1101")==0) return 'J';
    else if (strcmp(s,"1110")==0) return 'F';
    else if (strcmp(s,"1111")==0) return 'L';
}

char luxV(char *st) {
    char *s;
    s=calloc(5,sizeof(char));
    mvn(s,st,4,7);
    s[0]+=48;s[1]+=48;s[2]+=48;s[3]+=48;
    if (strcmp(s,"0000")==0) return '0';
    else if (strcmp(s,"0001")==0) return '1';
    else if (strcmp(s,"0010")==0) return '2';
    else if (strcmp(s,"0011")==0) return '3';
    else if (strcmp(s,"0100")==0) return '4';
    else if (strcmp(s,"0101")==0) return '5';
    else if (strcmp(s,"0110")==0) return '6';
    else if (strcmp(s,"0111")==0) return '7';
    else if (strcmp(s,"1000")==0) return '8';
    else if (strcmp(s,"1001")==0) return '9';
    else if (strcmp(s,"1010")==0) return 'K';
    else if (strcmp(s,"1011")==0) return 'S';
    else if (strcmp(s,"1100")==0) return 'N';
    else if (strcmp(s,"1101")==0) return 'J';
    else if (strcmp(s,"1110")==0) return 'F';
    else if (strcmp(s,"1111")==0) return 'L';
}

void pw(char *v)
{
    for(int i=0;i<40;i++) printf("%d",v[i]);
    printf("\n");
}

int fer(char *s) {
    int j=0, r=0;
    for(j=0;j<10;j++,s++) {r += (*s*(1<<(9-j)));}
    return r;
}

int drufer(char *s) {
    int j=0, r=0;
    for(j=0;j<10;j++,s++) {r += (*s*(1<<(13-j)));}
    return r;
}

int b2d(char *v, int l)
{
    int r=0, j=0;
    l--;
    for(;*v++;j++) {
        r += (*v*(1<<(l-j)));
    }
    return r;
}

void setsex(char*, char *, char *);
int sex2d(char *);
#define SX(x,y,z) setsex(M[sex2d(#x)],#y,#z)
void i0()
{
    for(int i=0;i<1000;i++) {
        for(int j=0;j<40;j++) M[i][j]=0;
    }
    for(int i=0;i<40;i++) { A[i] = 0; R3[i] = 0; Q[i] = 0; ONE[i] = 0; ADDR[i] = 0; HALF[i] = 0; }
    ONE[39] = 1; HALF[1]=1;
    #if 0
    // D.O.I. 999-1023
    SX(3F7,S5001,263L2);
    SX(3F8,L03L1,10003);
    SX(3F9,L4000,00002);
    SX(3FK,L4000,00001);
    SX(3FS,40000,81004);
    SX(3FN,L43L1,363F8);
    SX(3FJ,L43F7,423FF);
    SX(3FF,L5000,26000);
    SX(3FL,00000,00002);
    SX(3L0,465F6,604S8);
    SX(3L1,7LLLL,LLLL6);
    SX(3L2,403FL,223LN);
    SX(3L3,81004,223L7);
    SX(3L4,42001,22001);
    SX(3L5,503L0,7J000);
    SX(3L6,263L9,223LN);
    SX(3L7,F53FJ,423L8);
    SX(3L8,L5000,L4000);
    SX(3L9,L4001,40002);
    SX(3LK,40001,L13L4);
    SX(3LS,L43FK,423L4);
    SX(3LN,L43L9,423L9);
    SX(3LJ,503FL,L5001);
    SX(3LF,80008,0000N);
    SX(3LL,40001,273FS);
    #endif
}

int sex2d(char *s) {
    int r=sex[s[0]]*256;
    r+=sex[s[1]]*16;
    r+=sex[s[2]];
    return r;
}

char *d2sex(int v)
{
    char *r, *or;
    int vv = 0;
    r = calloc(4,sizeof(char));
    r[0]=48;r[1]=48;r[2]=48;
    or = r; r+=2;
    while(v!=0) {
        vv = v%16;
        *r--=chas[vv];
        v/=16;
    }
    return strdup(or);
}

char *sex2ad(char *s)
{
    char *r;
    r=calloc(11,sizeof(char));
    strcpy(r,restr[s[0]]);
    strcat(r,cel[s[1]]);
    strcat(r,cel[s[2]]);
    return strdup(r);
}

char *sex2op(char *s)
{
    char *r;
    r=calloc(9,sizeof(char));
    strcpy(r,cel[s[0]]);
    strcat(r,cel[s[1]]);
    return strdup(r);
}

char ab(char a, char b, char *c)
{
    char s = a+b+*c;
    if(s==3) {s=1; *c=1;}
    else if(s==2) {s=0; *c=1;}
    else *c=0;
    return s;
}

void cmp(char *a)
{
    char c = 0;
    for(int i=0;i<40;i++) CMPG[i]=a[i]^1;
    for(int i=39;i>=0;i--) CMPG[i]=ab(CMPG[i],ONE[i],&c);
}

void cmp1(char *a)
{
    char c = 0;
    for(int i=0;i<40;i++) CMPG[i]=a[i]^1;
}

void mv(char *, char *);

char *iabs(char *a)
{
    char *t;
    t=calloc(40,sizeof(char));
    mv(t,a);
    if(a[0]) { cmp(a); mv(t,CMPG); }
    return t;
}

// v1>v2 return 1 otherwise 0
int gt(char *v1, char *v2)
{
    for (int i=0;i<40;i++) {
        if(v1[i]!=v2[i]) return (v1[i]==1) ? 1 : 0;
    }
    return 0;
}

int eq(char *v1, char *v2)
{
    for (int i=0;i<40;i++) {
        if(v1[i]!=v2[i]) return 0;
    }
    return 1;
}

void mv(char *d, char *s)
{
    for(int i=0;i<40;i++) d[i]=s[i];
}

void mvn(char *d, char *s, int ss, int se)
{
    for(int i=ss,j=0;i<=se;i++,j++) d[j]=s[i];
}

void _add(void)
{
    char c = 0;
    for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],R3[i],&c);
}

char* _sub(void)
{
    char c = 0;
    cmp(R3);
    for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
}

void shl(void)      // T - 0
{
    char t[40];
    for(int i=0;i<40;i++) t[i] = 0;
    for(int i=39;i>0;i--) t[i-1]=A[i];
    t[39]=Q[1];
    mv(A,t);
    for(int i=0;i<40;i++) t[i] = 0;
    for(int i=39;i>1;i--) t[i-1]=Q[i];
    t[0] = Q[0];
    mv(Q,t);
}

void shr(void)      // T - 1
{
    char t[40]; char s=A[39];
    for(int i=0;i<40;i++) t[i] = 0;
    for(int i=0;i<39;i++) t[i+1]=A[i];
    t[0]=A[0];
    mv(A,t);
    for(int i=0;i<40;i++) t[i] = 0;
    for(int i=1;i<39;i++) t[i+1]=Q[i];
    t[1] = s; t[0] = Q[0];
    mv(Q,t);
}

void set(char* m, long v)
{
    unsigned long vu=0;

    vu = (unsigned long) v;
    for(int i=39;i>=0;i--) {
        m[i]=vu%2;
        vu/=2;
    }
}

void setsex(char* m, char *sls, char *srs)
{
    char op[8], addr[12];
    char *sl, *sr, *osl, *osr;
    osl=calloc(strlen(sls),sizeof(sls));
    osr=calloc(strlen(srs),sizeof(srs));
    sl=osl;sr=osr;
    int t=0;
    for(;*sls;sls++,osl++) { 
        *osl=toupper(*sls); 
    }
    for(;*srs;srs++,osr++) { 
        *osr=toupper(*srs);  
    }

    strcpy(op,cel[sl[0]]);strcat(op,cel[sl[1]]);
    for(int i=0;i<8;i++) m[i]=op[i]-'0';

    strcpy(addr,cel[sl[2]]);
    strcat(addr,cel[sl[3]]);strcat(addr,cel[sl[4]]);
    for(int i=10,j=2;i<20;i++,j++) m[i]=addr[j]-'0';

    strcpy(op,cel[sr[0]]);strcat(op,cel[sr[1]]);
    for(int i=20,j=0;i<28;i++,j++) m[i]=op[j]-'0';

    strcpy(addr,cel[sr[2]]);
    strcat(addr,cel[sr[3]]);strcat(addr,cel[sr[4]]);
    for(int i=30,j=2;i<40;i++,j++) m[i]=addr[j]-'0';
}

void mul(char *v)
{
    mv(R3,v);
    for(int i=39;i>0;i--) {
        if(Q[39]==1) { _add(); mv(A,ADDR); shr(); }
        else { shr(); }
    }
    if(Q[0]==1) { _sub(); mv(A,ADDR); }
}

void idiv(int y)
{
    mv(R3,M[y]);
    for(int i=39;i>0;i--) {
        _sub();
        if(ADDR[0]==1) { Q[39]=0; shl(); } 
        else {
            Q[39]=1;
            mv(A,ADDR);
            shl();
        }
    }
}

int op0(char v, char* ad)
{
    int n = fer(ad);
    n%=64;
    if (n==0) {HLTF = 1; return HLTF; }
    switch(v) {
        case '1':
        case '3':
        case '5':
        case '7':
            for(int i=0;i<40;i++) A[i] = 0;
            for(int i=0;i<n;i++) shl();
            break;
        case '0':
        case '2':
        case '4':
        case '6':
             for(int i=0;i<n;i++) shl();
            break;
        case '8':
        case 'K':
        case 'N':
            HLTF = 1;
            break;
        case '9':
        case 'S':
        case 'J':
        case 'L':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
            for(int i=0;i<n;i++) shl();
            break;
        case 'F':
            HLTF = 1;
            break;
        default:
            break;
    }
}

int op1(char v, char* ad)
{
    int n = fer(ad);
    n%=64;
    if (n==0) {HLTF = 1; return HLTF; }
    switch(v) {
        case '1':
        case '3':
        case '5':
        case '7':
            for(int i=0;i<40;i++) A[i] = 0;
            for(int i=0;i<n;i++) shr();
            break;
        case '0':
        case '2':
        case '4':
        case '6':
             for(int i=0;i<n;i++) shr();
            break;
        case '8':
        case 'K':
        case 'N':
            HLTF = 16;
            break;
        case '9':
        case 'S':
        case 'J':
        case 'L':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
            for(int i=0;i<n;i++) shr();
            break;
        case 'F':
            HLTF = 1;
            break;
        default:
            break;
    }
}

void op2(char v, char *ad)
{
    switch(v) {
        case '0':
            HLTF = 1;
            PC = fer(ad);
            lfto = 0;
            break;
        case '2':
            PC = fer(ad);
            lfto = 0;
            break;
        case '4':
            HLTF = 1;
            PC = fer(ad);
            lfto = 1;
            break;
        case '6':
            PC = fer(ad);
            lfto = 1;
            break;
        case '1':
            for(int i=0;i<40;i++) A[i] = 0;
            HLTF = 1;
            PC = fer(ad);
            lfto = 0;
            break;
        case '3':
            for(int i=0;i<40;i++) A[i] = 0;
            PC = fer(ad);
            lfto = 0;
            break;
        case '5':
            for(int i=0;i<40;i++) A[i] = 0;
            HLTF = 1;
            PC = fer(ad);
            lfto = 1;
            break;
        case '7':
            for(int i=0;i<40;i++) A[i] = 0;
            PC = fer(ad);
            lfto = 1;
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
            HLTF = 1;
            PC = fer(ad);
            lfto = 0;
            break;
        case 'S':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
            PC = fer(ad);
            lfto = 0;
            break;
        case 'J':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
            HLTF = 1;
            PC = fer(ad);
            lfto = 1;
            break;
        case 'L':
            for(int i=0;i<40;i++) A[i] = 0;
            A[1]=1;
             PC = fer(ad);
            lfto = 1;
            break;
        default:
            break;
    }
}

void op3(char v, char *ad)
{
    switch(v) {
        case '0':
        case '2':
        case '4':
        case '6':
            if(A[0]==0) op2(v,ad);
            break;
        default:
            op2(v,ad);
            break;
    }
}

void op4(char v, char *ad)
{
    int addr = fer(ad);
    switch(v) {
        case '0':
        case '4':
            mv(M[addr],A);
            break;
        case '1':
        case '5':
            for(int i=0;i<40;i++) { A[i] = 0; M[addr][i] = 0; }
            break;
        case '2':
            for(int i=30;i<40;i++) M[addr][i]=A[i];
            break;
        case '3':
            for(int i=0;i<40;i++) A[i]=0;
            for(int i=30;i<40;i++) M[addr][i]=A[i];
            break;
        case '6':
            for(int i=10;i<20;i++) M[addr][i]=A[i];
            break;
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            for(int i=10;i<20;i++) M[addr][i]=A[i];
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case 'S':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            for(int i=30;i<40;i++) M[addr][i]=0;
            break;
        case 'L':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            for(int i=10;i<20;i++) M[addr][i]=0;
            break;
        default:
            break;
    }
}

void op5(char v, char *ad)
{
    int addr = fer(ad);
    switch(v) {
        case '0':
        case '2':
        case '4':
        case '6':
            mv(Q,M[addr]);
            break;
        case '1':
        case '3':
        case '5':
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            mv(Q,M[addr]);
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
        case 'S':
        case 'J':
        case 'L':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            mv(Q,M[addr]);
            break;
        default:
            break;
    }
}

void op6(char v, char *ad)
{
    int addr = fer(ad);
    switch(v) {
        case '6':
            if(gt(iabs(A),iabs(M[addr]))) { idiv(addr); HLTF=6; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=6;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            if(gt(iabs(A),iabs(M[addr]))) { idiv(addr); HLTF=6; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=6;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case 'L':
        case 'S':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            if(gt(iabs(A),iabs(M[addr]))) { idiv(addr); HLTF=6; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=6;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '9':
        case 'J':
            for(int i=0;i<40;i++) A[i]=0;
            idiv(addr);
            break;
        default:
            break;
    }
}

void op7(char v, char *ad)
{
    int addr=fer(ad);
    char *t;
    t=calloc(40,sizeof(char));
    switch(v) {
        case '0':
            cmp(M[addr]);
            mul(CMPG);
            break;
        case '1':
            for(int i=0;i<40;i++) A[i]=0;
            cmp(M[addr]);
            mul(CMPG);
            break;
        case '2':
            cmp(iabs(M[addr]));
            mul(CMPG);
            break;
        case '3':
            for(int i=0;i<40;i++) A[i]=0;
            cmp(iabs(M[addr]));
            mul(CMPG);
            break;
        case '4':
            mul(M[addr]);
            break;
        case '5':
            for(int i=0;i<40;i++) A[i]=0;
            mul(M[addr]);
            break;
        case '6':
            mul(iabs(M[addr]));
            break;
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            mul(iabs(M[addr]));
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF=16;
            break;
        case '9':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            cmp(M[addr]);
            mul(CMPG);
            break;
        case 'S':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            cmp(iabs(M[addr]));
            mul(CMPG);
            break;
        case 'J':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            mul(M[addr]);
            break;
        case 'L':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            mul(iabs(M[addr]));
            break;
    }
}

char *readp(char *p)
{
    char *r;
    char c;
    int j=0;
    r=calloc(5,sizeof(char));
    while((c=*p++)!='\n') {
        if(c=='O') r[j]=1;
        else if(c=='o') continue;
        else r[j]=0;
        j++;
    }
    return r;
}

void punchp(char *v)
{
    char t[7];
    t[0]=' ';
    t[1]=(v[0]==0)? ' ' : 'O';
    t[2]='o';
    t[3]=(v[1]==0)? ' ' : 'O';
    t[4]=(v[2]==0)? ' ' : 'O';
    t[5]=(v[3]==0)? ' ' : 'O';
    t[6]=0;
    fputs(t,fo);
}

void punch5p(char *v)
{
    char t[7];
    t[0]=(v[0]==0)? ' ' : 'O';
    t[1]=(v[1]==0)? ' ' : 'O';
    t[2]='o';
    t[3]=(v[2]==0)? ' ' : 'O';
    t[4]=(v[3]==0)? ' ' : 'O';
    t[5]=(v[4]==0)? ' ' : 'O';
    t[6]=0;
    fputs(t,fo);
}

void exc(char *);

void op8(char v, char **ad)
{
    char l[5];
    char t[20];
    char op[8];
    char adr[10];
    int nd = fer(*ad);
    int dad = 0;
    switch(v) {
        case '0':
            for(int i=0;i<nd/4;i++) {
                shl();shl();shl();shl();
                fgets(BF,8,fi);
                mvn(l,readp(BF),0,4);
                A[36]=l[1]; A[37]=l[2];
                A[38]=l[3]; A[39]=l[4];
            }
            break;
        case '1':
        case '9':
            for(int i=0;i<40;i++) A[i]=0;
            for(int i=0;i<nd/4;i++) {
                shl();shl();shl();shl();
                fgets(BF,6,fi);
                mvn(l,readp(BF),0,4);
                A[36]=l[1]; A[37]=l[2];
                A[38]=l[3]; A[39]=l[4];
            }
            break;
        case '2':
            for(int i=0;i<nd/4;i++) {
                punchp(A);
                shl();shl();shl();shl();
            }
            break;
        case '3':
            for(int i=0;i<40;i++) A[i]=0;
            for(int i=0;i<nd/4;i++) {
                punchp(A);
                shl();shl();shl();shl();
            }
            break;
        case '5':
            // DRUM SPECIAL VARIANT (40 bit)
            mvn(t,M[PC-1],20,39);
            mvn(op,t,0,7);
            mvn(adr,t,6,19);
            dad = drufer(adr); dad%=16384;
            switch(luxV(op)) {
                case '0':
                case '1':
                case '8':
                case '9':
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fread(DBF,sizeof(char),11,dr);
                    A[29]=DBF[0];A[30]=DBF[1];A[31]=DBF[2];A[32]=DBF[3];
                    A[33]=DBF[4];A[34]=DBF[5];A[35]=DBF[6];A[36]=DBF[7];
                    A[37]=DBF[8];A[38]=DBF[9];A[39]=DBF[10];
                    break;
                default:
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fread(DBF,sizeof(char),11,dr);
                    A[29]=DBF[0];A[30]=DBF[1];A[31]=DBF[2];A[32]=DBF[3];
                    A[33]=DBF[4];A[34]=DBF[5];A[35]=DBF[6];A[36]=DBF[7];
                    A[37]=DBF[8];A[38]=DBF[9];A[39]=DBF[10];
                    lfto=0; exc(M[PC-1]);
                    break;
            }
            break;
        case '6':
            // DRUM SPECIAL VARIANT (40 bit)
            mvn(t,M[PC-1],20,39);
            mvn(op,t,0,7);
            mvn(adr,t,6,19);
            dad = drufer(adr); dad%=16384;
            switch(luxV(op)) {
                case '0':
                case '1':
                case '8':
                case '9':
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    break;
                default:
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    lfto=0; exc(M[PC-1]);
                    break;
            }
            break;
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            // DRUM SPECIAL VARIANT (40 bit)
            mvn(t,M[PC-1],20,39);
            mvn(op,t,0,7);
            mvn(adr,t,6,19);
            dad = drufer(adr); dad%=16384;
            switch(luxV(op)) {
                case '0':
                case '1':
                case '8':
                case '9':
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    break;
                default:
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    lfto=0; exc(M[PC-1]);
                    break;
            }
            break;
        case 'S':
            for(int i=0;i<40;i++) A[i]=0; A[1]=1;
            for(int i=0;i<nd/4;i++) {
                punchp(A);
                shl();shl();shl();shl();
            }
            break;
        case 'L':
            for(int i=0;i<40;i++) A[i]=0; A[1]=1;
            // DRUM SPECIAL VARIANT (40 bit)
            mvn(t,M[PC-1],20,39);
            mvn(op,t,0,7);
            mvn(adr,t,6,19);
            dad = drufer(adr); dad%=16384;
            switch(luxV(op)) {
                case '0':
                case '1':
                case '8':
                case '9':
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    break;
                default:
                    DBF[0]=A[0];DBF[1]=A[1];DBF[2]=A[2];DBF[3]=A[3];
                    DBF[4]=A[4];DBF[5]=A[5];DBF[6]=A[6];DBF[7]=A[7];
                    DBF[8]=A[8];DBF[9]=A[9];DBF[10]=A[10];
                    shl();shl();shl();shl();
                    shl();shl();shl();shl();
                    shl();shl();shl();
                    fseek(dr,dad,SEEK_SET);
                    fwrite(DBF,sizeof(char),11,dr);
                    lfto=0; exc(M[PC-1]);
                    break;
            }
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        default:
            break;
    }
}

void op9(char v, char *ad)
{
    int nd = fer(ad);
    int b = 0;
    char l[5];
    char d5[5], nc[6];
    switch(v) {
        case '1':
            for(int i=0;i<40;i++) A[i]=0;
            shr();shr();shr();shr();
            fgets(BF,6,fi);
            strcpy(l,readp(BF));
            A[36]=l[1]; A[37]=l[2];
            A[38]=l[3]; A[39]=l[4];
            A[0]=l[0];
            break;
        case '2':
            d5[0]=ad[9];d5[1]=ad[0];d5[2]=ad[1];d5[3]=ad[2];d5[4]=ad[3];
            nc[5]=ad[9];nc[4]=ad[8];nc[3]=ad[7];nc[2]=ad[6];nc[1]=ad[5];nc[0]=ad[4];
            b = ceil(b2d(nc,6)/4.0);
            for(int i=0;i<b;i++) {
                punch5p(d5);
            }
            break;
        default:
            break;
    }
}

void opK(char v, char *ad)
{
    int addr = fer(ad);
    char c=0;
    switch(v) {
        case '0':
            cmp1(Q);
            for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case '1':
            cmp1(Q);
            mv(A,CMPG);
            break;
        case '2':
            if(Q[0]==0) {
                cmp1(Q);
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                c=0;
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
                mv(A,ADDR);
            }
            break;
        case '3':
            if(Q[0]==0) {
                cmp1(Q);
                mv(A,CMPG);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                mv(A,ADDR);
            }
            break;
        case '4':
            for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
            c=0;
            for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
            mv(A,ADDR);
            break;
        case '5':
            for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
            mv(A,ADDR);
            break;
        case '6':
            if(Q[0]==1) {
                cmp1(Q);
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                c=0;
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
                mv(A,ADDR);
            }
            break;
        case '7':
            if(Q[0]==1) {
                cmp1(Q);
                mv(A,CMPG);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                mv(A,ADDR);
            }
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
            cmp1(Q);
            for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
            mv(A,ADDR);
            break;
        case 'S':
            if(Q[0]==0) {
                cmp1(Q);
                for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
                mv(A,ADDR);
            }
            break;
        case 'J':
            for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
            for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
            mv(A,ADDR);
            break;
        case 'L':
            if(Q[0]==1) {
                cmp1(Q);
                for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(Q[i],ONE[i],&c);
                for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
                mv(A,ADDR);
            }
            break;
        default:
            break;
    }
}

void opS(char v, char *ad)
{
    char c=0;
    switch(v) {
        case '0':
            mv(R3,Q);
            _sub();
            mv(A,ADDR);
            break;
        case '1':
            cmp(Q);
            mv(A,CMPG);
            break;
        case '2':
            mv(R3,iabs(Q));
            _sub();
            mv(A,ADDR);
            break;
        case '3':
            mv(R3,iabs(Q));
            cmp(R3);
            mv(R3,CMPG);
            _sub();
            mv(A,ADDR);
            break;
        case '4':
            mv(R3,Q);
            _add();
            mv(A,ADDR);
            break;
        case '5':
            mv(A,Q);
            break;
        case '6':
            mv(R3,iabs(Q));
            _add();
            mv(A,ADDR);
            break;
        case '7':
            mv(A,iabs(Q));
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
            cmp(Q);
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case 'S':
            cmp(iabs(Q));
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case 'J':
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],Q[i],&c);
            mv(A,ADDR);
            break;
        case 'L':
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],iabs(Q)[i],&c);
            mv(A,ADDR);
            break;
        default:
            break;
    }
}

void opJ(char v, char *ad)
{
    int addr = fer(ad);
    switch(v) {
        case '0':
        case '2':
        case '4':
        case '6':
            for(int i=0;i<40;i++) {
                if((M[addr][i]==1)&&(Q[i]==1)) Q[i]=1;
                else Q[i]=0;
            }
            break;
        case '1':
        case '3':
        case '5':
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            for(int i=0;i<40;i++) {
                if((M[addr][i]==1)&&(Q[i]==1)) Q[i]=1;
                else Q[i]=0;
            }
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
        case 'S':
        case 'J':
        case 'L':
            A[1]=1;
            for(int i=0;i<40;i++) {
                if((M[addr][i]==1)&&(Q[i]==1)) Q[i]=1;
                else Q[i]=0;
            }
            break;
        default:
            break;
    }
}

void opF(char v, char *ad)
{
    int addr = fer(ad);
    char c=0;
    char t[40];
    mv(t,M[addr]);
    switch(v) {
        case '0':
            cmp1(t);
            for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case '1':
            cmp1(t);
            mv(A,CMPG);
            break;
        case '2':
            if(t[0]==0) {
                cmp1(t);
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                c=0;
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
                mv(A,ADDR);
            }
            break;
        case '3':
            if(t[0]==0) {
                cmp1(t);
                mv(A,CMPG);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                mv(A,ADDR);
            }
            break;
        case '4':
            for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
            c=0;
            for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
            mv(A,ADDR);
            break;
        case '5':
            for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
            mv(A,ADDR);
            break;
        case '6':
            if(t[0]==1) {
                cmp1(Q);
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                c=0;
                for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],ADDR[i],&c);
                mv(A,ADDR);
            }
            break;
        case '7':
            if(t[0]==1) {
                cmp1(t);
                mv(A,CMPG);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                mv(A,ADDR);
            }
            break;
        case '8':
        case 'K':
        case 'N':
            HLTF = 16;
            break;
        case '9':
            cmp1(t);
            for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
            mv(A,ADDR);
            break;
        case 'S':
            if(t[0]==0) {
                cmp1(t);
                for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
                mv(A,ADDR);
            }
            break;
        case 'J':
            for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
            for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
            mv(A,ADDR);
            break;
        case 'L':
            if(t[0]==1) {
                cmp1(t);
                for(int i=39;i>=0;i--) ADDR[i]=ab(CMPG[i],HALF[i],&c);
                mv(A,ADDR);
            } else {
                for(int i=39;i>=0;i--) ADDR[i]=ab(t[i],ONE[i],&c);
                for(int i=39;i>=0;i--) ADDR[i]=ab(ADDR[i],HALF[i],&c);
                mv(A,ADDR);
            }
            break;
        case 'F':
            HLTF = 1;
        default:
            break;
    }
}

void opL(char v, char *ad)
{
    int addr = fer(ad);
    char t[40];
    char c=0;
    mv(t,M[addr]);
    switch(v) {
        case '0':
            mv(R3,t);
            _sub();
            mv(A,ADDR);
            break;
        case '1':
            cmp(t);
            mv(A,CMPG);
            break;
        case '2':
            mv(R3,iabs(t));
            _sub();
            mv(A,ADDR);
            break;
        case '3':
            mv(R3,iabs(t));
            cmp(R3);
            mv(R3,CMPG);
            _sub();
            mv(A,ADDR);
            break;
        case '4':
            mv(R3,t);
            _add();
            mv(A,ADDR);
            break;
        case '5':
            mv(A,t);
            break;
        case '6':
            mv(R3,iabs(t));
            _add();
            mv(A,ADDR);
            break;
        case '7':
            mv(A,iabs(t));
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 16;
            break;
        case '9':
            cmp(t);
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case 'S':
            cmp(iabs(t));
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],CMPG[i],&c);
            mv(A,ADDR);
            break;
        case 'J':
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],t[i],&c);
            mv(A,ADDR);
            break;
        case 'L':
            for(int i=39;i>=0;i--) ADDR[i]=ab(HALF[i],iabs(t)[i],&c);
            mv(A,ADDR);
            break;
        default:
            break;
    }
}

void exc(char *w)
{
    char t[20];
    char *op;
    char *ad;
    op=calloc(8,sizeof(char));
    ad=calloc(10,sizeof(char));
    if (lfto) { mvn(t,w,0,19); lfto=0; } else { mvn(t,w,20,39); lfto=1; }
    mvn(op,t,0,7);
    mvn(ad,t,10,19);
    if (lfto) PC++;
    switch(luxT(op)) {
        case '0':
            op0(luxV(op),ad);
            break;
        case '1':
            op1(luxV(op),ad);
            break;
        case '2':
            op2(luxV(op),ad);
            break;
        case '3':
            op3(luxV(op),ad);
            break;
        case '4':
            op4(luxV(op),ad);
            break;
        case '5':
            op5(luxV(op),ad);
            break;
        case '6':
            op6(luxV(op),ad);
            break;
        case '7':
            op7(luxV(op),ad);
            break;
        case '8':
            op8(luxV(op),&ad);
            break;
        case '9':
            op9(luxV(op),ad);
            break;
        case 'K':
            opK(luxV(op),ad);
            break;
        case 'S':
            opS(luxV(op),ad);
            break;
        case 'J':
            opJ(luxV(op),ad);
            break;
        case 'F':
            opF(luxV(op),ad);
            break;
        case 'L':
            opL(luxV(op),ad);
            break;
        default:
            break;
    }
}

int bs(void)
{
    char *bf, *obf, w[30];
    char lw[20], rw[20];
    char c;
    int nw=1, l=0, ol=0;
    bf=calloc(30,sizeof(char));
    obf=bf;
    for(int i=0;i<30;i++) { bf[i]=0; w[i]=0; }
    fgets(bf,29,stdin);
    printf("B/");fgets(bf,29,stdin);
    ol=strtol(bf,NULL,10); l=ol;
    printf("B/%04d ",l);
    while(fgets(bf,29,stdin)) {
        if(strcmp(bf,"\n")==0) continue;
        if (bf[0]=='.') break;
        obf=strchr(bf,'\n');*obf=0;
        if(nw==1) {
            strcpy(lw,bf);
            nw++;
        }
        else {
            strcpy(rw,bf); 
            setsex(M[l++],lw,rw); 
            nw=1; 
            pw(M[l-1]);
        }
        printf("B/%04d ",l);
    }
    exc(M[ol]); //left side
    exc(M[ol]); //right side
    PC=ol;
}

int main(int n, char **a)
{
    char c;
    i0();

    //fi=fopen(a[1],"r+");
    fi=fopen("doi.pt","r+");
    fo=fopen("out.pt","w+");
    dr=fopen("drum.bin","wb+");

    printf("ILC\n");
    printf("? "); c = getc(stdin);
    while(c!='.') {
        if(c=='b') {
            fflush(stdin);bPC=bs();
        }
        while(HLTF == 0) {
            if(HLTF)printf("H%d",HLTF);
            exc(M[PC]);
        }
        printf("? "); c = getc(stdin);
        HLTF=0;
    }

    fclose(fi); fclose(fo); fclose(dr);
    return 0;
}