#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int s, nbyte;
    char buf[MAXLINE+1];

    if(argc != 3) {
        printf("usage: %s ip_address port_number\n", argv[0]);
        exit(0);
    }

    /* 1) UDP 소켓 개설 */
    if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }
    printf("1. socket() success\n");

    /* 2) 서버 소켓주소 구조체 작성 */
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));
    printf("2. server address setting success\n");

    /* 3) 서버 소켓주소를 인자로 하여 connect() 호출 */
    if(connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect fail");
        exit(0);
    }
    printf("3. connect() success\n");

    printf("input: ");
    if(fgets(buf, MAXLINE, stdin) == NULL) {
        printf("fgets fail\n");
        exit(0);
    }

    /* 4) write() 또는 send()로 에코 메시지 전달 */
    if(write(s, buf, strlen(buf)) < 0) {
        perror("write fail");
        exit(0);
    }
    printf("4. write() success\n");

    /* 5) read() 또는 recv()로 에코 메시지 수신 */
    if((nbyte = read(s, buf, MAXLINE)) < 0) {
        perror("read fail");
        exit(0);
    }
    printf("5. read() success\n");

    buf[nbyte] = 0;
    printf("receive: %s\n", buf);

    /* 6) 연결된 UDP 종료 */
    printf("6. connected UDP finished\n");

    /* 7) close()로 UDP 소켓 닫기 */
    close(s);
    printf("7. close() success\n");

    return 0;
}
hyeonseo@ub