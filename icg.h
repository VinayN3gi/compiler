#ifndef ICG_H
#define ICG_H

// FIX: increased buffer sizes to prevent overflow
extern char icg[500][256];
extern int icgIndex;

void emit(char *code);
void printICG();

#endif