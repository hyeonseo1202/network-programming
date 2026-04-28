#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

char *EXIT_STRING = "exit";   // 종료 문자열
#define MAXLINE 511

int recv_and_print(int sd);       // 상대 메시지 수신 후 화면 출력
int input_and_send(int sd);       // 키보드 입력 후 상대에게 메시지 전달
int send_time_periodically(int sd); // 10초마다 현재 시간 전송

int main(int argc, char *argv[]) {
    struct sockaddr_in cliaddr, servaddr;
    int listen_sock, accp_sock;
    int addrlen = sizeof(cliaddr);
    pid_t pid1, pid2;

    if (argc != 2) {
        printf("사용법 : %s port\n", argv[0]);
        exit(0);
    }

    /* 소켓 생성 */
    if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    /* 서버 주소 구조체 초기화 */
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    /* bind */
    if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind fail");
        exit(0);
    }

    puts("서버가 클라이언트를 기다리고 있습니다.");
    listen(listen_sock, 1);

    /* accept */
    if ((accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen)) < 0) {
        perror("accept fail");
        exit(0);
    }

    puts("클라이언트가 연결되었습니다.");

    /* 첫 번째 자식: 클라이언트 메시지 수신 후 화면 출력 */
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork fail");
        close(accp_sock);
        close(listen_sock);
        exit(0);
    }

    if (pid1 == 0) {
        recv_and_print(accp_sock);
        close(accp_sock);
        exit(0);
    }

    /* 두 번째 자식: 10초마다 현재 시간 전송 */
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork fail");
        close(accp_sock);
        close(listen_sock);
        exit(0);
    }

    if (pid2 == 0) {
        send_time_periodically(accp_sock);
        close(accp_sock);
        exit(0);
    }

    /* 부모 프로세스: 키보드 입력 받아 클라이언트에게 전송 */
    input_and_send(accp_sock);

    close(accp_sock);
    close(listen_sock);
    return 0;
}

/* 키보드 입력받고 상대에게 메시지 전달 */
int input_and_send(int sd) {
    char buf[MAXLINE + 1];
    int nbyte;

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        nbyte = strlen(buf);
        write(sd, buf, nbyte);

        /* 종료 문자열 입력 시 종료 */
        if (strstr(buf, EXIT_STRING) != NULL) {
            puts("Good bye.");
            close(sd);
            exit(0);
        }
    }
    return 0;
}

/* 상대로부터 메시지 수신 후 화면 출력 */
int recv_and_print(int sd) {
    char buf[MAXLINE + 1];
    int nbyte;

    while (1) {
        if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
            perror("read fail");
            close(sd);
            exit(0);
        }

        if (nbyte == 0) {
            break;
        }

        buf[nbyte] = 0;

        /* 종료 문자열 수신 시 종료 */
        if (strstr(buf, EXIT_STRING) != NULL) {
            break;
        }

        printf("%s", buf);
    }
    return 0;
}

/* 10초마다 현재 시간을 클라이언트에게 전송 */
int send_time_periodically(int sd) {
    char buf[MAXLINE + 1];
    time_t tim;

    while (1) {
        tim = time(NULL);
        snprintf(buf, sizeof(buf), "[현재 시각] %s", ctime(&tim));
        write(sd, buf, strlen(buf));

        sleep(10);
    }
    return 0;
}
