#include <stdio.h>
#include <string.h>
#include "icg.h"
#include "codegen.h"

// Simple register pool: R0–R3, round-robin
// For academic purposes this avoids runaway R100 register numbers
static int regPool = 0;
#define NUM_REGS 4

static int nextReg()
{
    int r = regPool;
    regPool = (regPool + 1) % NUM_REGS;
    return r;
}

void generateTargetCode()
{
    printf("\n--- TARGET CODE ---\n");

    for(int i = 0; i < icgIndex; i++)
    {
        char lhs[64], op1[64], op[16], op2[64];

        // ---- Pattern 1: lhs = op1 OP op2  (4-token) ----
        if(sscanf(icg[i], "%63s = %63s %15s %63s", lhs, op1, op, op2) == 4)
        {
            int reg = nextReg();

            printf("MOV R%d, %s\n", reg, op1);

            // Arithmetic
            if     (strcmp(op, "+") == 0) printf("ADD R%d, %s\n", reg, op2);
            else if(strcmp(op, "-") == 0) printf("SUB R%d, %s\n", reg, op2);
            else if(strcmp(op, "*") == 0) printf("MUL R%d, %s\n", reg, op2);
            else if(strcmp(op, "/") == 0) printf("DIV R%d, %s\n", reg, op2);

            // FIX: relational operators — use CMP + SET
            else if(strcmp(op, ">")  == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETG R%d\n",  reg); }
            else if(strcmp(op, "<")  == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETL R%d\n",  reg); }
            else if(strcmp(op, ">=") == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETGE R%d\n", reg); }
            else if(strcmp(op, "<=") == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETLE R%d\n", reg); }
            else if(strcmp(op, "<>") == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETNE R%d\n", reg); }
            else if(strcmp(op, "=")  == 0) { printf("CMP R%d, %s\n", reg, op2); printf("SETE R%d\n",  reg); }

            printf("MOV %s, R%d\n\n", lhs, reg);
        }

        // ---- Pattern 2: lhs = op1  (simple copy / assignment) ----
        else if(sscanf(icg[i], "%63s = %63s", lhs, op1) == 2)
        {
            printf("MOV %s, %s\n\n", lhs, op1);
        }

        // ---- Pattern 3: READ var ----
        else if(strncmp(icg[i], "READ ", 5) == 0)
        {
            char var[64];
            sscanf(icg[i], "READ %63s", var);
            printf("IN %s\n\n", var);  // IN instruction reads from stdin into var
        }

        // ---- Pattern 4: WRITE var/string ----
        else if(strncmp(icg[i], "WRITE ", 6) == 0)
        {
            char var[128];
            sscanf(icg[i], "WRITE %127[^\n]", var);
            printf("OUT %s\n\n", var);  // OUT instruction outputs var
        }

        // ---- Pattern 5: IF NOT cond GOTO label ----
        else if(strncmp(icg[i], "IF NOT", 6) == 0)
        {
            char cond[64], label[32];
            sscanf(icg[i], "IF NOT %63s GOTO %31s", cond, label);
            printf("CMP %s, 0\n", cond);
            printf("JE %s\n\n", label);  // jump if condition == 0 (false)
        }

        // ---- Pattern 6: IF cond GOTO label ----
        else if(strncmp(icg[i], "IF ", 3) == 0)
        {
            char cond[64], label[32];
            sscanf(icg[i], "IF %63s GOTO %31s", cond, label);
            printf("CMP %s, 0\n", cond);
            printf("JNE %s\n\n", label); // jump if condition != 0 (true)
        }

        // ---- Pattern 7: GOTO label ----
        else if(strncmp(icg[i], "GOTO ", 5) == 0)
        {
            char label[32];
            sscanf(icg[i], "GOTO %31s", label);
            printf("JMP %s\n\n", label);
        }

        // ---- Pattern 8: label: ----
        else if(icg[i][strlen(icg[i]) - 1] == ':')
        {
            printf("%s\n", icg[i]);
        }

        // ---- Unknown / passthrough ----
        else
        {
            printf("; [unhandled] %s\n", icg[i]);
        }
    }

    printf("\n--- END OF TARGET CODE ---\n");
}