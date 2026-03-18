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
    int time = 0; //현재 시간
    int t1 = 0; //첫 고객은 즉시 도착
    int t2 = Te + 1;

    long long C = 0;    //지금까지 몇 명 완료되었는가 
    double area_n = 0.0;    //시간 평균 고객수 몇이었는가   
    double busy = 0.0;    //서버가 얼마나 바빴는가
    int tn = 0;    //직전 이벤트 시각           

    while (time < Te) {
        if (t1 < t2) {
            time = t1; //도착 발생
            if (time > Te) time = Te; //다음 이벤트가 종료 시각 이후라면, 그냥 Te까지만 반영하기 위한 안전장치

            area_n += (double)n * (time - tn); //고객 수 면적 누적
            if (n > 0) busy += (double)(time - tn); //바쁜시간 누적
            tn = time;

            if (time >= Te) break; //종료조건

            n++;

            t1 = time + geometric(Pa); //다음 도착시간 예약

            if (n == 1) { //서비스가 비어있으면 서비스 시작
                t2 = time + geometric(Ps);
            }
        }
        else { //서비스 완료 처리
            time = t2;
            if (time > Te) time = Te; //서비스 완료가 종료 시각 이후라면, 그냥 Te까지만 반영하기 위한 안전장치

            area_n += (double)n * (time - tn);
            if (n > 0) busy += (double)(time - tn);
            tn = time;

            if (time >= Te) break; //종료조건

            n--; //시스템 내 고객 수 감소
            C++; //완료 고객 수 증가

            if (n > 0) { //서비스를 마친 뒤에도 고객이 남아있으면 대기중인 다음 고객이 즉시 서비스에 들어감
                t2 = time + geometric(Ps); //새 서비스 완료시간 예약
            }
            else { //고객이 아무도 없으면
                t2 = Te + 1; //서버는 idle해지므로 예정된 서비스 완료가 없어서 Te+1값을 가지게 됨.
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