#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define WSIZE 48
#define MSIZE 4096
typedef signed char word;

word ADDR[WSIZE];
word A[WSIZE];
word Q[WSIZE];

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
    else if(t==-2) {
        t=0; *c=1;
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
    
    return r;
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

    mv(ta2,nbc(a2));

    for(int i=WSIZE-1;i>=0;i--)
        r[i]=anb(a1[i],ta2[i],c);

    return r;
}

void roll(word *v, word *w)
{
    word t=v[0];
    for(int i=WSIZE-1,j=WSIZE-2;i>0;i--,j--) v[j]=v[i];
    v[WSIZE-1]=w[0];
    for(int i=WSIZE-1,j=WSIZE-2;i>0;i--,j--) w[j]=w[i];
    w[WSIZE-1]=t;
}

void rolr(word *v, word *w)
{
    word t=v[WSIZE-1];
    for(int i=WSIZE-2,j=WSIZE-1;i>=2;i--,j--) v[j]=v[i];
    v[0]=0;
    for(int i=WSIZE-2,j=WSIZE-1;i>=2;i--,j--) w[j]=w[i];
    w[0]=t;
}

void shll(word *v, word *w)
{
    for(int i=WSIZE-1,j=WSIZE-2;i>0;i--,j--) v[j]=v[i];
    v[WSIZE-1]=w[0];
    for(int i=WSIZE-1,j=WSIZE-2;i>0;i--,j--) w[j]=w[i];
}

void shlr(word *v, word *w)
{
    for(int i=WSIZE-2,j=WSIZE-1;i>=2;i--,j--) v[j]=v[i];
    v[0]=0;
    for(int i=WSIZE-2,j=WSIZE-1;i>=2;i--,j--) w[j]=w[i];
    w[0]=t;
}

char *tostr(word *s)
{
    char *r;
    r=calloc(WSIZE,sizeof(char));
    for(int i=0;i<WSIZE;i++) 
        r[i]=(char)s[i]+48;
    return r;
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
    mv(v3,nbsub(v1,v2,&c));
    printf("%s\n",tostr(v3));
    printf("%ld\n",nb2dc(v3));
    return 0;
}