#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
void encode(char *image, char *text, char *out)
{
    FILE *imgIN;
    FILE *textIN;
    FILE *imgOUT;
    int width;
    int height;
    char pixel[3] = {0};
    char c[2] = "a";
    int buffer[55] = {0};
    int size = 0;
    if ((imgIN = fopen(image, "rb")) == NULL)
    {
        printf("Error opening image input file");
        exit(EXIT_FAILURE);
    }
    if ((textIN = fopen(text, "rb")) == NULL)
    {
        printf("Error opening text file");
        exit(EXIT_FAILURE);
    }
    if ((imgOUT = fopen(out, "wb")) == NULL)
    {
        printf("Error opening image output file");
        exit(EXIT_FAILURE);
    }
    fseek(imgIN, 18, SEEK_CUR);
    fread(&width, sizeof(int), 1, imgIN);
    fread(&height, sizeof(int), 1, imgIN);
    fseek(imgIN, 0, SEEK_SET);
    fread(&buffer, sizeof(char), 54, imgIN);
    fwrite(buffer, sizeof(char), 54, imgOUT);
    fseek(textIN, 0, SEEK_END);
    size = ftell(textIN);
    fseek(textIN, 0, SEEK_SET);
    fread(pixel, sizeof(char), 3, imgIN);
    pixel[2] = size & 0xFF;
    pixel[1] = (size >> 8) & 0xFF;
    pixel[0] = (size >> 16) & 0xFF;
    fwrite(pixel, sizeof(char), 3, imgOUT);
    int charactersRead = 0;
    int k = 6;
    fread(c, sizeof(char), 1, textIN);
    charactersRead++;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fread(pixel, sizeof(char), 3, imgIN);
            if (charactersRead <= size)
            {
                if (k >= 0)
                {
                    pixel[2] = (pixel[2] & 0xFC) | (((c[0]) >> k) & 0x3);
                    k-=2;
                }
                else
                {
                    fread(c, sizeof(char), 1, textIN);
                    charactersRead++;
                    k = 6;
                }
            }
            fwrite(pixel, sizeof(char), 3, imgOUT);
        }
    }
    fclose(imgIN);
    fclose(imgOUT);
    fclose(textIN);
}