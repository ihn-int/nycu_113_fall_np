#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int xchg_data(FILE* fp, int sockfd) {
    char sendline[MAXLINE], recvline[MAXLINE];
    struct in_addr ip_addr;
    struct sockaddr_in ss;
    socklen_t sock_len;
    int n = 0;
    int stopflag = 0, okflag = 0;

    char hostname[MAXLINE], str[INET6_ADDRSTRLEN];
    char** pptr;
    struct hostent *hptr;

    sock_len = sizeof(ss);
    getsockname(sockfd, (SA*)&ss, &sock_len);
    ip_addr = ss.sin_addr;
    sprintf(sendline, "%s %s\n", "111550001", inet_ntoa(ip_addr));
    printf("sent: %s %s\n", "111550001", inet_ntoa(ip_addr));

    Writen(sockfd, sendline, strlen(sendline));

    while (1) {
        // recieve hostname
        if ((n = Read(sockfd, recvline, MAXLINE)) == 0) {
            err_quit("str_cli: server terminated prematurely.");
        }
        recvline[n] = '\0';
        printf("recv: %s\n", recvline);
        if ((hptr = gethostbyname(recvline)) == NULL) {
            err_msg("gethostbyname error for host: %s", recvline);
        }

        // send IP
        sprintf(sendline, "%s", inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));\

        Writen(sockfd, sendline, strlen(sendline));
        printf("sent: %s\n", sendline);
        //printf("test: %s\n", hptr->h_name);

        // recieve message
        if ((n = Read(sockfd, recvline, MAXLINE)) == 0) {
            err_quit("str_cli: server terminated prematurely.");
        }
        recvline[n] = '\0';
        printf("recv: %s\n", recvline);
        if (strncmp(recvline, "good", 4) == 0) {
            // send ID and IP
            sprintf(sendline, "%s %s\n", "111550001", inet_ntoa(ip_addr));
            printf("sent: %s %s\n", "111550001", inet_ntoa(ip_addr));

            Writen(sockfd, sendline, strlen(sendline));

            // recieve ack
            if ((n = Read(sockfd, recvline, MAXLINE)) == 0) {
                err_quit("str_cli: server terminated prematurely.");
            }
            recvline[n] = '\0';
            printf("recv: %s\n", recvline);
            if (strncmp(recvline, "ok", 2) == 0) {
                // ok
                return 0;
            }
            else if (strncmp(recvline, "nak", 3) == 0) {
                // nak
                return 2;
            }

        }
        if (strncmp(recvline, "bad", 3) == 0) {
            // bad
            return 1;
        }
        //exit(0);
    }
}

int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) err_quit("usage: ass3cli001 <IPaddress>");

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT + 2);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA*) &servaddr, sizeof(servaddr));

    int res = xchg_data(stdin, sockfd);
    if (res == 0) {
        printf("pass\n");
    }
    else if (res == 1) {
        printf("something wrong in gethostbyname\n");
    }
    else if (res == 2) {
        printf("something wrong in sent ID and IP again\n");
    }
    exit(0);
}
