#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define WSIZE 48
#define MSIZE 4096
typedef signed char word;

word ADDR[WSIZE];

word *dc2nb(long value)
{
    word *rv;
    int i=WSIZE-1;
    int rem=0;
    rv=calloc(WSIZE,sizeof(word));

    while (value != 0)
	{
		rem = value % -2;
		value = value / -2;

		if (rem < 0)
		{
			rem += 2;
			value += 1;
		}
        rv[i--]=rem;
    }

    return rv;
}

long ipw2(int n)
{  
    long r=1;
    long b=-2;
    for(int i=0;i<n;i++) 
        r*=b;
    return r;
}

long nb2dc(word* v)
{   
    long r=0;
    for(int i=WSIZE-1,j=0;i>=0;i--,j++)
        if(v[i]) r+=(long)v[i]*ipw2(j);
    return r;
}

word anb(word a, word b, word *c)
{
    word t = a+b+*c;
    if(t==3) { 
        t=1; *c=-1; 
    }
    else if(t==2) { 
        t=0; *c=-1; 
    }
    else if(t==-1) { 
        t=1; *c=1; 
    }
    else *c=0;
    return t;
}

word *nbc(word *v)
{
    word *r;
    r=calloc(WSIZE,sizeof(word));

    for(int i=0;i<WSIZE;i++)
        r[i]=-1*v[i];
}

word *nbadd(word *a1, word *a2, word *c)
{
    word *r;
    r=calloc(WSIZE,sizeof(word));

    for(int i=WSIZE-1;i>=0;i--)
        r[i]=anb(a1[i],a2[i],c);

    return r;
}

word *nbsub(word *a1, word *a2, word *c)
{
    word *r,*ta2;
    r=calloc(WSIZE,sizeof(word));
    ta2=calloc(WSIZE,sizeof(word));

    ta2=nbc(a2);

    for(int i=WSIZE-1;i>=0;i--)
        r[i]=anb(a1[i],ta2[i],c);

    return r;
}

char *tostr(word *s)
{
    char *r;
    r=calloc(WSIZE,sizeof(char));
    for(int i=0;i<WSIZE;i++) 
        r[i]=(char)s[i]+48;
    return r;
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

int main(int n, char **a)
{
    word c=0;
    word *v1, *v2, *v3;
    v1=calloc(WSIZE,sizeof(word));
    v2=calloc(WSIZE,sizeof(word));
    v3=calloc(WSIZE,sizeof(word));
    mv(v1,dc2nb(254));
    mv(v2,dc2nb(129));
    mv(v3,nbadd(v1,v2,&c));
    printf("%s\n",tostr(v3));
    printf("%s\n",tostr(dc2nb(-267)));
    printf("%ld\n",nb2dc(dc2nb(-267)));
    return 0;
}