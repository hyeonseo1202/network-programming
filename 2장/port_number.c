#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>


#define MSG "Test Message"

int main() {
	int sd1, sd2;
	int addrlen;
	struct sockaddr_in servaddr, cliaddr;
	unsigned short port1, port2;

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(9000);
	sd1=socket(PF_INET,SOCK_STREAM,0);
	sd2=socket(PF_INET,SOCK_DGRAM,0);
	if(connect(sd1, (struct sockaddr*)&servaddr, sizeof(servaddr))<0) {
			perror("connect fail");
			exit(1);
	}
	getsockname(sd1, (struct sockaddr*)&cliaddr, &addrlen);
	port1 = ntohs(cliaddr.sin_port);
	sendto(sd2, MSG, strlen(MSG), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	getsockname(sd2, (struct sockaddr*)&servaddr, &addrlen);
	getsockname(sd2, (struct sockaddr*)&cliaddr, &addrlen);
	port2 = ntohs(cliaddr.sin_port);

	printf("stream 		socket port number = %d\n", port1);
	printf("datagram	socket port number = %d\n", port2);
	close(sd1);
	close(sd2);
	return 0;
}
