#include <stdio.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>

#define MAX 255
#define PORT 8080
#define SA struct sockaddr


int printToClient(int sockfd, const char *format, ...) {
    char buff[MAX];
    int result = 0;
    bzero(buff, MAX);
    va_list args;
    va_start(args, format);
    result = vsprintf(buff, format, args);
    write(sockfd, buff, sizeof(buff));
    va_end(args);

    return result;
}

int selectLanguage(int sockfd) {

    int language = 0;
    char buff[MAX];
    while (language == 0) {
        bzero(buff, MAX);
        printToClient(sockfd, "1-English\n2-Turkish\n3-French\n4-Spanish\nPlease Select a Language:");
        read(sockfd, buff, sizeof(buff));
        language = atoi(buff);
        if (language < 0 || language > 4) {
            language = 0;
        }
    }
    printToClient(sockfd, "Selected Language is : %d", language);
    return language;
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

int createSocket(void) {

    int socketi;
    //create socket
    socketi = socket(AF_INET, SOCK_STREAM, 0);
    if (socketi == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    return socketi;
}

int createConnection(int socketi) {

    int connection, len, language;

    struct sockaddr_in serverAddress, clientAddress;


    //clear
    bzero(&serverAddress, sizeof(serverAddress));

    //assign address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);
    //bind socket
    if ((bind(socketi, (SA *) &serverAddress, sizeof(serverAddress))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }

    //listen port
    if ((listen(socketi, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else {
        printf("Server listening..\n");
    }

    len = sizeof(clientAddress);
    //accept connection
    connection = accept(socketi, (SA *) &clientAddress, &len);
    if (connection < 0) {
        printf("server acccept failed...\n");
        exit(0);
    } else {
        printf("server acccept the client...\n");
    }
    return connection;
}

// Driver function
int main() {
    int connection, language, socketi;

    socketi = createSocket();
    connection = createConnection(socketi);
    language = selectLanguage(connection);

    //run command


    //close socket
    close(socketi);
}
