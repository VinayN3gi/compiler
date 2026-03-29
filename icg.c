#include <stdio.h>
#include <string.h>

char icg[200][100];
int icgIndex = 0;

void emit(char *code)
{
    strcpy(icg[icgIndex++], code);
    printf("%s\n", code);
}

void printICG()
{
    printf("\n--- INTERMEDIATE CODE ---\n");

    for(int i = 0; i < icgIndex; i++)
        printf("%d: %s\n", i+1, icg[i]);
}