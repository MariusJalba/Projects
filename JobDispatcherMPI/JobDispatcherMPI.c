#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <windows.h>
#include "requests.h"
#include <pthread.h>
#define THRESHOLD 5
#define lineLen 256
typedef struct task
{
    char argument[256];
    char command[256];
    char client[256];
} task;
typedef struct Data
{
    task *taskarray;
    int numtasks;
    int *workersStatus;
    int size;
} Data;
pthread_mutex_t dispatchMutex;
pthread_mutex_t receiveMutex;
double program_start_time = 0.0;
double get_elapsed_time(double start_time)
{
    return MPI_Wtime() - start_time;
}
void parse_file(char *filename, int *numTasks, task **task_array)
{
    FILE *command_file = fopen(filename, "r");
    if (command_file == NULL)
    {
        printf("Error opening command file\n");
        return;
    }
    int capacity = 100;
    *task_array = malloc(sizeof(task) * capacity);
    if (*task_array == NULL)
    {
        printf("Memory allocation failed\n");
        fclose(command_file);
        return;
    }
    char line[lineLen];
    while (fgets(line, lineLen, command_file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        if (*numTasks >= capacity)
        {
            capacity *= 2;
            *task_array = realloc(*task_array, capacity * sizeof(task));
            if (*task_array == NULL)
            {
                printf("Memory reallocation failed\n");
                fclose(command_file);
                return;
            }
        }
        char *token = strtok(line, " ");
        task current_task = {0};
        int command_found = 0;
        int client_found = 0;
        int first_argument = 1;
        while (token != NULL)
        {
            if (strncmp(token, "WAIT", 4) == 0 ||
                strncmp(token, "ANAGRAMS", 8) == 0 ||
                strncmp(token, "PRIMEDIVISORS", 13) == 0 ||
                strncmp(token, "PRIMES", 6) == 0 ||
                strncmp(token, "MATRIXADD", 9) == 0 ||
                strncmp(token, "MATRIXMULT", 10) == 0)
            {
                strcpy(current_task.command, token);
                command_found = 1;
            }
            else if (strncmp(token, "CLI", 3) == 0)
            {
                strcpy(current_task.client, token);
                client_found = 1;
            }
            else
            {
                if (first_argument)
                {
                    strcpy(current_task.argument, token);
                    first_argument = 0;
                }
                else
                {
                    strcat(current_task.argument, " ");
                    strcat(current_task.argument, token);
                }
            }
            token = strtok(NULL, " ");
        }

        (*task_array)[*numTasks] = current_task;
        (*numTasks)++;
    }
    fclose(command_file);
}
void *dispatch_work(void *arg)
{
    FILE *log_file = fopen("server_log.txt", "a");
    Data *data = (Data *)arg;
    task *taskarray = data->taskarray;
    MPI_Status status;
    int numtasks = data->numtasks;
    int size = data->size;
    char client[256] = "a";
    char command[256] = "a";
    char argument[256] = "a";
    char argummentcpy[256] = "a";
    int capacity = 5;
    task *bigMatrixes = malloc(sizeof(task) * capacity);
    int bM = 0;
    for (int i = 0; i < numtasks; i++)
    {
        int found_freeworker = 0;
        while (!found_freeworker)
        {
            for (int j = 1; j < size; j++)
            {
                if (data->workersStatus[j] == 0)
                {
                    double elapsed_time = get_elapsed_time(program_start_time);
                    task currentTask = taskarray[i];
                    strcpy(client, currentTask.client);
                    strcpy(command, currentTask.command);
                    strcpy(argument, currentTask.argument);
                    if (strcmp(command, "MATRIXADD") == 0 || strcmp(command, "MATRIXMULT") == 0)
                    {
                        strcpy(argummentcpy, argument);
                        int N = atoi(strtok(argummentcpy, " "));
                        if (N > THRESHOLD)
                        {
                            bigMatrixes[bM] = currentTask;
                            pthread_mutex_lock(&dispatchMutex);
                            bM++;
                            pthread_mutex_unlock(&dispatchMutex);
                            if (bM >= capacity)
                            {
                                capacity *= 2;
                                bigMatrixes = realloc(bigMatrixes, sizeof(task) * capacity);
                                bigMatrixes[bM] = currentTask;
                                pthread_mutex_lock(&dispatchMutex);
                                bM++;
                                pthread_mutex_unlock(&dispatchMutex);
                            }
                        }
                        else
                        {
                            MPI_Send(client, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                            MPI_Send(command, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                            MPI_Send(argument, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                        }
                        found_freeworker = 1;
                        break;
                    }
                    else
                    {
                        fprintf(log_file, "[%.3f] Dispatching: %s %s %s to worker %d\n",
                                elapsed_time, currentTask.client, currentTask.command, currentTask.argument, j);
                        fflush(log_file);
                        MPI_Send(client, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                        MPI_Send(command, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                        MPI_Send(argument, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                        pthread_mutex_lock(&dispatchMutex);
                        data->workersStatus[j] = 1;
                        found_freeworker = 1;
                        pthread_mutex_unlock(&dispatchMutex);
                        break;
                    }
                }
            }
            if (!found_freeworker)
            {
                Sleep(5);
            }
        }
    }
    for (int i = 0; i < bM; i++)
    {
        char argumentcpy[256] = "a";
        strcpy(client, bigMatrixes[i].client);
        strcpy(command, bigMatrixes[i].command);
        sprintf(command, "%sBLOCKED", command);
        strcpy(argument, bigMatrixes[i].argument);
        strcpy(argumentcpy, argument);
        int N = atoi(strtok(argumentcpy, " "));
        char *file1 = strtok(NULL, " ");
        char *file2 = strtok(NULL, " ");
        int *matrix1 = load_matrix(file1, N);
        int *matrix2 = load_matrix(file2, N);
        int *result = calloc(sizeof(int), N * N);
        for (int j = 1; j < size; j++)
        {
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Dispatching: %s %s %s to worker %d\n",
                    elapsed_time, client, command, argument, j);
            fflush(log_file);
            MPI_Send(client, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
            MPI_Send(command, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
            MPI_Send(argument, lineLen, MPI_CHAR, j, 0, MPI_COMM_WORLD);
            int rows_per_worker = N / (size - 1);
            int start = (j - 1) * rows_per_worker;
            int end = (j == size - 1) ? N : start + rows_per_worker;
            for (int k = start; k < end; k++)
            {
                for (int l = 0; l < N; l++)
                {
                    MPI_Send(&matrix1[k * N + l], 1, MPI_INT, j, 0, MPI_COMM_WORLD);
                    MPI_Send(&matrix2[k * N + l], 1, MPI_INT, j, 0, MPI_COMM_WORLD);
                }
            }
        }
        free(matrix1);
        free(matrix2);
        free(result);
    }
    strcpy(command, "STOP");
    for (int i = 1; i < size; i++)
    {
        MPI_Send(command, lineLen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        MPI_Send(command, lineLen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        MPI_Send(command, lineLen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    return NULL;
}
void worker(int size)
{
    MPI_Status status;
    char client[lineLen] = {0};
    char command[lineLen] = {0};
    char argument[lineLen] = {0};
    int number = 0;
    int anagramSize = 0;
    char **anagrams = NULL;
    int rank = 0;
    int count = 0;
    int Notfinished = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    while (Notfinished != 0)
    {
        MPI_Recv(client, lineLen, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(command, lineLen, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(argument, lineLen, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        double elapsed_time = get_elapsed_time(program_start_time);
        FILE *log_file = fopen("server_log.txt", "a");
        if (log_file == NULL)
        {
            printf("Error opening log file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if (strcmp(command, "STOP") == 0 && strcmp(client, "STOP") == 0 && strcmp(argument, "STOP") == 0)
        {
            Notfinished = 0;
        }
        else if (strcmp(command, "PRIMES") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            number = Primes(atoi(argument));
            MPI_Send(client, lineLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        else if (strcmp(command, "PRIMEDIVISORS") == 0)
        {
            number = PrimeDivisors(atoi(argument));
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            MPI_Send(client, lineLen, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
        else if (strcmp(command, "ANAGRAMS") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            anagramSize = ANAGRAMS(argument, &anagrams);
            MPI_Send(client, lineLen, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
            MPI_Send(&anagramSize, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            for (int i = 0; i < anagramSize; i++)
            {
                MPI_Send(anagrams[i], strlen(anagrams[i] + 1), MPI_CHAR, 0, 2, MPI_COMM_WORLD);
            }
            double elapsed_time = get_elapsed_time(program_start_time);
        }
        else if (strcmp(command, "MATRIXADD") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            int N = atoi(strtok(argument, " "));
            char *file1 = strtok(NULL, " ");
            char *file2 = strtok(NULL, " ");
            int *matrix1 = load_matrix(file1, N);
            int *matrix2 = load_matrix(file2, N);
            int *result = malloc(sizeof(int) * N * N);
            matrix_addition(matrix1, matrix2, result, N);
            MPI_Send(client, lineLen, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
            MPI_Send(&N, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    MPI_Send(&result[i * N + j], 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
                }
            }
            free(matrix1);
            free(matrix2);
            free(result);
        }
        else if (strcmp(command, "MATRIXMULT") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            int N = atoi(strtok(argument, " "));
            char *file1 = strtok(NULL, " ");
            char *file2 = strtok(NULL, " ");
            int *matrix1 = load_matrix(file1, N);
            int *matrix2 = load_matrix(file2, N);
            int *result = malloc(sizeof(int) * N * N);
            matrix_multiplication(matrix1, matrix2, result, N);
            MPI_Send(client, lineLen, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
            MPI_Send(&N, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    MPI_Send(&result[i * N + j], 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
                }
            }
            free(matrix1);
            free(matrix2);
            free(result);
        }
        else if (strcmp(command, "MATRIXADDBLOCKED") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            int N = atoi(strtok(argument, " "));
            int rows_per_worker = N / (size - 1);
            int start_row = rows_per_worker * (rank - 1);
            int end_row = start_row + rows_per_worker;
            if (rank == size - 1)
            {
                end_row = N;
            }
            int *result = malloc(sizeof(int) * N * N);
            int *matrix1 = calloc(sizeof(int), N * N);
            int *matrix2 = malloc(sizeof(int) * N * N);
            for (int k = start_row; k < end_row; k++)
            {
                for (int l = 0; l < N; l++)
                {
                    MPI_Recv(&matrix1[k * N + l], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&matrix2[k * N + l], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                }
            }
            for (int i = start_row; i < end_row; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    result[i * N + j] = matrix1[i * N + j] + matrix2[i * N + j];
                }
            }
            MPI_Send(client, lineLen, MPI_CHAR, 0, 5, MPI_COMM_WORLD);
            MPI_Send(&N, 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
            for (int k = start_row; k < end_row; k++)
            {
                for (int l = 0; l < N; l++)
                {
                    MPI_Send(&result[k * N + l], 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
                }
            }
            free(matrix1);
            free(matrix2);
            free(result);
        }
        else if (strcmp(command, "MATRIXMULTBLOCKED") == 0)
        {
            fprintf(log_file, "[%.3f] Working: %s %s %s \n",
                    elapsed_time, client, command, argument);
            fflush(log_file);
            int N = atoi(strtok(argument, " "));
            char *file1 = strtok(NULL, " ");
            char *file2 = strtok(NULL, " ");
            int *matrix1 = load_matrix(file1, N);
            int *matrix2 = load_matrix(file2, N);
            int *result = calloc(sizeof(int), N * N);
            int rows_per_worker = N / (size - 1);
            int start_row = rows_per_worker * (rank - 1);
            int end_row = start_row + rows_per_worker;
            if (rank == size - 1)
            {
                end_row = N;
            }
            for (int k = start_row; k < end_row; k++)
            {
                for (int l = 0; l < N; l++)
                {
                    MPI_Recv(&matrix1[k * N + l], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&matrix2[k * N + l], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                }
            }
            for (int i = start_row; i < end_row; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    result[i * N + j] = 0;
                    for (int k = 0; k < N; k++)
                    {
                        result[i * N + j] += matrix1[i * N + k] * matrix2[k * N + j];
                    }
                }
            }
            MPI_Send(client, lineLen, MPI_CHAR, 0, 6, MPI_COMM_WORLD);
            MPI_Send(&N, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
            for (int k = start_row; k < end_row; k++)
            {
                for (int l = 0; l < N; l++)
                {
                    MPI_Send(&result[k * N + l], 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
                }
            }
            free(matrix1);
            free(matrix2);
            free(result);
        }
        fclose(log_file);
    }
}
void *receive_results(void *arg)
{
    Data *data = (Data *)arg;
    FILE *log_file = fopen("server_log.txt", "a");
    if (log_file == NULL)
    {
        printf("Error opening log file for receiving results.\n");
        return NULL;
    }
    MPI_Status status;
    MPI_Request request;
    FILE *fout;
    int size = data->size;
    char result[256] = "a";
    char client[256] = "a";
    char anagram[256] = "a";
    int number = 0;
    int results_received = 0;
    int numTasks = data->numtasks;
    int anagramsSize = 0;
    int N = 0;
    for (int i = 0; i < numTasks; i++)
    {
        MPI_Recv(client, lineLen, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int source = status.MPI_SOURCE;
        int tag = status.MPI_TAG;
        char filename[256];
        sprintf(filename, "%s.txt", client);
        fout = fopen(filename, "a");
        if (fout == NULL)
        {
            printf("Error opening file %s\n", filename);
            continue;
        }
        if (tag == 0)
        {
            MPI_Recv(&number, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            fprintf(fout, "PRIMES: %d\n", number);
            fflush(log_file);
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 1)
        {
            MPI_Recv(&number, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
            fprintf(fout, "PRIMEDIVISORS: %d\n", number);
            fflush(log_file);
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 2)
        {
            MPI_Recv(&anagramsSize, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            fprintf(fout, "ANAGRAMS:\n");
            fflush(log_file);
            for (int i = 0; i < anagramsSize; i++)
            {
                MPI_Recv(anagram, lineLen, MPI_CHAR, source, 2, MPI_COMM_WORLD, &status);
                fprintf(fout, "%s\n", anagram);
            }
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 3)
        {
            MPI_Recv(&N, 1, MPI_INT, source, 3, MPI_COMM_WORLD, &status);
            int number = 0;
            fprintf(fout, "MATRIXADD:\n");
            fflush(log_file);
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    MPI_Recv(&number, 1, MPI_INT, source, 3, MPI_COMM_WORLD, &status);
                    fprintf(fout, "%d ", number);
                }
                fprintf(fout, "\n");
            }
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 4)
        {
            MPI_Recv(&N, 1, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
            int number = 0;
            fprintf(fout, "MATRIXMULT:\n");
            fflush(log_file);
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    MPI_Recv(&number, 1, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
                    fprintf(fout, "%d ", number);
                }
                fprintf(fout, "\n");
            }
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 5)
        {
            int N = 0;
            MPI_Recv(&N, 1, MPI_INT, source, 5, MPI_COMM_WORLD, &status);
            int rows_per_worker = N / (size - 1);
            for (int i = 1; i < size; i++)
            {
                if (i > 1)
                {
                    MPI_Recv(client, lineLen, MPI_CHAR, i, 5, MPI_COMM_WORLD, &status);
                    MPI_Recv(&N, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &status);
                }
                int *result = malloc(sizeof(int) * rows_per_worker * N);
                int start_row = (i - 1) * rows_per_worker;
                int end_row = (i == size - 1) ? N : start_row + rows_per_worker;
                for (int k = start_row; k < end_row; k++)
                {
                    for (int l = 0; l < N; l++)
                    {
                        MPI_Recv(&result[k * N + l], 1, MPI_INT, i, 5, MPI_COMM_WORLD, &status);
                        fflush(fout);
                        fprintf(fout, "%d ", result[k * N + l]);
                    }
                    fflush(fout);
                    fprintf(fout, "\n");
                }
            }
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        else if (tag == 6)
        {
            int N = 0;
            MPI_Recv(&N, 1, MPI_INT, source, 6, MPI_COMM_WORLD, &status);
            int rows_per_worker = N / (size - 1);
            for (int i = 1; i < size; i++)
            {
                if (i > 1)
                {
                    MPI_Recv(client, lineLen, MPI_CHAR, i, 6, MPI_COMM_WORLD, &status);
                    MPI_Recv(&N, 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);
                }
                int *result = malloc(sizeof(int) * rows_per_worker * N);
                int start_row = (i - 1) * rows_per_worker;
                int end_row = (i == size - 1) ? N : start_row + rows_per_worker;
                for (int k = start_row; k < end_row; k++)
                {
                    for (int l = 0; l < N; l++)
                    {
                        MPI_Recv(&result[k * N + l], 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);
                        fflush(fout);
                        fprintf(fout, "%d ", result[k * N + l]);
                    }
                    fflush(fout);
                    fprintf(fout, "\n");
                }
            }
            double elapsed_time = get_elapsed_time(program_start_time);
            fprintf(log_file, "[%.3f] Received results for client: %s from worker %d\n",
                    elapsed_time, client, status.MPI_SOURCE);
            fflush(log_file);
        }
        fclose(fout);
        pthread_mutex_lock(&receiveMutex);
        data->workersStatus[source] = 0;
        pthread_mutex_unlock(&receiveMutex);
    }
    fclose(log_file);
    return NULL;
}
void mainServer(task *taskarray, int numTasks, int size)
{
    FILE *log_file = fopen("server_log.txt", "w");
    if (log_file == NULL)
    {
        printf("Error opening log file.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int *workersStatus = calloc(size + 1, sizeof(int));
    Data data = {taskarray, numTasks, workersStatus, size};
    pthread_t dispatch_thread;
    pthread_t receive_thread;
    pthread_create(&dispatch_thread, NULL, dispatch_work, &data);
    pthread_create(&receive_thread, NULL, receive_results, &data);
    pthread_join(dispatch_thread, NULL);
    pthread_join(receive_thread, NULL);
    free(workersStatus);
    printf("Final time:%.3f seconds", get_elapsed_time(program_start_time));
    fclose(log_file);
}

int main(int argc, char *argv[])
{
    int size;
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pthread_mutex_init(&dispatchMutex, NULL);
    pthread_mutex_init(&receiveMutex, NULL);
    program_start_time = MPI_Wtime();
    if (rank == 0)
    {
        int numTasks = 0;
        task *taskArray = NULL;
        if (argc < 2)
        {
            printf("Usage: %s <filename>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        parse_file(argv[1], &numTasks, &taskArray);
        mainServer(taskArray, numTasks, size);
        free(taskArray);
    }
    else
    {
        worker(size);
    }
    pthread_mutex_destroy(&dispatchMutex);
    pthread_mutex_destroy(&receiveMutex);
    MPI_Finalize();
    return 0;
}