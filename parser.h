#ifndef PARSER_H
#define PARSER_H

#include "token.h"

extern Token *current;

void advance();
void match(char *type);
void error(char *msg);
void parseProgram();
void parseAssignment();
#endif