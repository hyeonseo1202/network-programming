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



int main()
{
    int win = 0;
    int lose = 0;
    int draw = 0;
   long trials = 10000000;

    for (long i = 0; i < trials; i++) {
        real r = ranf();

        if (r < 1.0/3.0) {
            lose++;
        }
        else if (r < 2.0/3.0) {
            win++;
        }
        else {
            draw++;
        }
    }

    printf("총 시행 횟수: %ld\n\n", trials);

    printf("진 것 확률  : %.6f\n", (double)lose / trials);
    printf("이긴 것 확률: %.6f\n", (double)win / trials);
    printf("비긴 것 확률: %.6f\n", (double)draw / trials);

    return 0;
}