#include <stdio.h>
#include <string.h>
#include "token.h"
#include "parser.h"

int runLexer();

int main()
{
    printf("Running Lexer...\n");

    runLexer();
    display();

    current = head;

    printf("\nRunning Parser...\n");

    parseProgram();

    while(current != NULL)
    {
        if(strcmp(current->type, "IDENTIFIER") == 0)
            parseAssignment();
        else
            advance();
    }

    return 0;
}