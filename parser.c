#include <stdio.h>
#include <string.h>
#include "parser.h"

// ICG function
void generateAssign(char *lhs, char *rhs);

Token *current;

void advance()
{
    if(current != NULL)
        current = current->next;
}

void error(char *msg)
{
    printf("Syntax Error at line %d: %s\n", current->line, msg);
}

void match(char *type)
{
    if(current != NULL && strcmp(current->type, type) == 0)
        advance();
    else
        error("Unexpected token");
}

void parseAssignment()
{
    char lhs[50];
    strcpy(lhs, current->value);

    match("IDENTIFIER");
    match("ASSIGN");

    if(strcmp(current->type, "NUMBER") == 0 ||
       strcmp(current->type, "IDENTIFIER") == 0)
    {
        char rhs[50];
        strcpy(rhs, current->value);

        advance();

        generateAssign(lhs, rhs);   // ✅ correct place
    }
    else
    {
        error("Invalid expression");
    }
}

void parseProgram()
{
    if(current != NULL && strcmp(current->value, "program") == 0)
    {
        advance();
        match("IDENTIFIER");
        match("SEMICOLON");

        printf("Program parsed successfully\n");
    }
    else
    {
        error("Expected 'program'");
    }
}