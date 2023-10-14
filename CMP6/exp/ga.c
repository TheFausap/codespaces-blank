#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define SOLN 9000000
#define RNDMAX 100

typedef struct _sol sol;

typedef struct _sol
{
    long double rank, x, y, z;
    void (*fitness)(sol *);
} sol;

sol solutions[SOLN];

long double drand (double low, double high)
{
    
    return ( (long double)rand() * ( high - low ) ) / (long double)RAND_MAX + low;
}

void fits(sol *s)
{
    long double ans = (6 * s->x + -s->y + powl(s->z,200)) - 25;
    s->rank = (ans==0) ? 9999 : fabsl(1.0L/ans);
}

int cmp(const void *a, const void *b)
{
    return ((sol*)a)->rank < ((sol*)b)->rank;
}

int main(int n, char **a)
{
    srand((unsigned int)clock());
    
    for(int i=0;i<SOLN;i++) {
        solutions[i].rank = 0;
        solutions[i].x=drand(-RNDMAX,RNDMAX);
        solutions[i].y=drand(-RNDMAX,RNDMAX);
        solutions[i].z=drand(-RNDMAX,RNDMAX);
        solutions[i].fitness=&fits;
    }

    for(int i=0;i<SOLN;i++) solutions[i].fitness(&solutions[i]);

    qsort(solutions,SOLN,sizeof(sol),&cmp);

    for(int i=0;i<10;i++) printf("Rank %Lf\nx: %Lf, y: %Lf, z: %Lf\n", 
        solutions[i].rank, solutions[i].x, solutions[i].y, solutions[i].z);

    return 0;
}