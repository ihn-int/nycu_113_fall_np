#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    return;
}

int main(int argc, char** argv) {
    int listenfd, udpfd, connfd[13];
    struct sockaddr_in servaddr, cliaddr, usrvaddr, ss, peeraddr;
    struct in_addr ip;
    char sendline[MAXLINE], recvline[MAXLINE];
    socklen_t socklen, peerlen, clilen;
    int n, nchar, childpid, ran;
    
    if (argc != 2) err_quit("usage: ass4cli001 <IPaddress>");

    // TCP socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT + 3);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    
    // UDP socket
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&usrvaddr, sizeof(usrvaddr));
    usrvaddr.sin_family = AF_INET;
    usrvaddr.sin_port = htons(SERV_PORT + 3);
    Inet_pton(AF_INET, argv[1], &usrvaddr.sin_addr);
    Connect(udpfd, (SA*)&usrvaddr, sizeof(usrvaddr));
    
    // get IP
    socklen = sizeof(ss);
    getsockname(udpfd, (SA*)&ss, &socklen);
    ip = ss.sin_addr;
    
    // send 11
    sprintf(sendline, "%i %s %s", 11, "111550001", inet_ntoa(ip));
    printf("send udp: %i %s %s\n",  11, "111550001", inet_ntoa(ip));
    Write(udpfd, sendline, strlen(sendline));
    
    // get response
    nchar = Read(udpfd, recvline, MAXLINE);
    recvline[nchar] = 0;
    printf("recv udp: %s\n", recvline);
    sscanf(recvline, "%i", &n);
    if (n >= 90) {
        printf("ERROR: n = %i\n", n);
        exit(0);
    }
    bzero(recvline, MAXLINE);
    
    // send 13
    sprintf(sendline, "%i %s %i", 13, "111550001", (int)(SERV_PORT + 3));
    printf("send udp: %i %s %i\n",  13, "111550001", (int)(SERV_PORT + 3));
    Write(udpfd, sendline, strlen(sendline));
    
    // Initialize
    Signal(SIGCHLD, sig_chld);
    int i = 0;
    for (int k = 0; k < 13; k++) {
        connfd[k] = -1;
    }
    
    // concurrent TCP server
    for (;;) {
        bzero(&cliaddr, sizeof(cliaddr));
        clilen = sizeof(cliaddr);
        if ((connfd[i] = accept(listenfd, (SA*) &cliaddr, &clilen)) < 0) {
            if (errno == EINTR) {
                 continue;
            }
            else {
                err_sys("Accept error");
            }
        }
        // fork
        if ((childpid = Fork()) == 0) {
            Close(listenfd);
            printf("connection %i created.\n", i);

            // get random number 
            Readline(connfd[i], recvline, MAXLINE);
            printf("recv tcp: %s", recvline);
            sscanf(recvline, "%i", &ran);
            
            // send response
            peerlen = sizeof(peeraddr);
            getpeername(connfd[i], (SA*)&peeraddr, &peerlen);
            sprintf(sendline, "%d %i\n", ntohs(peeraddr.sin_port), ran);
            printf("send tcp: %d %i\n", ntohs(peeraddr.sin_port), ran);
            Write(connfd[i], sendline, strlen(sendline));
            
            // get response
            Readline(connfd[i], recvline, MAXLINE);
            printf("recv tcp : %s", recvline);
            printf("connection %i end.\n", i);
            Close(connfd[i]);
            if (strncmp(recvline, "ok", 2) == 0) {        
                exit(0);
            }
            else if (strncmp(recvline, "nak", 3) == 0) {
                exit(1);
            }
        }
        Close(connfd[i]);
        i++;
        if (i >= n) break;
    }
    
    // get ok from udp server
    bzero(recvline, MAXLINE);
    Read(udpfd, recvline, MAXLINE);
    if (strncmp(recvline, "ok", 2) == 0) {
        printf("Pass\n");
    }
        
    exit(0);
}
