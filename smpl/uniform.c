#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU 8086

typedef double real;
#define then

#define A 16807L
#define M 2147483647L

static long In[16] = {0L,
    1973272912L, 747177549L,  20464843L,  640830765L, 1098742207L,
      78126602L,  84743774L, 831312807L,  124667236L, 1172177002L,
    1124933064L, 1223960546L,1878892440L, 1449793615L,  553303732L
};

static int strm = 1;


#if CPU==8086
real ranf()
{
    short *p, *q, k;
    long Hi, Lo;

    p  = (short *)&In[strm];
    Hi = *(p+1) * A;
    *(p+1) = 0;
    Lo = In[strm] * A;

    p  = (short *)&Lo;
    Hi += *(p+1);

    q  = (short *)&Hi;
    *(p+1) = *q & 0x7FFF;

    k = *(q+1) << 1;
    if (*q & 0x8000) then k++;

    Lo -= M;
    Lo += k;
    if (Lo < 0) then Lo += M;

    In[strm] = Lo;
    return ((real)Lo * 4.656612875E-10);
}
#endif

real uniform(real a, real b)
{
    return a + (b - a) * ranf();
}



int main()
{
    int i;


    printf("\n<0.9 ~ 1.5 uniform>\n");
    for (i = 0; i < 10; i++) {
        printf("%f / ", uniform(0.9,1.5));
    }
    printf("\n");

    return 0;
}