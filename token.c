#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

// Define the head pointer here (extern declared in token.h)
Token *head = NULL;
Token *tail = NULL;

void insert(char *type, char *value, int line)
{
    Token *newToken = (Token*)malloc(sizeof(Token));

    strncpy(newToken->type,  type,  99);  newToken->type[99]  = '\0';
    strncpy(newToken->value, value, 99);  newToken->value[99] = '\0';
    newToken->line = line;
    newToken->next = NULL;

    if(head == NULL)
    {
        head = newToken;
        tail = newToken;
    }
    else
    {
        tail->next = newToken;
        tail = newToken;
    }
}

void deleteToken(char *value)
{
    Token *prev = NULL;
    Token *curr = head;

    while(curr != NULL)
    {
        if(strcmp(curr->value, value) == 0)
        {
            if(prev == NULL)
                head = curr->next;
            else
                prev->next = curr->next;

            if(curr == tail)
                tail = prev;

            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void display()
{
    Token *curr = head;
    printf("\n--- TOKEN LIST ---\n");
    while(curr != NULL)
    {
        printf("(%s, %s, line %d)\n", curr->type, curr->value, curr->line);
        curr = curr->next;
    }
    printf("------------------\n\n");
}