#include<stdio.h>
#include<stdlib.h>
int is24_bit(char *image)
{
    FILE *fin;
    if ((fin = fopen(image, "rb")) == NULL)
    {
        printf("Error opening input image file\n");
        exit(EXIT_FAILURE);
    }
    fseek(fin, 28, SEEK_SET);
    int bit_value = 0;
    fread(&bit_value, sizeof(char), 2, fin); 
    fclose(fin);
    return bit_value;
}