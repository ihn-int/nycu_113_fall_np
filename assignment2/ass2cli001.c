#include	"unp.h"
#include <stdio.h>
#include <stdlib.h>

int gcd(int a, int b) {
		if (b == 0) return a;
		return gcd(b, a%b);
}

int xchg_data(FILE* fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];
	//int a, b;
    struct in_addr ip_addr;
    struct sockaddr_in ss;
    socklen_t sock_len;
    fd_set rset;
    int maxfdp1;
    int count = 0;
    int stopflag = 0;
    int okflag = 0;
    int n;
    
    sock_len = sizeof(ss);
    getsockname(sockfd, (SA*)&ss, &sock_len);
    ip_addr = ss.sin_addr;
    sprintf(sendline, "%s %s\n", "111550001", inet_ntoa(ip_addr));
    printf("sent: %s %s\n", "111550001", inet_ntoa(ip_addr));
    
	Writen(sockfd, sendline, strlen(sendline));
	
	while (1) {
	    FD_SET(fileno(fp), &rset);
	    FD_SET(sockfd, &rset);
	    maxfdp1 = max(fileno(fp), sockfd) + 1;
	    
	    Select(maxfdp1, &rset, NULL, NULL, NULL);
	    
	    // socket
	    if (FD_ISSET(sockfd, &rset)) {
	        if ((n = Read(sockfd, recvline, MAXLINE)) == 0)
	            err_quit("str_cli: server terminated prematurely");
	            
	        if (strncmp(recvline, "bad", 3) == 0) {
	            count--;
	            printf("recv: bad\nsomething wrong! try again.\n");
	        }
	        if (strncmp(recvline, "stop", 4) == 0) {
    	        stopflag = 1;
    	        printf("recv: stop\n");
    	        sprintf(sendline, "%i %s\n", count, inet_ntoa(ip_addr));
	            Writen(sockfd, sendline, strlen(sendline));
	            printf("sent line count & IP address: %i %s\n", count, inet_ntoa(ip_addr));
    	    }
    	    if (strncmp(recvline, "ok", 2) == 0) {
    	        okflag = 1;
    	        printf("recv: ok\n");
    	        return 0;
    	    }
    	    if (strncmp(recvline, "nak", 3) == 0) {
        	    printf("revc: nak\n");
    	        return 2;
    	    }
	    }
	    
	    // stdin
	    if (FD_ISSET(fileno(fp), &rset)) {
	    
	        // check stop
	        if (stopflag) {
	            
	            stopflag = 0;
	        }
	        else if (okflag) {
	            //Shutdown(sockfd, SHUT_WR);	/* send FIN */
	            return 0;
	        }
	    
	        else if ((n = Read(fileno(fp), sendline, MAXLINE)) == 0) {
	            return 1;
	        }
	        else {
	            Writen(sockfd, sendline, n);
	            count++;
	            //printf("send: %scount: %i\n", sendline, count);
	        }
	    }
	}
	/*
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
	*/
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
	servaddr.sin_port = htons(SERV_PORT + 1);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	
	int res = xchg_data(stdin, sockfd);		/* do it all */
	if (res == 1) printf("user end the connect.\n");
	else if (res == 0) printf("success\n");
	exit(0);
}
