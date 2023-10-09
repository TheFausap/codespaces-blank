// Paper Tape Sex Enconder
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

static struct termios oldt, newt;

char *cel[] = {  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 
                 0, 0, 0, 0, 0, 0, 0, 0, "0000", "0001",
                 "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 "1110", 0, 0, 0, "1101", "1010", "1111", 0, "1100", 0, 
                 0, 0, 0, "1011" };

FILE *fi, *fo;

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
    fputs(t,fo);
    fputs("\n",fo);
}

int main(int n, char **a)
{
    char c;
    int ccnt=1;
    fo=fopen("out1.pt","w+");
    tcgetattr( STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_lflag &= ~(ICANON);
    newt.c_oflag |= (OPOST|OLCUC|ONLRET);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    printf("PTP\n");
    while(c=fgetc(stdin)) {
        if(ccnt==2) printf("XX");
        if(ccnt==5) printf("__");
        if(ccnt==7) printf("XX");
        if(ccnt==10) { puts(""); ccnt=0; }
        if(c=='.') break;
        else {
            punchp(cel[toupper(c)]);
        }
        ccnt++;
    }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    fclose(fo);
    return 0;
}
