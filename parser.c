#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "icg.h"

Token *current;

// -------- TEMP + LABEL --------
int tempCount = 1;
int labelCount = 1;

char* newTemp()
{
    char *temp = (char*)malloc(10);
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

char* newLabel()
{
    char *label = (char*)malloc(10);
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
    printf("Syntax Error at line %d: %s\n", current->line, msg);
    errorFlag = 1;
    advance(); // continue parsing
}

void match(char *type)
{
    if(current && strcmp(current->type, type) == 0)
        advance();
    else
        error("Unexpected token");
}

// -------- EXPRESSIONS --------

char* parseFactor()
{
    char *val;

    if(strcmp(current->type,"IDENTIFIER")==0 ||
       strcmp(current->type,"NUMBER")==0)
    {
        val = strdup(current->value);
        advance();
        return val;
    }

    else if(strcmp(current->type,"LPAREN")==0)
    {
        match("LPAREN");
        val = parseExpression();
        match("RPAREN");
        return val;
    }

    error("Invalid factor");
    return NULL;
}

char* parseTerm()
{
    char *left = parseFactor();

    while(current &&
         (strcmp(current->type,"MULTIPLY")==0 ||
          strcmp(current->type,"DIVIDE")==0))
    {
        char op[5];
        strcpy(op, current->value);
        advance();

        char *right = parseFactor();

        char *temp = newTemp();

        char code[100];
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
         (strcmp(current->type,"PLUS")==0 ||
          strcmp(current->type,"MINUS")==0))
    {
        char op[5];
        strcpy(op, current->value);
        advance();

        char *right = parseTerm();

        char *temp = newTemp();

        char code[100];
        sprintf(code, "%s = %s %s %s", temp, left, op, right);
        emit(code);

        left = temp;
    }

    // relational operators
    if(current &&
      (strcmp(current->type,"GT")==0 ||
       strcmp(current->type,"LT")==0 ||
       strcmp(current->type,"GE")==0 ||
       strcmp(current->type,"LE")==0 ||
       strcmp(current->type,"EQ")==0 ||
       strcmp(current->type,"NE")==0))
    {
        char op[5];
        strcpy(op, current->value);
        advance();

        char *right = parseExpression();

        char *temp = newTemp();

        char code[100];
        sprintf(code, "%s = %s %s %s", temp, left, op, right);
        emit(code);

        return temp;
    }

    return left;
}

// -------- STATEMENTS --------

void parseAssignment()
{
    char lhs[50];
    strcpy(lhs, current->value);

    match("IDENTIFIER");
    match("ASSIGN");

    char *rhs = parseExpression();

    char code[100];
    sprintf(code, "%s = %s", lhs, rhs);
    emit(code);
}

void parseRead()
{
    match("KEYWORD"); // read
    match("LPAREN");
    match("IDENTIFIER");
    match("RPAREN");
}

void parseWrite()
{
    match("KEYWORD"); // write
    match("LPAREN");

    if(strcmp(current->type,"STRING")==0)
        match("STRING");
    else
        parseExpression();

    match("RPAREN");
}

void parseIf()
{
    match("KEYWORD"); // if
    char *cond = parseExpression();

    char *L1 = newLabel();
    char *L2 = newLabel();

    char code[100];
    sprintf(code, "IF %s GOTO %s", cond, L1);
    emit(code);

    match("KEYWORD"); // then
    parseStatement();

    if(current && strcmp(current->type,"SEMICOLON")==0)
        advance();

    if(current && strcmp(current->value,"else")==0)
    {
        sprintf(code, "GOTO %s", L2);
        emit(code);

        sprintf(code, "%s:", L1);
        emit(code);

        match("KEYWORD"); // else
        parseStatement();

        sprintf(code, "%s:", L2);
        emit(code);
    }
    else
    {
        sprintf(code, "%s:", L1);
        emit(code);
    }
}

void parseWhile()
{
    char *Lstart = newLabel();
    char *Lend = newLabel();

    char code[100];

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
    if(strcmp(current->type,"IDENTIFIER")==0)
        parseAssignment();

    else if(strcmp(current->value,"read")==0)
        parseRead();

    else if(strcmp(current->value,"write")==0)
        parseWrite();

    else if(strcmp(current->value,"if")==0)
        parseIf();

    else if(strcmp(current->value,"while")==0)
        parseWhile();

    else
        error("Invalid statement");
}

// -------- DECLARATION --------

void parseDeclaration()
{
    match("KEYWORD"); // var
    match("IDENTIFIER");
    match("COLON");
    match("KEYWORD"); // integer
    match("SEMICOLON");
}

// -------- BLOCK --------

void parseBlock()
{
    match("KEYWORD"); // begin

    while(current && strcmp(current->value,"end") != 0)
    {
        parseStatement();

        if(current && strcmp(current->type,"SEMICOLON")==0)
            match("SEMICOLON");
    }

    match("KEYWORD"); // end
    match("DOT");
}

// -------- PROGRAM --------

void parseProgram()
{
    match("KEYWORD"); // program
    match("IDENTIFIER");
    match("SEMICOLON");

    while(current && strcmp(current->value,"var")==0)
        parseDeclaration();

    parseBlock();

    printf("\nProgram parsed successfully ✅\n");
}