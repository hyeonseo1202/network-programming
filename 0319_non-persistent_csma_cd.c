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


int main() {
    int t = 1000000;
    double total = 0;

    for (int i = 0; i < t; i++) {
        int count = 0;
        int state = 1;

        while (state != 0) {
            switch (state) {

                case 1:
                    count++;
                    state = 2;
                    break;

                case 2: {
                    count++;
                    double r = ranf();
                    if (r < 0.2)
                        state = 3;
                    else
                        state = 5;
                    break;
                }

                case 3:
                    count++;
                    state = 4;
                    break;

                case 4: {
                    count++;
                    double r = ranf();
                    if (r < 0.25)
                        state = 3;  // 반복
                    else
                        state = 7;
                    break;
                }

                case 5: {
                    count++;
                    double r = ranf();
                    if (r < 0.45)
                        state = 6;
                    else
                        state = 7;
                    break;
                }

                case 6:
                    count++;
                    state = 7;
                    break;

                case 7:
                    count++;
                    state = 8;
                    break;

                case 8: {
                    count++;
                    double r = ranf();
                    if (r < 0.3)
                        state = 1;  // 처음으로 돌아감
                    else
                        state = 0;  // 종료
                    break;
                }

                default:
                    state = 0;
                    break;
            }
        }

        total += count;
    }

    printf("Average = %f\n", total / t);
    return 0;
}