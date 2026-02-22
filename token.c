#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

Token *head = NULL;

void insert(char *type, char *value, int line)
{
    Token *temp = (Token*) malloc(sizeof(Token));
    strcpy(temp->type, type);
    strcpy(temp->value, value);
    temp->line = line;
    temp->next = NULL;

    if(head == NULL)
        head = temp;
    else {
        Token *check = head;
        while(check->next != NULL)
            check = check->next;
        check->next = temp;
    }
}

void deleteToken(char *value)
{
    Token *temp = head;
    Token *prev = NULL;

    while(temp != NULL)
    {
        if(strcmp(temp->value, value) == 0)
        {
            if(prev == NULL)
                head = temp->next;
            else
                prev->next = temp->next;

            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void display()
{
    Token *temp = head;
    while(temp != NULL)
    {
        printf("(%s, %s, line %d)\n",
               temp->type,
               temp->value,
               temp->line);
        temp = temp->next;
    }
}