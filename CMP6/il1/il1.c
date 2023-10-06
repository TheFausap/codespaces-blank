#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1bit (sign) + 39bits

char A[40];
char R3[40];
char Q[40];
char ADDR[40];
char CMPG[40];
char M[1000][40];
char ONE[40];
int PC = 3;

char HLTF = 0;
char lfto = 1;

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

char lux(char *s) {
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

int fer(char *s) {
    int j=0, r=0;
    for(;*s++;j++) {
        r += (*s*(1<<(9-j)));
    }
}

void i0()
{
    for(int i=0;i<1000;i++) {
        for(int j=0;j<40;j++) M[i][j]=0;
    }
    for(int i=0;i<40;i++) { A[i] = 0; R3[i] = 0; Q[i] = 0; ONE[i] = 0; ADDR[0]; }
    ONE[39] = 1;
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

void cadd(int y)    // T - L
{
    char c = 0;
    mv(R3,M[y]);
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void absadd(int y)
{
    char c = 0;
    mv(R3,M[y]);
    if(R3[0]) cmp(R3);
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

char* _sub(void)
{
    char c = 0;
    cmp(R3);
    for(int i=39;i>=0;i--) ADDR[i]=ab(A[i],CMPG[i],&c);
}

void csub(int y)
{
    char c = 0;
    mv(R3,M[y]);
    cmp(R3);
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],CMPG[i],&c);
}

void abssub(int y)
{
    char c = 0;
    mv(R3,M[y]);
    if(R3[0]) cmp(R3);
    cmp(R3);
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void cia(int y)     // T - F
{
    char c = 0;
    mv(R3,M[y]);
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) R3[i]=ab(R3[i],ONE[i],&c);
    c = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void cis(int y)
{
    char c = 0;
    mv(R3,M[y]);
    for(int i=0;i<40;i++) R3[i]^=1;
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void addq(void)     // T - S
{
    char c = 0;
    mv(R3,Q);
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void ciaq(int y)    // T - K
{
    char c = 0;
    mv(R3,Q);
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) R3[i]=ab(R3[i],ONE[i],&c);
    c = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void subq(int y)
{
    char c = 0;
    mv(R3,Q);
    cmp(R3);
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
}

void cisq(int y)
{
    char c = 0;
    mv(R3,Q);
    for(int i=0;i<40;i++) R3[i]^=1;
    for(int i=0;i<40;i++) A[i] = 0;
    for(int i=39;i>=0;i--) A[i]=ab(A[i],R3[i],&c);
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

void op0(char v, char* ad)
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

void op1(char v, char* ad)
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
            HLTF = 1;
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
            HLTF = 1;
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
            HLTF = 1;
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
            if(gt(iabs(A),iabs(M[addr]))) { idiv(addr); HLTF=1; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=1;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case '7':
            for(int i=0;i<40;i++) A[i]=0;
            if(gt(iabs(A),iabs(M[addr]))) { idiv(addr); HLTF=1; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=1;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case 'L':
        case 'S':
            for(int i=0;i<40;i++) A[i]=0;
            A[1]=1;
            if(gt(abs(A),iabs(M[addr]))) { idiv(addr); HLTF=1; }
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==0)) { idiv(addr); HLTF=1;}
            else if((eq(iabs(A),iabs(M[addr])))&&(A[0]==1)) { idiv(addr); }
            break;
        case '8':
        case 'K':
        case 'N':
        case 'F':
            HLTF = 1;
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
            HLTF=1;
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

void exc(void)
{
    char t[20];
    char op[8];
    char ad[10];
    if (lfto) { mvn(t,M[PC],0,19); lfto=0; } else { mvn(t,M[PC],20,39); lfto=1; }
    mvn(op,t,0,7);
    mvn(ad,t,10,19);
    switch(lux(op)) {
        case '0':
            op0(lux(op),ad);
            break;
        case '1':
            op1(lux(op),ad);
            break;
        case '2':
            op2(lux(op),ad);
            break;
        case '3':
            op3(lux(op),ad);
            break;
        case '4':
            op4(lux(op),ad);
            break;
        case '5':
            op5(lux(op),ad);
            break;
        case '6':
            op6(lux(op),ad);
            break;
        case '7':
            op7(lux(op),ad);
            break;
        default:
            break;
    }
}

int main(int n, char **a)
{
    char c;
    i0();

    set(Q,42);
    set(M[100],4);
    idiv(100);
#if 0
    printf("ILC\n");
    printf("? "); c = getc(stdin);
    while(c!='.') {
        while(HLTF != 1) {
            exc();
        }
        printf("? "); c = getc(stdin);
    }
#endif

    return 0;
}