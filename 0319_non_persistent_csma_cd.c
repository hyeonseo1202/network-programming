#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU 8086

typedef double real;
#define then

#define A 16807L
#define M 2147483647L

#define NUM_NODES 50
#define FRAME_LEN 20
#define NUM_SLOTS 10000000L
#define INF 2147483647L

/* =========================
   Provided random generator
   ========================= */
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

/* =========================
   geometric(p): 1,2,3,...
   ========================= */
long geometric_wait(double p)
{
    double u = ranf();

    /* p=1은 이번 과제에서 제외 */
    if (p >= 1.0) return 1;

    return (long)(log(1.0 - u) / log(1.0 - p)) + 1;
}

/* =========================
   non-persistent CSMA/CD
   ========================= */
double simulate_nonpersistent(double p)
{
    int backlogged[NUM_NODES];      /* 0: idle, 1: backlogged */
    long next_sense[NUM_NODES];     /* 다시 채널 감지할 슬롯 */
    long attempt_slot[NUM_NODES];   /* 실제 전송 시작할 슬롯 */

    int i;
    long t;

    int transmitting_node = -1;
    int remaining_tx_slots = 0;

    long successful_busy_slots = 0;

    for (i = 0; i < NUM_NODES; i++) {
        backlogged[i] = 0;
        next_sense[i] = INF;
        attempt_slot[i] = INF;
    }

    for (t = 0; t < NUM_SLOTS; t++) {
        int channel_idle = 1;
        int collision_happened = 0;

        int starters_count = 0;
        int first_starter = -1;

        /* -------------------------
           1) 현재 슬롯의 채널 상태 결정
           ------------------------- */
        if (remaining_tx_slots > 0) {
            channel_idle = 0;
        } else {
            for (i = 0; i < NUM_NODES; i++) {
                if (attempt_slot[i] == t) {
                    starters_count++;
                    if (first_starter == -1) {
                        first_starter = i;
                    }
                }
            }

            if (starters_count == 1) {
                /* 성공적으로 새 전송 시작 */
                transmitting_node = first_starter;
                remaining_tx_slots = FRAME_LEN;
                attempt_slot[first_starter] = INF;
                channel_idle = 0;
            } else if (starters_count >= 2) {
                /* 같은 슬롯에 둘 이상 시작 -> 충돌 */
                collision_happened = 1;
                channel_idle = 0;
            }
        }

        /* -------------------------
           2) idle 노드의 새 프레임 도착
              슬롯 t 동안 도착 -> t+1에 감지 시작
           ------------------------- */
        for (i = 0; i < NUM_NODES; i++) {
            if (backlogged[i] == 0 && i != transmitting_node) {
                if (ranf() < p) {
                    backlogged[i] = 1;
                    next_sense[i] = t + 1;
                }
            }
        }

        /* -------------------------
           3) backlogged 노드의 채널 감지
              슬롯 t를 감지해서 다음 행동 결정
           ------------------------- */
        for (i = 0; i < NUM_NODES; i++) {
            if (backlogged[i] == 1 && i != transmitting_node && attempt_slot[i] != t) {
                if (next_sense[i] == t) {
                    if (channel_idle) {
                        /* 채널이 idle이면 다음 슬롯에 전송 시작 */
                        attempt_slot[i] = t + 1;
                        next_sense[i] = INF;
                    } else {
                        /* non-persistent: geometric(p) 후 다시 감지 */
                        next_sense[i] = t + geometric_wait(p);
                    }
                }
            }
        }

        /* -------------------------
           4) 충돌 처리
           ------------------------- */
        if (collision_happened) {
            for (i = 0; i < NUM_NODES; i++) {
                if (attempt_slot[i] == t) {
                    attempt_slot[i] = INF;
                    next_sense[i] = t + geometric_wait(p);
                }
            }
        }

        /* -------------------------
           5) 성공 전송 슬롯 누적 및 종료 처리
           ------------------------- */
        if (remaining_tx_slots > 0) {
            successful_busy_slots++;
            remaining_tx_slots--;

            if (remaining_tx_slots == 0) {
                backlogged[transmitting_node] = 0;
                next_sense[transmitting_node] = INF;
                attempt_slot[transmitting_node] = INF;
                transmitting_node = -1;
            }
        }
    }

    return (double)successful_busy_slots / (double)NUM_SLOTS;
}

int main(void)
{
    double logp;
    double p;
    double utilization;

    FILE *fp = fopen("result.csv", "w");
    if (fp == NULL) {
        printf("result.csv 파일을 열 수 없습니다.\n");
        return 1;
    }

    fprintf(fp, "log10_p,p,utilization\n");

    printf("Non-persistent CSMA/CD Simulation Start\n");
    printf("NUM_NODES = %d, FRAME_LEN = %d, NUM_SLOTS = %ld\n\n",
           NUM_NODES, FRAME_LEN, NUM_SLOTS);

    /* log10(p) = -2.0, -1.8, ..., -0.2 */
    for (logp = -2.0; logp <= -0.2 + 1e-9; logp += 0.2) {
        p = pow(10.0, logp);

        printf("Running for log10(p)=%.1f, p=%.8f ...\n", logp, p);

        utilization = simulate_nonpersistent(p);

        printf(" -> utilization = %.8f\n\n", utilization);
        fprintf(fp, "%.1f,%.8f,%.8f\n", logp, p, utilization);
    }

    fclose(fp);

    printf("Simulation finished.\n");
    printf("Result saved to result.csv\n");

    return 0;
}