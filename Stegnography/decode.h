#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
void decode(char *image)
{
    FILE *fin;
    FILE *fout;
    char pixel[3];
    int width = 0;
    int height = 0;
    char c[2];
    if ((fin = fopen(image, "rb")) == NULL)
    {
        printf("Error opening input image file\n");
        exit(EXIT_FAILURE);
    }
    if ((fout = fopen("out.txt", "w")) == NULL)
    {
        printf("Error opening output text file\n");
        exit(EXIT_FAILURE);
    }
    fseek(fin, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fin);
    fread(&height, sizeof(int), 1, fin);
    fseek(fin, 54, SEEK_SET);
    char sizeBytes[3];
    fread(sizeBytes, sizeof(char), 3, fin);
    int size = (sizeBytes[2]) | (sizeBytes[1] << 8) | (sizeBytes[0] << 16);
    int charactersRead = 0;
    int contor = 0;
    c[0] = 0x0;
    while (charactersRead < size)
    {
        if (contor < 4)
        {
            fread(pixel, sizeof(char), 3, fin);
            //showBits(pixel[2]);
            c[0] = (c[0] << 2) | (pixel[2] & 0x03);
            contor++;
        }
        else
        {
            charactersRead++;
            contor = 0;
            fread(pixel,sizeof(char),3,fin);
            if(c[0] != '\n')
            {
                fputc(c[0], fout);
            }
            c[0] = 0x0;
        }
    }
    fclose(fin);
    fclose(fout);
}
