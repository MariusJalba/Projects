#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
typedef struct Game
{
    char board[3][3];
    char currentPlayer;
    int connfd1;
    int connfd2;
    pthread_mutex_t lock;
    struct sockaddr_in client;
} Game;
void showBoard(Game *g)
{
    fsync(g->connfd1);
    fsync(g->connfd2);
    char buffer[1024] = {0};
    strcat(buffer, "|-----|\n");

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            char cell[4];
            sprintf(cell, "|%c", g->board[i][j]);
            strcat(buffer, cell);
        }
        strcat(buffer, "|\n");
    }
    strcat(buffer, "|-----|\n\0");
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
}
void updateBoard(Game *g, int cell, char player)
{
    int row = (cell - 1) / 3;
    int col = (cell - 1) % 3;
    char buffer[256] = {0};

    if (g->board[row][col] == 'O' || g->board[row][col] == 'X')
    {
        strcpy(buffer, "Cell already occupied. Select another one.\n");
        if (g->currentPlayer == 'X')
        {
            write(g->connfd1, buffer, strlen(buffer));
        }
        else
        {
            write(g->connfd2, buffer, strlen(buffer));
        }
    }
    else
    {
        g->board[row][col] = player;
        g->currentPlayer = (g->currentPlayer == 'X') ? 'O' : 'X';
        showBoard(g);
    }
}
void makeMove(Game *g)
{
    char buffer[256] = {0};
    char input[256] = {0};
    int cell = 0;

    sprintf(buffer, "%c's turn\n", g->currentPlayer);
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));

    if (g->currentPlayer == 'X')
    {
        fsync(g->connfd1);
        fsync(g->connfd2);
        strcpy(buffer, "Waiting for Player X to make a move...\n");
        write(g->connfd2, buffer, strlen(buffer));
        while (1)
        {
            memset(input, 0, sizeof(input));
            fsync(g->connfd1);
            fsync(g->connfd2);
            read(g->connfd1, input, sizeof(input) - 1);
            printf("input:%s", input);
            cell = atoi(input);
            printf("%d", cell);
            if (cell < 1 || cell > 9)
            {
                strcpy(buffer, "Invalid input. Please select a valid cell (1-9):\n");
                write(g->connfd1, buffer, strlen(buffer));
            }
            else
            {
                updateBoard(g, cell, g->currentPlayer);
                break;
            }
        }
    }
    else
    {

        strcpy(buffer, "Waiting for Player O to make a move...\n");
        fsync(g->connfd1);
        fsync(g->connfd2);
        write(g->connfd1, buffer, strlen(buffer));
        while (1)
        {
            memset(input, 0, sizeof(input));
            read(g->connfd2, input, sizeof(input) - 1);
            printf("input:%s", input);
            cell = atoi(input);
            printf("%d", cell);
            if (cell < 1 || cell > 9)
            {
                strcpy(buffer, "Invalid input. Please select a valid cell (1-9):\n");
                write(g->connfd2, buffer, strlen(buffer));
            }
            else
            {
                updateBoard(g, cell, g->currentPlayer);
                break;
            }
        }
    }
}

void resetBoard(Game *g)
{
    pthread_mutex_lock(&g->lock);
    char cellNumber = '1';
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            g->board[i][j] = cellNumber++;
        }
    }
    pthread_mutex_unlock(&g->lock);
}
int checkDraw(Game *g)
{
    pthread_mutex_lock(&g->lock);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (g->board[i][j] != 'X' && g->board[i][j] != 'O')
            {
                pthread_mutex_unlock(&g->lock);
                return 0;
            }
        }
    }
    char buffer[1024] = "Draw\n";
    write(g->connfd1, buffer, strlen(buffer));
    write(g->connfd2, buffer, strlen(buffer));
    pthread_mutex_unlock(&g->lock);
    return 1;
}

int checkWin(Game *g)
{
    pthread_mutex_lock(&g->lock);
    for (int i = 0; i < 3; i++)
    {
        if (g->board[i][0] == g->board[i][1] && g->board[i][1] == g->board[i][2] && g->board[i][0] != ' ')
        {
            char buffer[1024];
            sprintf(buffer, "%c Won\n", g->board[i][0]);
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));
            return 1;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (g->board[0][i] == g->board[1][i] && g->board[1][i] == g->board[2][i] && g->board[0][i] != ' ')
        {
            char buffer[1024];
            sprintf(buffer, "%c Won\n", g->board[0][i]);
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));

            return 1;
        }
    }
    if (g->board[0][0] == g->board[1][1] && g->board[1][1] == g->board[2][2] && g->board[0][0] != ' ')
    {
        char buffer[1024];
        sprintf(buffer, "%c Won\n", g->board[0][0]);
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));

        return 1;
    }
    if (g->board[0][2] == g->board[1][1] && g->board[1][1] == g->board[2][0] && g->board[0][2] != ' ')
    {
        char buffer[1024];
        sprintf(buffer, "%c Won\n", g->board[0][2]);
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
        return 1;
    }
    pthread_mutex_unlock(&g->lock);
    return 0;
}

void *initializeBoard(void *arg);

void playAgain(Game *g)
{
    char buffer[1024];
    while (1)
    {
        strcpy(buffer, "Do you want to play again?\n");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
        char ans1[10] = {0};
        char ans2[10] = {0};
        read(g->connfd1, ans1, sizeof(ans1) - 1);
        read(g->connfd2, ans2, sizeof(ans2) - 1);
        printf("%s %s", ans1, ans2);
        if (strncmp(ans1, "yes", 3) == 0 && strncmp(ans2, "yes", 3) == 0)
        {
            resetBoard(g);
            g->currentPlayer = (rand() % 2 == 0) ? 'X' : 'O';
            initializeBoard((void *)g);
            return;
        }
        else
        {
            strcpy(buffer, "Game over!\n");
            write(g->connfd1, buffer, strlen(buffer));
            write(g->connfd2, buffer, strlen(buffer));
            close(g->connfd1);
            close(g->connfd2);
            free(g);
            pthread_exit(NULL);
        }
    }
}

void *initializeBoard(void *arg)
{
    Game *g = (Game *)arg;
    char cellNumber = '1';
    char buffer[1024] = {'a'};
    char buffer1[1024] = {"a"};
    srand(time(NULL));
    g->currentPlayer = rand() % 2 == 0 ? 'X' : 'O';
    strcpy(buffer, "You are X\n\0");
    write(g->connfd1, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "You are O\n\0");
    write(g->connfd2, buffer, strlen(buffer));
    if (g->currentPlayer == 'X')
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "X begins\n\0");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
    }
    else
    {
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "O begins\n\0");
        write(g->connfd1, buffer, strlen(buffer));
        write(g->connfd2, buffer, strlen(buffer));
    }
    resetBoard(g);
    showBoard(g);
    while (checkWin(g) != 1 && checkDraw(g) != 1)
    {
        makeMove(g);
    }
    playAgain(g);
    return NULL;
}
int main(int argc, char *argv[])
{
    int sockfd;
    int connfd1;
    int connfd2;
    struct sockaddr_in serveraddr;
    if (argc != 3)
    {
        printf("Usage:./server IP PORT\n");
        exit(0);
    }
    int port = atoi(argv[2]);
    char *ip = argv[1];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket creation succesfull\n");
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) != 0)
    {
        printf("socket bind failed\n");
        exit(1);
    }
    else
        printf("Socket successfully binded\n");
    if ((listen(sockfd, 10)) != 0)
    {
        printf("listen failed \n");
        exit(2);
    }
    printf("Server listening\n");
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if ((connfd1 = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error accepting client1");
            exit(3);
        }
        printf("Client1 connected\n");
        if ((connfd2 = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Error accepting client2");
            exit(3);
        }
        printf("Client2 connected\n");
        Game *g = malloc(sizeof(Game));
        if (g == NULL)
        {
            perror("Memory allocation failed");
            close(connfd1);
            close(connfd2);
            continue;
        }
        pthread_mutex_init(&g->lock, NULL);
        g->client = client_addr;
        g->connfd1 = connfd1;
        g->connfd2 = connfd2;
        pthread_t thread;
        pthread_create(&thread, NULL, initializeBoard, (void *)g);
        pthread_join(thread, NULL);
    }
    close(sockfd);
    close(connfd1);
    close(connfd2);
    return 0;
}
