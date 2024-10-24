#include	"unp.h"
#include <stdio.h>
#include <stdlib.h>

int gcd(int a, int b) {
		if (b == 0) return a;
		return gcd(b, a%b);
}

int xchg_data(FILE* fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];
	int a, b;

	if (Fgets(sendline, MAXLINE, fp) != NULL) {
			
		sendline[9] = '\n';
	}
	Writen(sockfd, sendline, strlen(sendline));
	
	while(1){
		printf("start\n");
		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("Error at recieving two numbers.");
		
		// recieving two numbers
		sscanf(recvline, "%d %d", &a, &b);
		printf("%s\n", recvline);
		// calculating GCD
		sprintf(sendline, "%d\n", gcd(a, b));
		printf("%s", sendline);
		Writen(sockfd, sendline, strlen(sendline));

		// recieving result
		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("Error at recieving result.");
		printf("%s", recvline);
		if (strncmp(recvline, "nak", 3) == 0) {
			continue;
		}
		if (strncmp(recvline, "wrong", 5) == 0){
		       	return 1;
		}
		if (strncmp(recvline, "ok", 2) == 0) return 0;
	}
}

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	
	int res = xchg_data(stdin, sockfd);		/* do it all */
	if (res == 1) printf("Wrong\n");
	else printf("Ok\n");
	exit(0);
}
