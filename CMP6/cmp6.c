// CMP6.c : Defines the entry point for the application.
//

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define CS(x,y) case x:return y;break;case y:return x;break
#define E(x) fprintf(stderr,"%s",x)

char M[1000][12];
char SR[6];
char CR[12];
char CC[12];
char rA[12];
char rX[12];
char rL[12];
char rF[12];

char ONE[] = {'0','0','0','0','0','1'};

void a10(char* a, char* b)
{
#define ca(x) (a[x]-'0')+(b[x]-'0')

    char c = 0;
    int t;

    t = ca(11);
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[11] = t + '0';

    t = ca(10) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[10] = t + '0';

    t = ca(9) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[9] = t + '0';

    t = ca(8) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[8] = t + '0';

    t = ca(7) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[7] = t + '0';

    t = ca(6) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[6] = t + '0';

    t = ca(5) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[5] = t + '0';

    t = ca(4) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[4] = t + '0';

    t = ca(3) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[3] = t + '0';

    t = ca(2) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[2] = t + '0';

    t = ca(1) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[1] = t + '0';

    t = ca(0) + c;
    if (t > 9) { t -= 10; c = 1; }
    else c = 0;
    a[0] = t + '0';
}

char cmp(unsigned char c)
{
    switch (c) {
        CS('i', ' ');
        CS('\n', ',');
        CS('\t', '\"');
        CS(167, 223);
        CS('-', '0');
        CS('.', ';');
        CS('|', ')');
        CS(':', '+');
        CS('1', '2');
        CS('A', 'B');
        CS('J', 'K');
        CS('/', 'S');
        CS('3', '4');
        CS('C', 'D');
        CS('L', 'M');
        CS('T', 'U');
        CS('5', '6');
        CS('E', 'F');
        CS('N', 'O');
        CS('V', 'W');
        CS('7', '8');
        CS('G', 'H');
        CS('P', 'Q');
        CS('X', 'Y');
        CS('9', '\'');
        CS('I', '#');
        CS('R', '$');
        CS('Z', '%');
        CS('&', '(');
        CS(162, '@');
        CS('*', '?');
        CS('=', '\\');
    default:
        break;
    }
}

#define TA(x) switch(a[x]){\
    case '-':ta=-1;break;case '\'':ta=10;break;\
    case '&':ta=11;break;case '(':ta=12;break;\
    default:if(isdigit(a[x])) {ta=a[x]-'0'; anumf=1;} else anumf=0;}

#define TB(x) switch(b[x]){\
    case '-':ta=-1;break;case '\'':ta=10;break;\
    case '&':ta=11;break;case '(':ta=12;break;\
    default:if(isdigit(b[x])) {tb=b[x]-'0'; bnumf=1;} else bnumf=0;}

#define PR(x) \
    TA(x) TB(x) \
    if (!subf) { \
        if((anumf)&&(bnumf)){ta=ta+tb+c;if(ta>9){ta-=10; c=1;}else c=0;}\
        else if((!anumf)&&(bnumf))ta=ta;\
        else if((anumf)&&(!bnumf))ta=tb;\
        else if((!anumf)&&(!bnumf))E("Adder alphabetic error\n");\
    } else {\
        if((anumf)&&(bnumf)){ota=ta;ta=ta-tb+c;if(ta<0){ ta=ota+10-tb; c=-1;} else c=0;}\
        else if((!anumf)&&(bnumf))ta=ta;\
        else if((anumf)&&(!bnumf))ta=tb;\
        else if((!anumf)&&(!bnumf))E("Adder alphabetic error\n");\
    }\
    r[x]=ta+'0'

char* gadd(char* a, char* b)
{
    int ta = 0, tb = 0, ota = 0, c = 0;
    char aneg = 0, bneg = 0, anumf = 0, bnumf = 0, subf = 0;
    char r[12];

    if (((a[0] != '0') && (a[0] != '-')) && ((b[0] != '0') && (b[0] != '-'))) E("Adder alphabetic error\n");
    if (a[0] == '-') aneg = 1;
    if (b[0] == '-') bneg = 1;
    subf = aneg ^ bneg;

    switch (a[11]) {
    case '-':ta = -1; break; case '\'':ta = 10; break; case '&':ta = 11; break; case '(':ta = 12; break; default:if (isdigit(a[11])) {
        ta = a[11] - '0'; anumf = 1;
    }
            else anumf = 0;
    } switch (b[11]) {
    case '-':ta = -1; break; case '\'':ta = 10; break; case '&':ta = 11; break; case '(':ta = 12; break; default:if (isdigit(b[11])) {
        tb = b[11] - '0'; bnumf = 1;
    }
            else bnumf = 0;
    } if (!subf) {
        if ((anumf) && (bnumf)) {
            ta = ta + tb + c; if (ta>9) {
                ta -= 10; c = 1;
            }
            else c = 0;
        }
        else if ((!anumf) && (bnumf))ta = ta; else if ((anumf) && (!bnumf))ta = tb; else if ((!anumf) && (!bnumf))fprintf(stderr, "%s", "Adder alphabetic error\n");
    }
    else {
        if ((anumf) && (bnumf)) {
            ota = ta;
            ta = ta - tb + c; if (ta<0) {
                ta = ota + 10 - tb; c = -1;
            }
            else c = 0;
        }
        else if ((!anumf) && (bnumf))ta = ta; else if ((anumf) && (!bnumf))ta = tb; else if ((!anumf) && (!bnumf))fprintf(stderr, "%s", "Adder alphabetic error\n");
    } r[11] = ta + '0';
    PR(10);
    PR(9); PR(8);
    PR(7); PR(6);
    PR(5); PR(4);
    PR(3); PR(2);
    PR(1);

    if ((a[0] != '0') && (a[0] != '-')) r[0] = a[0];
    else if ((b[0] != '0') && (b[0] != '-')) r[0] = b[0];

    return strdup(r);
}

void exc(void);

void alp(void)
{
    SR[0] = CC[6]; SR[1] = CC[7];
    SR[2] = CC[8]; SR[3] = CC[9];
    SR[4] = CC[10]; SR[5] = CC[11];
}

void bet(void)
{
    int ad = (SR[0] << 5) | (SR[1] << 4) | (SR[2] << 3) | (SR[3] << 2) | (SR[4] << 1) | SR[5];

    CR[0] = M[ad][0]; CR[1] = M[ad][1]; CR[2] = M[ad][2];
    CR[3] = M[ad][3]; CR[4] = M[ad][4]; CR[5] = M[ad][5];
    CR[6] = M[ad][6]; CR[7] = M[ad][7]; CR[8] = M[ad][8];
    CR[9] = M[ad][9]; CR[10] = M[ad][10]; CR[11] = M[ad][11];
    a10(CC, ONE);
}

void gam(void)
{
    SR[0] = CR[0]; SR[1] = CR[1];
    SR[2] = CR[2]; SR[3] = CR[3];
    SR[4] = CR[4]; SR[5] = CR[5];
    exc();
}

void delt(void)
{
    SR[0] = CR[6]; SR[1] = CR[7];
    SR[2] = CR[8]; SR[3] = CR[9];
    SR[4] = CR[10]; SR[5] = CR[11];
    exc();
}

void exc(void)
{
    
}

void set(int a, char* v)
{
    char c;
    int i = 0;

    while (c=*v++) {
        M[a][i++] = c;
    }
}

int main(int n, char** a)
{
    set(100, "000000021843");
    set(101, "-00002036675");

    gadd(M[100], M[101]);

    return 0;
}
