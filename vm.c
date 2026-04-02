#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icg.h"
#include "vm.h"

// ---- Variable Store ----
// Stores up to 64 variables, each with a name and integer value

#define MAX_VARS 64

typedef struct {
    char name[64];
    int  value;
} Variable;

static Variable vars[MAX_VARS];
static int varCount = 0;

// Get variable value by name
static int getVar(char *name)
{
    // If it's a plain integer literal, return it directly
    // (handles cases like t1 = 5 + x where 5 is an operand)
    int isNum = 1;
    int start = 0;
    if(name[0] == '-') start = 1; // allow negative
    for(int i = start; i < (int)strlen(name); i++)
    {
        if(name[i] < '0' || name[i] > '9') { isNum = 0; break; }
    }
    if(isNum) return atoi(name);

    for(int i = 0; i < varCount; i++)
        if(strcmp(vars[i].name, name) == 0)
            return vars[i].value;

    // Variable not found — initialise to 0
    strncpy(vars[varCount].name, name, 63);
    vars[varCount].value = 0;
    varCount++;
    return 0;
}

// Set variable value by name
static void setVar(char *name, int value)
{
    for(int i = 0; i < varCount; i++)
    {
        if(strcmp(vars[i].name, name) == 0)
        {
            vars[i].value = value;
            return;
        }
    }
    // New variable
    strncpy(vars[varCount].name, name, 63);
    vars[varCount].value = value;
    varCount++;
}

// Find the index of a label like "L3:"
static int findLabel(char *label)
{
    char withColon[32];
    snprintf(withColon, sizeof(withColon), "%s:", label);

    for(int i = 0; i < icgIndex; i++)
    {
        if(strcmp(icg[i], withColon) == 0)
            return i;
    }
    return -1; // label not found
}

// ---- Main VM Execution ----

void runVM()
{
    printf("\n--- PROGRAM OUTPUT ---\n");

    int pc = 0; // program counter — index into icg[]

    while(pc < icgIndex)
    {
        char *instr = icg[pc];

        char lhs[64], op1[64], op[16], op2[64];

        // ---- READ var ----
        if(strncmp(instr, "READ ", 5) == 0)
        {
            char var[64];
            sscanf(instr, "READ %63s", var);
            int val;
            printf("Enter value for %s: ", var);
            scanf("%d", &val);
            setVar(var, val);
            pc++;
        }

        // ---- WRITE string literal ----
        else if(strncmp(instr, "WRITE '", 7) == 0)
        {
            // Extract content between the quotes
            char *start = strchr(instr, '\'');
            char *end   = strrchr(instr, '\'');

            if(start && end && start != end)
            {
                // print characters between first and last quote
                for(char *p = start + 1; p < end; p++)
                    putchar(*p);
                putchar('\n');
            }
            pc++;
        }

        // ---- WRITE expr ----
        else if(strncmp(instr, "WRITE ", 6) == 0)
        {
            char var[64];
            sscanf(instr, "WRITE %63s", var);
            printf("%d\n", getVar(var));
            pc++;
        }

        // ---- lhs = op1 OP op2  (4-token arithmetic/relational) ----
        else if(sscanf(instr, "%63s = %63s %15s %63s", lhs, op1, op, op2) == 4)
        {
            int v1 = getVar(op1);
            int v2 = getVar(op2);
            int result = 0;

            if     (strcmp(op, "+")  == 0) result = v1 + v2;
            else if(strcmp(op, "-")  == 0) result = v1 - v2;
            else if(strcmp(op, "*")  == 0) result = v1 * v2;
            else if(strcmp(op, "/")  == 0) result = (v2 != 0) ? v1 / v2 : 0;
            else if(strcmp(op, ">")  == 0) result = v1 >  v2;
            else if(strcmp(op, "<")  == 0) result = v1 <  v2;
            else if(strcmp(op, ">=") == 0) result = v1 >= v2;
            else if(strcmp(op, "<=") == 0) result = v1 <= v2;
            else if(strcmp(op, "<>") == 0) result = v1 != v2;
            else if(strcmp(op, "=")  == 0) result = v1 == v2;

            setVar(lhs, result);
            pc++;
        }

        // ---- lhs = op1  (simple copy) ----
        else if(sscanf(instr, "%63s = %63s", lhs, op1) == 2)
        {
            setVar(lhs, getVar(op1));
            pc++;
        }

        // ---- IF NOT cond GOTO label ----
        else if(strncmp(instr, "IF NOT ", 7) == 0)
        {
            char cond[64], label[32];
            sscanf(instr, "IF NOT %63s GOTO %31s", cond, label);

            if(getVar(cond) == 0) // condition is FALSE
            {
                int target = findLabel(label);
                if(target != -1) pc = target + 1;
                else             pc++;
            }
            else
                pc++;
        }

        // ---- IF cond GOTO label ----
        else if(strncmp(instr, "IF ", 3) == 0)
        {
            char cond[64], label[32];
            sscanf(instr, "IF %63s GOTO %31s", cond, label);

            if(getVar(cond) != 0) // condition is TRUE
            {
                int target = findLabel(label);
                if(target != -1) pc = target + 1;
                else             pc++;
            }
            else
                pc++;
        }

        // ---- GOTO label ----
        else if(strncmp(instr, "GOTO ", 5) == 0)
        {
            char label[32];
            sscanf(instr, "GOTO %31s", label);

            int target = findLabel(label);
            if(target != -1) pc = target + 1;
            else             pc++;
        }

        // ---- Label definition e.g. "L1:" — skip ----
        else if(instr[strlen(instr) - 1] == ':')
        {
            pc++;
        }

        // ---- Unknown — skip ----
        else
        {
            pc++;
        }
    }

    printf("--- END OF OUTPUT ---\n");
}