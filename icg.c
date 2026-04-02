#include <stdio.h>
#include <string.h>
#include "icg.h"

// FIX: increased sizes — 500 instructions, 256 chars each
char icg[500][256];
int icgIndex = 0;

void emit(char *code)
{
    if(icgIndex < 500)
    {
        strncpy(icg[icgIndex], code, 255);
        icg[icgIndex][255] = '\0';
        icgIndex++;
    }
    else
    {
        printf("ICG Error: Too many instructions (limit 500)\n");
    }
}

void printICG()
{
    printf("\n--- INTERMEDIATE CODE ---\n");
    for(int i = 0; i < icgIndex; i++)
        printf("%3d: %s\n", i + 1, icg[i]);
    printf("-------------------------\n");
}