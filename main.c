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

    printf("\nRunning Parser + ICG...\n");

    parseProgram();

    return 0;
}