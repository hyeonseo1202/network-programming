/**
 * M/M/1 대기열 시뮬레이션
 * 이 코드는 고객이 하나씩 도착하고, 서버가 한 명씩 처리하는 시스템을 시뮬레이션 함. 
 * 고객 도착 간격과 서비스 시간 모두 지수분포 이용
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU 8086
typedef double real;
#define then

#define A 16807L
#define M 2147483647L
#define INF 1.0e30

static long In[16] = {
    0L,
    1973272912L, 747177549L,  20464843L,  640830765L, 1098742207L,
      78126602L,  84743774L, 831312807L,  124667236L, 1172177002L,
    1124933064L, 1223960546L, 1878892440L, 1449793615L, 553303732L
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

real exponential(real lambda)
{
    return -log(1.0 - ranf()) / lambda;
}

int main()
{
    double Ta = 0.1, Ts = 0.1, te = 10000000.0;
    double B; // 서버가 바빳던 총 시간
    double C; //완료된 고객 수
    double L; //평균 시스템 내 고객 수
    double s; //시간에 따른 시스템 내 고객 수의 누적 면적
    double tb; //현재 Busy Period가 시작된 시각
    double tn; //직전 이벤트가 발생한 시간
    double U; //Utilization
    double W; //평균 체류 시간
    double X; //throughput
    double t1, t2, time, next_time;
    int n; //현재 시스템 안에 있는 고객 수

    n = 0;
    time = 0.0; //현재 시뮬레이션 시각
    tn = 0.0;
    s = 0.0;
    B = 0.0;
    C = 0.0;

    t1 = exponential(Ta);   // 다음 도착 서비스가 일어날 시각
    t2 = INF;               // 다음 서비스 완료 이벤트가 일어날 시각, 현재는 아직 서비스 도착 전이라 INF

    while (1) {
        if (t1 < t2) //다음 도착이 더 빠르면 arrival 처리
            next_time = t1;
        else //다음 완료가 더 빠르면 departure처리
            next_time = t2;

        if (next_time > te) { //시뮬레이션 종료 처리
            s += n * (te - tn); //직전 이벤트 시각부터 종료 시각 te까지 고객수 n이 유지되었으니 그만큼 면적 누적함. -> 평균 고객 수 L 계산에 필요
            if (n > 0) B += te - tb; //종료 시점에도 서버가 바쁘면, 그 시간도B에 포함
            time = te;
            break;
        }

        s += n * (next_time - tn);
        tn = next_time;
        time = next_time;

        if (t1 < t2) {   // 도착 이벤트 처리
            n++;
            t1 = time + exponential(Ta); //도착했으니, 다음 도착시간 새로 예약

            if (n == 1) { //도착 후 n이 1이면 도착 전엔 n이 0이었음. 원래 시스템이 비어있었으니 바로 서비스 시작 가능
                tb = time; //서버가 바쁜 상태가 된 시각 기록
                t2 = time + exponential(Ts); //방금 서비스가 시작됐으니, 이 서비스의 완료 시점 예약
            }
        } else {         // departure
            n--; //시스템 내 고객 수 감소
            C++; // 완료된 고객 수 증가
 
            if (n > 0) { //한 명 나갔는데도 아직 시스템에 고객이 남아있다면, 다음 고객이 바로 서비스 시작 가능
                t2 = time + exponential(Ts); //다음 departure 예약 가능
            } else {
                B += time - tb; //Busy time에 계산한 시각 저장
                t2 = INF; // 비어있으니까 INF로!
            }
        }
    }

    X = C / time;
    U = B / time;
    L = s / time;
    W = L / X;

    printf("throughput = %f\n", X);
    printf("utilization = %f\n", U);
    printf("mean no. in system = %f\n", L);
    printf("mean residence time = %f\n", W);

    return 0;
}