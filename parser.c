#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "icg.h"

Token *current;

// -------- TEMP + LABEL --------
int tempCount  = 1;
int labelCount = 1;

char* newTemp()
{
    char *temp = (char*)malloc(16);
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

char* newLabel()
{
    char *label = (char*)malloc(16);
    sprintf(label, "L%d", labelCount++);
    return label;
}

// -------- BASIC --------

void advance()
{
    if(current != NULL)
        current = current->next;
}

int errorFlag = 0;

void error(char *msg)
{
    if(current != NULL)
        printf("Syntax Error at line %d: %s (got '%s')\n", current->line, msg, current->value);
    else
        printf("Syntax Error: %s (unexpected end of input)\n", msg);

    errorFlag = 1;
    advance(); // error recovery: skip token and continue
}

void match(char *type)
{
    if(current && strcmp(current->type, type) == 0)
        advance();
    else
    {
        char msg[200];
        if(current)
            sprintf(msg, "Expected '%s' but got '%s'", type, current->value);
        else
            sprintf(msg, "Expected '%s' but reached end of input", type);
        error(msg);
    }
}

// -------- EXPRESSIONS --------

char* parseFactor()
{
    char *val;

    // FIX: NULL guard
    if(current == NULL) { error("Unexpected end of input in factor"); return strdup("?"); }

    if(strcmp(current->type, "IDENTIFIER") == 0 ||
       strcmp(current->type, "NUMBER")     == 0)
    {
        val = strdup(current->value);
        advance();
        return val;
    }
    else if(strcmp(current->type, "LPAREN") == 0)
    {
        match("LPAREN");
        val = parseExpression();
        match("RPAREN");
        return val;
    }

    error("Invalid factor");
    return strdup("?");
}

char* parseTerm()
{
    char *left = parseFactor();

    while(current &&
         (strcmp(current->type, "MULTIPLY") == 0 ||
          strcmp(current->type, "DIVIDE")   == 0))
    {
        char op[10];
        strncpy(op, current->value, 9); op[9] = '\0';
        advance();

        char *right = parseFactor();
        char *temp  = newTemp();

        char code[256];   // FIX: was 100
        sprintf(code, "%s = %s %s %s", temp, left, op, right);
        emit(code);

        left = temp;
    }

    return left;
}

char* parseExpression()
{
    char *left = parseTerm();

    while(current &&
         (strcmp(current->type, "PLUS")  == 0 ||
          strcmp(current->type, "MINUS") == 0))
    {
        char op[10];
        strncpy(op, current->value, 9); op[9] = '\0';
        advance();

        char *right = parseTerm();
        char *temp  = newTemp();

        char code[256];   // FIX: was 100
        sprintf(code, "%s = %s %s %s", temp, left, op, right);
        emit(code);

        left = temp;
    }

    // ---- Relational operators (single, non-chained) ----
    if(current &&
      (strcmp(current->type, "GT") == 0 ||
       strcmp(current->type, "LT") == 0 ||
       strcmp(current->type, "GE") == 0 ||
       strcmp(current->type, "LE") == 0 ||
       strcmp(current->type, "EQ") == 0 ||
       strcmp(current->type, "NE") == 0))
    {
        char op[10];
        strncpy(op, current->value, 9); op[9] = '\0';
        advance();

        char *right = parseTerm();  // FIX: was parseExpression() — avoids infinite recursion
        char *temp  = newTemp();

        char code[256];
        sprintf(code, "%s = %s %s %s", temp, left, op, right);
        emit(code);

        return temp;
    }

    return left;
}

// -------- STATEMENTS --------

void parseAssignment()
{
    char lhs[100];
    strncpy(lhs, current->value, 99); lhs[99] = '\0';

    match("IDENTIFIER");
    match("ASSIGN");

    char *rhs = parseExpression();

    char code[256];
    sprintf(code, "%s = %s", lhs, rhs);
    emit(code);
}

void parseRead()
{
    match("KEYWORD"); // read
    match("LPAREN");

    // FIX: emit READ instruction + support identifier list
    char code[256];

    char varName[100];
    strncpy(varName, current->value, 99); varName[99] = '\0';
    match("IDENTIFIER");

    sprintf(code, "READ %s", varName);
    emit(code);

    // Handle comma-separated identifier list: read(a, b, c)
    while(current && strcmp(current->type, "COMMA") == 0)
    {
        match("COMMA");
        strncpy(varName, current->value, 99); varName[99] = '\0';
        match("IDENTIFIER");

        sprintf(code, "READ %s", varName);
        emit(code);
    }

    match("RPAREN");
}

void parseWrite()
{
    match("KEYWORD"); // write
    match("LPAREN");

    char code[256];

    // FIX: emit WRITE instruction + support output list
    if(strcmp(current->type, "STRING") == 0)
    {
        sprintf(code, "WRITE %s", current->value);
        emit(code);
        match("STRING");
    }
    else
    {
        char *val = parseExpression();
        sprintf(code, "WRITE %s", val);
        emit(code);
    }

    // Handle comma-separated output list: write(a, b, 'str')
    while(current && strcmp(current->type, "COMMA") == 0)
    {
        match("COMMA");

        if(strcmp(current->type, "STRING") == 0)
        {
            sprintf(code, "WRITE %s", current->value);
            emit(code);
            match("STRING");
        }
        else
        {
            char *val = parseExpression();
            sprintf(code, "WRITE %s", val);
            emit(code);
        }
    }

    match("RPAREN");
}

void parseIf()
{
    match("KEYWORD"); // if
    char *cond = parseExpression();

    char *Lelse = newLabel();
    char *Lend  = newLabel();
    char code[256];

    // FIX: correct logic — if condition FALSE, jump to else
    sprintf(code, "IF NOT %s GOTO %s", cond, Lelse);
    emit(code);

    match("KEYWORD"); // then
    parseStatement();

    // FIX: only consume semicolon if next token is NOT 'else'
    // This prevents double-consumption with parseBlock
    if(current && strcmp(current->type, "SEMICOLON") == 0)
    {
        Token *next = current->next;
        if(next == NULL || strcmp(next->value, "else") != 0)
            advance(); // safe to consume — no else coming
        // if else IS coming, leave ; for parseBlock to consume
    }

    if(current && strcmp(current->value, "else") == 0)
    {
        sprintf(code, "GOTO %s", Lend);
        emit(code);

        sprintf(code, "%s:", Lelse);
        emit(code);

        match("KEYWORD"); // else
        parseStatement();

        sprintf(code, "%s:", Lend);
        emit(code);
    }
    else
    {
        // no else branch — Lelse is the exit point, Lend unused
        sprintf(code, "%s:", Lelse);
        emit(code);
    }
}

void parseWhile()
{
    char *Lstart = newLabel();
    char *Lend   = newLabel();
    char code[256];   // FIX: was 100

    sprintf(code, "%s:", Lstart);
    emit(code);

    match("KEYWORD"); // while
    char *cond = parseExpression();

    sprintf(code, "IF NOT %s GOTO %s", cond, Lend);
    emit(code);

    match("KEYWORD"); // do
    parseStatement();

    sprintf(code, "GOTO %s", Lstart);
    emit(code);

    sprintf(code, "%s:", Lend);
    emit(code);
}

void parseStatement()
{
    // FIX: NULL guard to prevent crash
    if(current == NULL)
    {
        error("Unexpected end of input");
        return;
    }

    if(strcmp(current->type, "IDENTIFIER") == 0)
        parseAssignment();

    else if(strcmp(current->value, "read")  == 0)
        parseRead();

    else if(strcmp(current->value, "write") == 0)
        parseWrite();

    else if(strcmp(current->value, "if")    == 0)
        parseIf();

    else if(strcmp(current->value, "while") == 0)
        parseWhile();

    else
        error("Invalid statement");
}

// -------- DECLARATION --------

void parseDeclaration()
{
    match("KEYWORD");    // var
    match("IDENTIFIER"); // variable name
    match("COLON");
    match("KEYWORD");    // integer
    match("SEMICOLON");
}

// -------- BLOCK --------

void parseBlock()
{
    match("KEYWORD"); // begin

    while(current && strcmp(current->value, "end") != 0)
    {
        parseStatement();

        if(current && strcmp(current->type, "SEMICOLON") == 0)
            match("SEMICOLON");
    }

    match("KEYWORD"); // end
    match("DOT");
}

// -------- PROGRAM --------

void parseProgram()
{
    match("KEYWORD");    // program
    match("IDENTIFIER"); // program name
    match("SEMICOLON");

    while(current && strcmp(current->value, "var") == 0)
        parseDeclaration();

    parseBlock();

    if(errorFlag == 0)
        printf("\nProgram parsed successfully\n");
    else
        printf("\nParsing completed with errors\n");
}