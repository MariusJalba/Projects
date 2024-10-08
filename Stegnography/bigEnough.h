#include <stdio.h>
#include <stdlib.h>
int bigEnough(char *image, char *text)
{
    FILE *img;
    FILE *txt;
    int width = 0;
    int height = 0;
    int size_txt = 0;
    if ((img = fopen(image, "rb")) == NULL)
    {
        printf("Error opening input image file\n");
        exit(EXIT_FAILURE);
    }
    if ((txt = fopen(text, "rb")) == NULL)
    {
        printf("Error opening input text file\n");
        exit(EXIT_FAILURE);
    }
    fseek(img, 18, SEEK_CUR);
    fread(&width, sizeof(int), 1, img);
    fread(&height, sizeof(int), 1, img);
    fseek(txt,0,SEEK_END);
    size_txt = ftell(txt);
    if((size_txt * 4)  <= width * height)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    fclose(img);
    fclose(txt);
}