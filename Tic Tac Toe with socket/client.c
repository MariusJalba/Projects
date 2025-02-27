#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("./client IP port\n");
        exit(0);
    }
    int sockfd;
    struct sockaddr_in server;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SocketError\n");
        exit(1);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));

    if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        printf("ConnectError\n");
        exit(2);
    }
    printf("Connected\n");

    char buffer[1024];
    int n;

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        fsync(sockfd);
        if ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0)
        {
            printf("%s", buffer);
            if (strstr(buffer, "Invalid input") != NULL)
            {
                memset(buffer, 0, sizeof(buffer));
                printf("Select your move: ");
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                write(sockfd, buffer, strlen(buffer));
            }
            else if (strstr(buffer, "make a move") == NULL)
            {
                memset(buffer, 0, sizeof(buffer));
                printf("Select your move: ");
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                write(sockfd, buffer, strlen(buffer));
            }
            else if (strcmp(buffer, "Do you want to play again?") == 0)
            {
                printf("type yes or no");
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                write(sockfd, buffer, strlen(buffer));
            }
        }
        else
        {
            printf("Server disconnected\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
