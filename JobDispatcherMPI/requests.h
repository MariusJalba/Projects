#ifndef REQUESTS_H
#define REQUESTS_H
int isPrime(int n);
int PrimeDivisors(int n);
int Primes(int n);
int factorial(int n);
void swap(char *a, char *b);
int checkRepeat(char *s, char **permutations, int k);
void createAnagram(char *s, int index, int n, char **permutations, int *k);
int ANAGRAMS(char *s, char ***permutations);
void matrix_addition(int *matrix1, int *matrix2, int *result, int N);
void matrix_multiplication(int *matrix1, int *matrix2, int *result, int N);
int *load_matrix(const char *filename, int N);
void save_matrix(FILE *file, int *matrix, int N);
#endif