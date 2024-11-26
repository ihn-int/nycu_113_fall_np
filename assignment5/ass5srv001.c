#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sig_child(int signo) {
	pid_t pid;
	int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        // do nothing
    return;
}

void run_server(
    int connfd_A, int connfd_B,
    char* _id_A, char* _id_B
) {
    char sendline[MAXLINE], recvline[MAXLINE];
    char id_A[20], id_B[20];
    int maxfdp;
    fd_set rset;
    int user_close_A, user_close_B, done_A, done_B;

    // save name
    bzero(id_A, 20);
    bzero(id_B, 20);
    sprintf(id_A, "%s", _id_A);
    sprintf(id_B, "%s", _id_B);
    
    // Initialize;
    maxfdp = max(connfd_A, connfd_B) + 1;
    user_close_A = 0;
    user_close_B = 0;
    done_A = 0;
    done_B = 0;

    // debug: server child info
    printf("Serv: Server child is handling user %s and user %s.\n", id_A, id_B);

    FD_ZERO(&rset);
    for (;;) {
        // check for loop break
        if (user_close_A && user_close_B) {
            break;
        }

        // fd set initialize
        FD_SET(connfd_A, &rset);
        FD_SET(connfd_B, &rset);

        // select
        Select(maxfdp, &rset, NULL, NULL, NULL);

        // check fd set
        if (FD_ISSET(connfd_A, &rset)) {
            // user A sent message
            bzero(recvline, MAXLINE);
            if (readline(connfd_A, recvline, MAXLINE) == 0 ) {
                // user A close connect
                user_close_A = 1;
                if (user_close_B) {
                    // user B has already left
                    bzero(sendline, MAXLINE);

                    //send message to user B
                    sprintf(sendline, "(%s left the room.)\n", id_A);
                    printf("sent A to B: %s", sendline);
                    Writen(connfd_B, sendline, strlen(sendline));
                    
                    // no longer send message to user B
                    return;
                }
                else if (!done_A){
                    done_A = 1;

                    // user B hasn't left yet
                    bzero(sendline, MAXLINE);
                    
                    // send message to user B
                    sprintf(sendline, "(%s left the room. Press Ctrl+D to leave.)\n", id_A);
                    printf("sent A to B: %s", sendline);
                    Writen(connfd_B, sendline, strlen(sendline));

                    // send FIN bit, no more write to user B
                    printf("Close to B.\n");
                    Shutdown(connfd_B, SHUT_WR);
                }
            }
            else if (strlen(recvline) > 1) {
                // user A sent a message
                printf("recv A: %s", recvline);
                bzero(sendline, MAXLINE);
                sprintf(sendline, "(%s) %s", id_A, recvline);

                // sent to user B
                printf("sent A to B: %s", sendline);
                Writen(connfd_B, sendline, strlen(sendline));
            }
        }
        if (FD_ISSET(connfd_B, &rset)) {
            // user B sent a message
            bzero(recvline, MAXLINE);
            if (readline(connfd_B, recvline, MAXLINE) == 0) {
                // user B close connect;
                user_close_B = 1;
                if (user_close_A) {
                    // user A has already left
                    bzero(sendline, MAXLINE);

                    // send message to user A
                    sprintf(sendline, "(%s left the room.)\n", id_B);
                    printf("sent B to A: %s", sendline);
                    Writen(connfd_A, sendline, strlen(sendline));

                    // no longer send message to user A
                    return;
                }
                else if (!done_B) {
                    done_B = 1;
                    // user B hasn't left yet
                    bzero(sendline, MAXLINE);

                    // send message to user B
                    sprintf(sendline, "(%s left the room. Press Ctrl+D to leave.)\n", id_B);
                    printf("sent B to A: %s", sendline);
                    Writen(connfd_A, sendline, strlen(sendline));

                    // send FIN bit, no more write to user B
                    printf("Close to A.\n");
                    Shutdown(connfd_A, SHUT_WR);               
                }
            }
            else if (strlen(recvline) > 1) {
                // user B sent a message
                printf("recv B: %s", recvline);
                bzero(sendline, MAXLINE);
                sprintf(sendline, "(%s) %s", id_B, recvline);

                // sent to user A
                printf("sent B to A: %s", sendline);
                Writen(connfd_A, sendline, strlen(sendline));
            }
        }
    }

}

int main(int argc, char** argv) {
    int listenfd, connfd_A, connfd_B;
    pid_t childpid;
    socklen_t clilen_A, clilen_B;
    struct sockaddr_in servaddr, cliaddr_A, cliaddr_B;
    int have_user;
    char str[INET_ADDRSTRLEN];
    char sendline[MAXLINE];
    char id_A[20], id_B[20];    // 20 is probably enough

    // create listen socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    // server initialize
    // Note: modify IP address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT+4);
    inet_ntop(AF_INET, &(servaddr.sin_addr), str, INET_ADDRSTRLEN);
    printf("addr:port => %s:%d\n", str,servaddr.sin_port);
    fflush(stdout);
    have_user = 0;
    bzero(id_A, sizeof(id_A));
    
    // listen socket
    Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    
    // attach siganl handler
    Signal(SIGCHLD, sig_child);

    for (;;) {
        if (have_user == 0) {   // no user yet
            clilen_A = sizeof(cliaddr_A);
            if ((connfd_A = accept(listenfd, (SA*)&cliaddr_A, &clilen_A)) < 0) {
                // connect to user A fail
                if (errno == EINTR)
                    continue;
                else
                    err_sys("accept error");
            }
            have_user = 1;
            // Connect user A
            bzero(id_A, 20);
            read(connfd_A, id_A, 20);
            printf("Get user A: %s\n", id_A);
            sprintf(sendline, "You are the 1st user. Wait for the second one!\n");
            printf("sent A: %s", sendline);
            Writen(connfd_A, sendline, strlen(sendline));
            
        }
        else {  // user A connect
            clilen_B = sizeof(cliaddr_B);
            if ((connfd_B = accept(listenfd, (SA*)&cliaddr_B, &clilen_B)) < 0) {
                // connect to user B fail
                if (errno == EINTR)
                    continue;
                else
                    err_sys("accept error");
            }
            have_user = 0;
            // Connect user B
            bzero(id_B, 20);
            read(connfd_B, id_B, 20);
            printf("Get user B: %s\n", id_B);

            // send response to user A
            inet_ntop(AF_INET, &(cliaddr_B.sin_addr), str, INET_ADDRSTRLEN);
            sprintf(sendline, "The second user is %s from %s\n", id_B, str);
            printf("sent A: %s", sendline);
            Writen(connfd_A, sendline, strlen(sendline));

            // send response to user B
            sprintf(sendline, "You are the 2nd user.\n");
            printf("sent B: %s", sendline);
            Writen(connfd_B, sendline, strlen(sendline));
            inet_ntop(AF_INET, &(cliaddr_A.sin_addr), str, INET_ADDRSTRLEN);
            sprintf(sendline, "The first user is %s from %s\n", id_A, str);
            printf("sent B: %s", sendline);
            Writen(connfd_B, sendline, strlen(sendline));
            fflush(stdout);

            // create server child and implement the call
            if ( (childpid = Fork()) == 0 ) {
                // child process
                close(listenfd);
                run_server(connfd_A, connfd_B, id_A, id_B); // do it all
                printf("Serv: Both users left the connection.\n");
                exit(0);
            }
            else {
                close(connfd_A);
                close(connfd_B);
            }
        }
    }

}
