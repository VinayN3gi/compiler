#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "icg.h"
#include "codegen.h"
#include "vm.h"

extern int errorFlag;

int main()
{
    printf("=== COMPILER START ===\n\n");

    // ---- Phase 1: Lexical Analysis ----
    printf("--- Running Lexer ---\n");
    int lexResult = runLexer();

    if(lexResult != 0)
    {
        printf("Lexer failed. Exiting.\n");
        return 1;
    }

    display();

    // ---- Phase 2: Parsing + ICG ----
    current = head;

    if(current == NULL)
    {
        printf("No tokens found. Exiting.\n");
        return 1;
    }

    printf("--- Running Parser + ICG ---\n");
    parseProgram();

    // ---- Phase 3: Print ICG ----
    printICG();

    if(errorFlag != 0)
    {
        printf("\nErrors found. Skipping code generation and execution.\n");
        return 1;
    }

    // ---- Phase 4: Target Code Generation ----
    generateTargetCode();

    // ---- Phase 5: Execute via VM ----
    runVM();

    printf("\n=== COMPILER DONE ===\n");
    return 0;
}