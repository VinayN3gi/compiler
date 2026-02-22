#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
    char type[100];
    char value[100];
    int line;
    struct Token *next;
} Token;

// Head pointer (extern because defined in token.c)
extern Token *head;

// Function declarations
void insert(char *type, char *value, int line);
void deleteToken(char *value);
void display();

#endif