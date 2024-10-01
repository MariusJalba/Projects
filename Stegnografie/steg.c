#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "decode.h"
#include "encode.h"
#include "verify_24bit.h"
#include "arguments.h"
#include "bigEnough.h"
int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("Arguments are not valid. Arguments should be input like this: <Option> Image File.bmp Text File.txt Output.bmp\n");
        exit(EXIT_FAILURE);
    }
    char *option = argv[1];
    char *imageFile = argv[2];
    char *textFile = argv[3];
    char *outputFile = argv[4];
    if (verify_extension(imageFile, textFile) == 1)
    {
        if (is24_bit(imageFile) == 24)
        {
            if (bigEnough(imageFile, textFile) == 1)
            {
                if (strcmp(option, "encode") == 0)
                {
                    encode(imageFile, textFile, outputFile);
                }
                else if (strcmp(option, "decode") == 0)
                {
                    decode(outputFile);
                }
                else
                {
                    printf("Invalid option. Use 'encode' or 'decode'.\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printf("Image is not big enough to contain the text.\n");
            }
        }
        else
        {
            printf("Input image is not in 24-bit format.\n");
        }
    }
    else
    {
        printf("Argument extensions are not correct.\n");
    }
    return 0;
}
