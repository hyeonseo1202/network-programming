#include <stdio.h>
#include <math.h>

#define CPU 8086
typedef double real;
#define then

#define A 16807L
#define M 2147483647L

static long In[16] = {
    0L,
    1973272912L, 747177549L,  20464843L,  640830765L, 1098742207L,
      78126602L,  84743774L, 831312807L,  124667236L, 1172177002L,
    1124933064L, 1223960546L, 1878892440L, 1449793615L, 553303732L
};

static long InitIn[16] = {
    0L,
    1973272912L, 747177549L,  20464843L,  640830765L, 1098742207L,
      78126602L,  84743774L, 831312807L,  124667236L, 1172177002L,
    1124933064L, 1223960546L, 1878892440L, 1449793615L, 553303732L
};

static int strm = 1;

void reset_rng(void)
{
    int i;
    for (i = 0; i < 16; i++) In[i] = InitIn[i];
    strm = 1;
}

#if CPU==8086
real ranf(void)
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

int geometric(real p)
{
    return (int)(log(1.0 - ranf()) / log(1.0 - p)) + 1;
}

void simulate_stats(double Pa, double Ps, int Te)
{
    int n = 0;
    int time = 0;
    int t1 = 0;
    int t2 = Te + 1;

    long long C = 0;          
    double area_n = 0.0;       
    double busy = 0.0;         
    int tn = 0;                

    while (time < Te) {
        if (t1 < t2) {
            time = t1;
            if (time > Te) time = Te;

            area_n += (double)n * (time - tn);
            if (n > 0) busy += (double)(time - tn);
            tn = time;

            if (time >= Te) break;

            n++;

            t1 = time + geometric(Pa);

            if (n == 1) {
                t2 = time + geometric(Ps);
            }
        }
        else {
            time = t2;
            if (time > Te) time = Te;

            area_n += (double)n * (time - tn);
            if (n > 0) busy += (double)(time - tn);
            tn = time;

            if (time >= Te) break;

            n--;
            C++;

            if (n > 0) {
                t2 = time + geometric(Ps);
            }
            else {
                t2 = Te + 1;
            }
        }
    }

    {
        double X = (double)C / (double)Te;      
        double U = busy / (double)Te;           
        double L = area_n / (double)Te;         
        double W = (X > 0.0) ? (L / X) : 0.0;   

        printf("te = %d 인 경우에 (Ta = %.1f, Ts = %.1f)\n", Te, Pa, Ps);
        printf("throughput = %.6f\n", X);
        printf("utilization = %.6f\n", U);
        printf("mean no. in system = %.6f\n", L);
        printf("mean residence time = %.6f\n", W);
    }
}

int main(void)
{
    double Ta = 0.1;       
    double Ts = 0.1;     

    reset_rng();
    simulate_stats(Ta, Ts, 10000000);

    return 0;
}