#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
int isPrime(int n)
{
    if (n <= 1)
    {
        return 0;
    }
    int flag = 1;
    for (int i = 2; i < n / 2; i++)
    {
        if (n % i == 0)
        {
            flag = 0;
            break;
        }
    }
    if (flag == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
int Primes(int n)
{
    bool *prime = malloc(sizeof(bool) * n);
    for (int i = 0; i < n; i++)
    {
        prime[i] = true;
    }
    prime[0] = prime[1] = false;
    // Apply the Sieve of Eratosthenes
    for (int p = 2; p * p < n; p++)
    {
        if (prime[p] == true)
        {
            for (int i = p * p; i < n; i += p)
            {
                prime[i] = false;
            }
        }
    }
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (prime[i])
        {
            count++;
        }
    }
    return count;
}
int PrimeDivisors(int n)
{
    int count = 0;
    while (n % 2 == 0)
    {
        count++;
        n = n / 2;
    }
    for (int i = 3; i * i <= n; i = i + 2)
    {

        while (n % i == 0)
        {
            count++;
            n = n / i;
        }
    }
    if (n > 2)
        count++;
    return count;
}
void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

void generatePermutations(char *s, int l, int r, char **permutations, int *count)
{
    if (l == r)
    {
        strcpy(permutations[*count], s);
        (*count)++;
        return;
    }
    for (int i = l; i <= r; i++)
    {
        swap(&s[l], &s[i]);
        generatePermutations(s, l + 1, r, permutations, count);
        swap(&s[l], &s[i]);
    }
}

int ANAGRAMS(char *s, char ***permutations)
{
    int n = strlen(s);
    int N = 1;
    for (int i = 2; i <= n; i++)
    {
        if (N > 500)
        {
            break;
        }
        N *= i;
    }

    *permutations = (char **)malloc(sizeof(char *) * N);
    if (*permutations == NULL)
    {
        printf("Memory allocation failed\n");
        return -1;
    }

    for (int i = 0; i < N; i++)
    {
        (*permutations)[i] = (char *)malloc(sizeof(char) * (n + 1));
        if ((*permutations)[i] == NULL)
        {
            printf("Memory allocation failed for permutation %d\n", i);
            return -1;
        }
    }

    int count = 0;
    generatePermutations(s, 0, n - 1, *permutations, &count);
    return count;
}
void matrix_addition(int *matrix1, int *matrix2, int *result, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result[i * N + j] = matrix1[i * N + j] + matrix2[i * N + j];
        }
    }
}
void matrix_multiplication(int *matrix1, int *matrix2, int *result, int N)
{
    for (int i = 0; i < N; i++)
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
}
int *load_matrix(const char *filename, int N)
{
    FILE *file = fopen(filename, "r");
    int *matrix = malloc(N * N * sizeof(int));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fscanf(file, "%d", &matrix[i * N + j]);
        }
    }
    fclose(file);
    return matrix;
}

void save_matrix(FILE *file, int *matrix, int N)
{

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fprintf(file, "%d ", matrix[i * N + j]);
        }
        fprintf(file, "\n");
    }
}