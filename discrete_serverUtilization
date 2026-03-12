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

int geometric(real p)
{
    return (int)(log(1.0 - ranf()) / log(1.0 - p)) + 1;
}

int main()
{
    double Pa = 0.1;   // 도착 확률
    double Ps = 0.1;   // 서비스 확률
    int Te = 200;      // 총 시뮬레이션 시간

    int n;             // 시스템 내 손님 수
    int time;
    int t1;            // 다음 도착 시간
    int t2;            // 다음 서비스 완료 시간

    int next_id = 1;           // 다음 도착 손님 번호
    int current_customer = -1; // 현재 서비스 중인 손님 번호

    /* 간단한 큐 */
    int queue[10000];
    int front = 0, rear = 0;

    n = 0;
    t1 = 0;
    t2 = Te;
    time = 0;

    while (time < Te) {

        if (t1 < t2) { //다음 도착 시간보다 다음 서비스 완료 시간이 크면 도착이 먼저니까 도착 출력
            /* arrival event */
            int cid;

            time = t1;
            cid = next_id++;
            n++;

            printf("at time %d, customer %d arrive\n", time, cid);

            /* 큐에 저장 */
            queue[rear++] = cid; // 바로 시작 못하니까 큐에 저장

            /* 다음 도착 예약 */
            t1 = time + geometric(Pa);

            /* 시스템이 비어 있었다면 즉시 서비스 시작 */
            if (n == 1) {
                current_customer = queue[front];
                printf("at time %d, customer %d begin\n", time, current_customer);
                t2 = time + geometric(Ps);
            }
        }
        else { //(t2 > t1) 도착보다 서비스 완료가 먼저! -> 현재 서비스 끝
            /* completion event */
            time = t2;

            printf("at time %d, customer %d end\n", time, current_customer); // 완료 먼저 출력 후

            /* 현재 손님 제거 */
            front++;
            n--;

            if (n > 0) { //다음 손님이 존재하면 바로 시작
                current_customer = queue[front];
                printf("at time %d, customer %d begin\n", time, current_customer);
                t2 = time + geometric(Ps); // 서비스 종료 시간 새로 제작
            }
            else { //더이상 고객이 없음
                current_customer = -1;
                t2 = Te; //서비스 종료시간을  max시간으로 만들어서 다음 t1,t2비교에서 무조건 t1이 작도록 만듦
            }
        }
    }

    return 0;
}