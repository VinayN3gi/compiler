#ifndef PARSER_H
#define PARSER_H

#include "token.h"

extern Token *current;

void advance();
void match(char *type);
void error(char *msg);

void parseProgram();
void parseDeclaration();
void parseBlock();
void parseStatement();

void parseAssignment();
void parseIf();
void parseWhile();
void parseRead();
void parseWrite();

char* parseExpression();
char* parseTerm();
char* parseFactor();

#endif