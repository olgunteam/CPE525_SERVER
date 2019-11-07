#include <stdio.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX 255
#define PORT 8080
#define SA struct sockaddr


void showOptions(int sockfd) {

    char buff[MAX];
    bzero(buff, MAX);
    sprintf(buff, "1-English\n2-Turkish\n3-French\n4-Spanish\nPlease Select a Language:");
    write(sockfd, buff, sizeof(buff));
}

void func(int sockfd) {
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {

        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');

        // and send that buffer to client
        write(sockfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


// Driver function
int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //clear
    bzero(&servaddr, sizeof(servaddr));

    //assign address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    //bind socket
    bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

    //listen port
    listen(sockfd, 5);
    len = sizeof(cli);
    //accept connection
    connfd = accept(sockfd, (SA *) &cli, &len);

    showOptions(connfd);

    //run command
    func(connfd);

    //clone socket
    close(sockfd);
}
