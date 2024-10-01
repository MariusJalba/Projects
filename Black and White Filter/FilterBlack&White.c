#include <stdio.h>
#include <stdlib.h>
void bmp(char *dfin, char *dfout)
{
    FILE *fin;
    FILE *fout2;
    int n[55] = {0};
    int width = 0;
    int height = 0;
    char pixel[3] = {0};
    if ((fin = fopen(dfin, "rb")) == NULL)
    {
        printf("Eroare deschidere fisier intrare");
    }
    if ((fout2 = fopen(dfout, "wb")) == NULL)
    {
        printf("Eroare deschidere fisier de iesire");
    }
    
    fseek(fin, 0, SEEK_SET);
    fseek(fin, 0x12, SEEK_SET);
    fread(&width, sizeof(int), 1, fin);
    fread(&height, sizeof(int), 1, fin);
    fseek(fin, 0, SEEK_SET);
    fread(&n, sizeof(char), 54, fin);
    fwrite(&n, sizeof(char), 54, fout2);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // read RGB values from input file
            fread(pixel, sizeof(unsigned char), 3, fin);

            // calculate grayscale value
            unsigned char gray_value = (unsigned char)(0.23 * pixel[2] + 0.58 * pixel[1] + 0.114 * pixel[0]);

            // set all color channels to the grayscale value
            pixel[0] = gray_value;
            pixel[1] = gray_value;
            pixel[2] = gray_value;
            fwrite(pixel,sizeof(char),3,fout2);
        }
    }
    fclose(fin);
    fclose(fout2);

}
void scriere(char *dfin, char *dfout1, char *dfout2)
{
    char c[3] = {"a"};
    int n = 0;
    FILE *fin;
    FILE *fout1;
    if ((fin = fopen(dfin, "rb")) == NULL)
    {
        printf("Eroare deschidere fisier intrare");
    }
    if ((fout1 = fopen(dfout1, "w")) == NULL)
    {
        printf("Eroare deschidere fisier de iesire");
    }
    fread(c, sizeof(char), 2, fin);
    fprintf(fout1, "%s", c);
    fputc('\n', fout1);
    fprintf(fout1, "dimensiune fisier:");
    fread(&n, sizeof(int), 1, fin);
    fprintf(fout1, "%d bytes\n", n);
    fseek(fin, 0x12, SEEK_SET);
    fread(&n, sizeof(int), 1, fin);
    fprintf(fout1, "Resolution: %dx", n);
    fread(&n, sizeof(int), 1, fin);
    fprintf(fout1, "%d", n);
    fseek(fin, 0, SEEK_SET);
    fseek(fin, 0x0A, SEEK_SET);
    fclose(fin);
    fclose(fout1);
}
int main(int arcg, char **arcgv)
{
    if (arcg != 4)
    {
        printf("Nr incorect de argumente");
        exit(EXIT_FAILURE);
    }
    scriere(arcgv[1], arcgv[2], arcgv[3]);
    bmp(arcgv[1],arcgv[3]);   
    return 0;
}