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


int findEmptySock(int* p_connfds) {
    for (int i = 0; i < 10; i++) {
        if (p_connfds[i] <= 0) {
            return i;
        }
    }
    return -1;
}

int getMaxFdp(int* p_connfds, int listenfd) {
    int maxfdp = listenfd;
    for (int i = 0; i < 10; i++) {
        if (maxfdp < p_connfds[i]) {
            maxfdp = p_connfds[i];
        }
    }
    return maxfdp + 1;
}

int sendMsgToClient(int* p_connfds, char* sendline, int target) {
    // check socket available
    if (p_connfds[target] < 0) return -1;

    printf("send to %d: %s", target, sendline);
    Writen(p_connfds[target], sendline, strlen(sendline));
    return 1;
}

int broadcastMsg(int* p_connfds, char* sendline, int exclusive) {
    // we use exclusive == -1 to broadcast to every one
    printf("broadcast from %d: %s", exclusive, sendline);
    for (int i = 0; i < 10; i++) {
        if (p_connfds[i] > 0 && i != exclusive) {
            // socket available
            Writen(p_connfds[i], sendline, strlen(sendline));
        }
    }
    return 1;
}
/*
int getId(int* p_connfds, char** pp_ids, int target) {
    if (p_connfds[target] < 0) return -1;
    read(p_connfds[target], pp_ids[target], 20);
    return 1;
}

int removeClient(int* p_connfds, char** pp_ids, int target) {
    p_connfds[target] = -1;
    bzero(pp_ids[target], 20);
    return 1;
}
*/

int main(int argc, char** argv) {
    int listenfd;
    socklen_t clilens[10];
    struct sockaddr_in servaddr, cliaddrs[10];
    char str[INET_ADDRSTRLEN];
    char sendline[MAXLINE], recvline[MAXLINE];
    char ids[10][20];    // 10 ids for 10 ten users;
    int connfds[10], maxfdp = 0;
    int numUser = 0, victim = 0;
    int recvsize = 0;
    fd_set rset;

    // create listen socket
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    // server initialize
    // Note: modify IP address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT+5);
    inet_ntop(AF_INET, &(servaddr.sin_addr), str, INET_ADDRSTRLEN);
    printf("addr:port => %s:%d\n", str, servaddr.sin_port);
    fflush(stdout);

    // socket and id clean up
    for (int i = 0; i < 10; i++) {
        connfds[i] = -1;
        bzero(ids[i], 20);
    }
    FD_ZERO(&rset);

    // listen socket
    Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    
    // attach siganl handler
    Signal(SIGCHLD, sig_child);

    for (;;) {
        // set up select set
        FD_SET(listenfd, &rset);
        for (int i = 0; i < 10; i++) {
            if (connfds[i] > 0) {
                FD_SET(connfds[i], &rset);
            }
        }

        // selecting
        maxfdp = getMaxFdp(connfds, listenfd);
        Select(maxfdp, &rset, NULL, NULL, NULL);

        // listen socket
        if (FD_ISSET(listenfd, &rset)) {
            // check the socket slot
            if (numUser < 10) { // still available
                // victim is the client id
                victim = findEmptySock(connfds);
                clilens[victim] = sizeof(cliaddrs[victim]);
                connfds[victim] = accept(listenfd, (SA*) &cliaddrs[victim], &clilens[victim]);
                // getId(connfds, (char**)ids, victim);
                read(connfds[victim], ids[victim], 20);
                numUser++;
                // send message
                // The "you are..." message is sent to only one user
                sprintf(sendline, "You are the #%d user.\n", numUser);
                sendMsgToClient(connfds, sendline, victim);

                // The "type or wait" message is sent to only one user
                sprintf(sendline, "You may now type in or wait for other users.\n");
                sendMsgToClient(connfds, sendline, victim);
                
                // The "join" message is broadcasted to every one
                sprintf(sendline, "(#%d user %s enters)\n", numUser, ids[victim]);
                broadcastMsg(connfds, sendline, victim);
            }
            // else block the connect
        }

        // check all the client
        for (int i = 0; i < 10; i++) {
            // The fd is in set only if it's activate
            if (connfds[i] > 0){
            if (FD_ISSET(connfds[i], &rset)) {
                // The user sends some message
                // get message and check if the user leaves
                if ((recvsize = readline(connfds[i], recvline, MAXLINE)) == 0) {
                    // The user leaves
                    // send "Bye" message
                    sprintf(sendline, "Bye!\n");
                    sendMsgToClient(connfds, sendline, i);
                    numUser--;

                    // broadcast the left message
                    // we must check if there is a last user
                    if (numUser <= 1) {
                        // one user left
                        sprintf(sendline, "(%s left the room. You are the last one. Press Ctrl+D to leave or wait for a new user.)\n", ids[i]);
                        broadcastMsg(connfds, sendline, i);
                    }
                    else {
                        // more than one user
                        sprintf(sendline, "(%s left the room. %d users left)\n", ids[i], numUser);
                        broadcastMsg(connfds, sendline, i);
                    }
                    // removeClient(connfds, (char**)ids, i);
                    Shutdown(connfds[i], SHUT_WR);
                    connfds[i] = -1;
                    bzero(ids[i], 20);
                }
                else if (recvsize > 1) {
                    // get some message from i, broadcast to everyone
                    sprintf(sendline, "(%s) %s", ids[i], recvline);
                    broadcastMsg(connfds, sendline, i);
                }
            }
            }
        }
        
    }

}
