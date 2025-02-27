#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "requests.h"
#define lineLen 256

typedef struct task
{
    char argument[256];
    char command[256];
    char client[256];
} task;

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
            task *temp = realloc(*task_array, capacity * 2 * sizeof(task));
            if (temp == NULL)
            {
                printf("Memory reallocation failed\n");
                fclose(command_file);
                free(*task_array);
                return;
            }
            *task_array = temp;
            capacity *= 2;
        }

        char *token = strtok(line, " ");
        task current_task = {0};
        while (token != NULL)
        {
            if (strncmp(token, "WAIT", 4) == 0)
            {
                strcpy(current_task.command, token);
                strcpy(current_task.client, "CLIWAIT");
            }
            else if (strncmp(token, "CLI", 3) == 0)
            {
                strcpy(current_task.client, token);
            }
            else if (strncmp(token, "ANAGRAMS", 8) == 0 ||
                     strncmp(token, "PRIMEDIVISORS", 13) == 0 ||
                     strncmp(token, "PRIMES", 6) == 0)
            {
                strcpy(current_task.command, token);
            }
            else
            {
                strcpy(current_task.argument, token);
            }
            token = strtok(NULL, " ");
        }

        (*task_array)[*numTasks] = current_task;
        (*numTasks)++;
    }

    fclose(command_file);
}

void work(task *taskArray, int numtasks)
{
    char clientfile[256] = "a";
    for (int i = 0; i < numtasks; i++)
    {
        sprintf(clientfile, "%s.txt", taskArray[i].client);
        FILE *fclient = fopen(clientfile, "a");
        if (fclient == NULL)
        {
            printf("Error opening client file: %s\n", taskArray[i].client);
            continue;
        }
        if (strcmp(taskArray[i].command, "WAIT") == 0)
        {
            int wait_time = atoi(taskArray[i].argument);
            Sleep(wait_time);
        }
        else if (strcmp(taskArray[i].command, "PRIMES") == 0)
        {
            int number = atoi(taskArray[i].argument);
            int result = Primes(number);
            fprintf(fclient, "PRIMES: %d\n", result);
        }
        else if (strcmp(taskArray[i].command, "ANAGRAMS") == 0)
        {
            char **anagrams = NULL;
            int numAnagrams = ANAGRAMS(taskArray[i].argument, &anagrams);
            fprintf(fclient, "ANAGRAMS:\n");
            for (int j = 0; j < numAnagrams; j++)
            {
                fprintf(fclient, "%s\n", anagrams[j]);
                free(anagrams[j]);
            }
            free(anagrams);
        }
        else if (strcmp(taskArray[i].command, "PRIMEDIVISORS") == 0)
        {
            int number = atoi(taskArray[i].argument);
            int result = PrimeDivisors(number);
            fprintf(fclient, "PRIMEDIVISORS: %d\n", result);
        }
        fclose(fclient);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command_file>\n", argv[0]);
        return 1;
    }

    task *taskArray = NULL;
    int numtasks = 0;
    struct timespec start, finish;

    parse_file(argv[1], &numtasks, &taskArray);

    clock_gettime(CLOCK_MONOTONIC, &start);
    work(taskArray, numtasks);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    double elapsed_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    free(taskArray);
    return 0;
}
