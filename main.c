#include <stdio.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>


#define MAX 255
#define PORT 9000
#define SA struct sockaddr

const char *languages[4] = {"English", "French", "Spanish", "Turkish"};
const char *translation[4][10] = {
        {"zero",  "one", "two",  "three", "four",   "five",  "six",  "seven", "eight",  "nine"},
        {"zéro",  "un",  "deux", "trois", "quatre", "cinq",  "six",  "sept",  "huit",   "neuf"},
        {"cero",  "uno", "dos",  "tres",  "cuatro", "cinco", "seis", "siete", "ocho",   "nueve"},
        {"sıfır", "bir", "iki", "üç",    "dört",   "beş",   "altı", "yedi̇", "seki̇z", "dokuz"}
};

void translateFromWord(char buff[255], int language, int connection);


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


    int language = -1;
    char buff[MAX];
    while (language == -1) {
        bzero(buff, MAX);
        printToClient(sockfd, "English(0), French(1), Spanish(2), Turkish(3)");
        read(sockfd, buff, sizeof(buff));
        language = atoi(buff);
        if (language < 0 || language > 4) {
            language = -1;
        }
    }
    printf("Client set language as %s \n", languages[language]);
    printToClient(sockfd, "Client set language as %s \n", languages[language]);


    return language;
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

void translateFromDigit(int digit, int language, int connection) {

    printf("This is positive integer so process and return “%s”\n", translation[language][digit]);
    printToClient(connection, "%s\n", translation[language][digit]);
}

int createConnection(int socketi) {

    int connection, len;

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
    char buff[MAX];


    socketi = createSocket();
    connection = createConnection(socketi);
    language = selectLanguage(connection);
    while (1) {
        bzero(buff, MAX);
        read(connection, buff, sizeof(buff));
        for (int i = 0; i < MAX; i++) {
            if (buff[i] == '\r') {
                buff[i] = '\0';
            }
            if (buff[i] == '\n') {
                buff[i] = '\0';
            }
        }
        if (strcmp("quit", buff) == 0) {
            printf("Send “Goodbye” and disconnect client from server.\n");
            printToClient(connection, "Goodbye\n");
            break;
        }

        if (isdigit(buff[0])) {
            translateFromDigit(atoi(buff), language, connection);
        } else {
            translateFromWord(buff, language, connection);
        }
    }

    //close socket
    close(socketi);

}

void translateFromWord(char buff[255], int language, int connection) {


    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 10; j++) {
            if (strcmp(buff, translation[i][j]) == 0) {
                if (language == i) {
                    printf("This is dictionary word process and response to client \"%d\"\n", j);
                    printToClient(connection, "%d\n", j);
                    return;
                } else {
                    printf("This is %s dictionary word but we choose %s in beginning so return same \"%s\"\n",
                           languages[i], languages[language], buff);
                    printToClient(connection, "%s\n", buff);
                    return;
                }
            }
        }
    }
    printf("Process word but this is not dictionary word so response as same \"%s\" \n", buff);
    printToClient(connection, "%s\n", buff);

}

