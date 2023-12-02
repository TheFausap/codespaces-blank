// Paper Tape Dec Enconder
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

FILE *fi, *fo;

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

void load()
{
    FILE *rt;
    char c;
    char *fn,*ofn,*ln;

    fn=calloc(10,sizeof(fn));
    ln=calloc(9,sizeof(ln));
    ofn=fn;
    printf(" _");
    while((c=fgetc(stdin))!='\n') {
        *fn=c;
        fn++;
    }
    rt=fopen(ofn,"r+");
    while(fgets(ln,8,rt))
    {
        fprintf(fo,"%s",ln);
    }
    fclose(rt);
}

int main(int n, char **a)
{
    char c;
    int ccnt=1,pos=0,sfl=0;
    fo=fopen("outde.pt","w+");
    tcgetattr( STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_lflag &= ~(ICANON);
#ifdef __APPLE__
    newt.c_oflag |= (OPOST|ONLRET);
#else
    newt.c_oflag |= (OPOST|OLCUC|ONLRET);
#endif
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    if(n>0) {
        pos=sex2d(a[1]);
        printf("PTP/DE %s\n", a[1]);
        printf("%03d _ ",pos);
    } else printf("PTP/DE 0\n");
    while((c=fgetc(stdin))) {
        if(ccnt==2) printf("  ");
        if(c=='.') break;
        if(c=='r') { load(); printf("R0K\n%03d _ ",++pos); continue; }
        else {
            if((ccnt==3)&&(isalpha(c))) punchp(cel['0']);
            punchp(cel[toupper(c)]);
        }
        if((ccnt>2)&&(isalpha(c))) { 
            if((c!='S')&&(c!='s')) { printf("\n%03d _ ",++pos); ccnt=0; }
            else sfl=1;
        }
        else if(sfl==1) { printf("\n%03d _ ",++pos); ccnt=0; sfl=0; }
        ccnt++;
    }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    fclose(fo);
    return 0;
}
