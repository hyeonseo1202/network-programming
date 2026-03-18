#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
    real Pa = 0.1;     // 도착 확률
    real Ps = 0.1;     // 서비스 확률
    int Te = 200;      // 총 시뮬레이션 시간

    int time = 0;
    int t1 = 0;        // 다음 arrival 시각
    int t2 = Te + 1;   // 다음 completion 시각 (없으면 매우 큰 값)

    int next_id = 1;           // 다음 고객 번호
    int current_customer = -1; // 현재 서비스 중인 고객
    int server_busy = 0;       // 0: idle, 1: busy

    int queue[10000];
    int front = 0, rear = 0;

    while (time < Te) {

        if (t1 < t2) {
            /* event 1: arrival */
            int cid;

            time = t1;
            if (time >= Te) break;

            cid = next_id++;
            printf("at time %d, customer %d arrive\n", time, cid);

            /* 다음 도착 예약 */
            t1 = time + geometric(Pa);

            /* event 2: request server */
            if (!server_busy) {
                server_busy = 1;
                current_customer = cid;
                printf("at time %d, customer %d begin\n", time, current_customer);
                t2 = time + geometric(Ps);
            } else {
                queue[rear++] = cid;
            }
        }
        else {
            /* event 3: completion */
            time = t2;
            if (time >= Te) break;

            printf("at time %d, customer %d end\n", time, current_customer);

            if (front < rear) {
                current_customer = queue[front++];
                printf("at time %d, customer %d begin\n", time, current_customer);
                t2 = time + geometric(Ps);
            } else {
                server_busy = 0;
                current_customer = -1;
                t2 = Te + 1;
            }
        }
    }

    return 0;
}