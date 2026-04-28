#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU 8086
typedef double real;
#define then

#define A 16807L
#define M 2147483647L

#define ARRIVAL    1
#define REQUEST    2
#define COMPLETION 3

static long In[16] = {
    0L,
    1973272912L, 747177549L,  20464843L,  640830765L, 1098742207L,
      78126602L,  84743774L, 831312807L,  124667236L, 1172177002L,
    1124933064L, 1223960546L, 1878892440L, 1449793615L, 553303732L
};

static int strm = 1;

#if CPU==8086
real ranf(void)
{
    short *p, *q, k;
    long Hi, Lo;

    p  = (short *)&In[strm];
    Hi = *(p + 1) * A;
    *(p + 1) = 0;
    Lo = In[strm] * A;

    p  = (short *)&Lo;
    Hi += *(p + 1);

    q  = (short *)&Hi;
    *(p + 1) = *q & 0x7FFF;

    k = *(q + 1) << 1;
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

int main(void)
{
    real Pa = 0.1;   /* 도착 확률 */
    real Ps = 0.1;   /* 서비스 확률 */
    int Te = 200;    /* 총 시뮬레이션 시간 */

    int time = 0;

    /* 다음 이벤트 시각 */
    int t_arrival = 0;         /* 다음 arrival 시각 */
    int t_completion = Te + 1; /* 다음 completion 시각, 없으면 큰 값 */

    /* 현재 발생할 이벤트 정보 */
    int event;
    int customer;

    /* 고객 번호 */
    int next_id = 1;
    int current_customer = -1;

    /* 서버 상태 */
    int server_busy = 0;

    /* 대기 큐 */
    int queue[10000];
    int front = 0, rear = 0;

    while (time < Te) {

        /* cause(&event, &customer) 역할을 직접 구현 */
        if (t_arrival < t_completion) {
            event = ARRIVAL;
            time = t_arrival;
            customer = next_id;
        } else {
            event = COMPLETION;
            time = t_completion;
            customer = current_customer;
        }

        if (time >= Te) break;

        switch (event) {

        case ARRIVAL:
            /* 고객 도착 */
            printf("at time %d, customer %d arrive\n", time, customer);

            /* 교수님 코드의
               schedule(2, 0.0, customer)
               를 바로 REQUEST case 실행으로 표현 */
            event = REQUEST;

            /* 다음 고객 도착 예약 */
            next_id++;
            t_arrival = time + geometric(Pa);

            switch (event) {

            case REQUEST:
                /* 서버 요청 */
                if (!server_busy) {
                    server_busy = 1;
                    current_customer = customer;
                    printf("at time %d, customer %d begin\n", time, customer);

                    /* 서비스 완료 예약 */
                    t_completion = time + geometric(Ps);
                } else {
                    /* 서버가 바쁘면 큐에 대기 */
                    queue[rear++] = customer;
                }
                break;
            }

            break;

        case COMPLETION:
            /* 서비스 완료 */
            printf("at time %d, customer %d end\n", time, customer);

            if (front < rear) {
                /* 대기 중인 다음 고객이 있으면 즉시 시작 */
                current_customer = queue[front++];
                printf("at time %d, customer %d begin\n", time, current_customer);

                /* 다음 완료 예약 */
                t_completion = time + geometric(Ps);
            } else {
                /* 더 이상 대기 고객이 없으면 서버 유휴 */
                server_busy = 0;
                current_customer = -1;
                t_completion = Te + 1;
            }
            break;
        }
    }

    return 0;
}