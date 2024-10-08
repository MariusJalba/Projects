#include<stdio.h>
#include<string.h>
int verify_extension(char *img, char *text)
{
    char *token1 = strrchr(img, 46);
    char *token2 = strrchr(text, 46);
    if (token1 && token2)
    {
        if ((strncmp(token1, ".bmp", 4) == 0) && (strcmp(token2, ".txt") == 0))
        {
            return 1;
        }
    }
    else
    {
        printf("No extensions found");
        return 0;
    }
    return 0;
}
int verify_options(char* option)
{
    if(strcmp(option,"encode") == 0)
    {
        return 1;
    }
    if(strcmp(option,"decode")==0)
    {
        return 2;
    }
    return 0;
}