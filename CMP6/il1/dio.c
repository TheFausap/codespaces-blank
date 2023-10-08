// D.I.O - Decimal input order
// alias DOI (Decimal order input)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

static struct termios oldt, newt;

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

FILE *fi, *fo;

unsigned long M[4096];

char *d2sex(int v)
{
    char *r, *or;
    int vv = 0;
    r = calloc(4,sizeof(char));
    r[0]=48;r[1]=48;r[2]=48;r[3]=0;
    or = r; r+=2;
    while(v!=0) {
        vv = v%16;
        *r--=chas[vv];
        v/=16;
    }
    return strdup(or);
}

char *d2b(unsigned long v)
{
    char *r, *or;
    unsigned long vv = 0;
    r = calloc(41,sizeof(char));
    for(int i=0;i<39;i++) r[i]='0';
    or = r; r+=39;
    while(v!=0) {
        *r--=(v%2)+'0';
        v/=2;
    }
    return strdup(or);
}

char *readp(char *p)
{
    char r[5];
    int j=0;
    while(*p!='\n') {
        if(*p=='O') r[j]='1';
        else if(*p=='o') continue;
        else r[j]='0';
        j++;
    }
    return strdup(r);
}

void punchp(char *v)
{
    char t[7];
    t[0]=' ';
    t[1]=(v[0]=='0')? ' ' : 'O';
    t[2]='o';
    t[3]=(v[1]=='0')? ' ' : 'O';
    t[4]=(v[2]=='0')? ' ' : 'O';
    t[5]=(v[3]=='0')? ' ' : 'O';
    t[6]=0;
    fputs(t,fo);fputs("\t\t// ",fo);fputs(v,fo);
    fputs("\n",fo);
}

void mvn(char *d, char *s, int ss, int se)
{
    int i,j;
    for(i=ss,j=0;i<=se;i++,j++) d[j]=s[i];
    d[j]=0;
}

#define last(x) x[strlen(x)-2]
#define cl(x,y) for(int _i=0;_i<y;_i++) x[_i]=0;
#define toups(x,y) for(int _i=0;_i<y;_i++) x[_i]=toupper(x[_i]);

int main(int n, char **a)
{
    char c=0;
    char bf[80], op[3];
    char t[10], t1[12];
    char *td;
    int loc=0, iloc=1, oloc=0;
    int ln=0;
    unsigned long tn=0;
    unsigned long jv=0, jvv=0;
    long double tj=0;

    td=calloc(6,sizeof(char));
    fo=fopen("out1.pt","w+");
    tcgetattr( STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_oflag |= (OPOST|OLCUC|ONLRET);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    printf("D.I.O begins\n");
    printf("%d _ ",loc);
    while(fgets(bf,79,stdin)) {
        cl(t,10);
        toups(bf,80);
        if (bf[0]=='.') break;
        ln=strlen(bf)-2;
        if ((last(bf)=='k')||(last(bf)=='K')) {
            if((ln-1<2)) loc=0;
            else { 
                mvn(t,bf,2,ln-1);
                loc=strtol(t,NULL,10);
                oloc=loc;
            }
        }
        if ((last(bf)=='f')||(last(bf)=='F')) {
            strcpy(t,cel[bf[0]]);
            strcat(t,cel[bf[1]]);
            // left order order
            jvv=strtol(t,NULL,2);
            jvv<<=12;
            jv|=jvv;
            if((ln-1<2)) tn=0;
            else { 
                mvn(t,bf,2,ln-1);
                tn=strtol(t,NULL,10);
            }
            jv|=tn; 
            if(iloc==1) jv<<=20;
            if(iloc==2) {M[loc++]=jv; iloc=1;}
            iloc++;
        }
        if ((last(bf)=='l')||(last(bf)=='L')) {
            strcpy(t,cel[bf[0]]);
            strcat(t,cel[bf[1]]);
            // left order order
            jvv=strtol(t,NULL,2);
            jvv<<=12;
            jv|=jvv;
            if((ln-1<2)) tn=0+oloc;
            else { 
                mvn(t,bf,2,ln-1);
                tn=strtol(t,NULL,10)+oloc;
            }
            jv|=tn; 
            if(iloc==1) jv<<=20;
            if(iloc==2) {M[loc++]=jv; iloc=1;}
            iloc++;
        }
        if ((last(bf)=='j')||(last(bf)=='J')) {
            strcpy(t,cel[bf[0]]);
            strcat(t,cel[bf[1]]);
            // left order order
            jvv=strtol(t,NULL,2);
            jvv<<=12;
            jv|=jvv;
            if((ln-1<2)) tn=0;
            else { 
                mvn(t1,bf,2,ln-1);
                tj=strtold(t1,NULL);
                tj/=(long double)1000000000000;
                tj*=(1LL<<39);
                tn=(unsigned long)truncl(tj);
            }
            jv+=tn; 
            if(iloc==1) { jv<<=20; jv&=0xFFFFFFFFFF; }
            if(iloc==2) {M[loc++]=jv&=0xFFFFFFFFFF; iloc=0;}
            iloc++;
        }
        printf("%d ? ",loc);
    }

    printf("D.I.O. ended\n");

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

    printf("Assembling at loc: %d\n",oloc);
    for(int i=oloc;i<loc;i++) printf("%d: %s\n",i,d2b(M[i]));
    fclose(fo);
    return 0;
}