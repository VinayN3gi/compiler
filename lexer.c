#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "lexer.h"

// ---- Reserved Keywords ----
char reserved_keywords[][20] = {
    "program", "var", "integer", "begin", "end",
    "if", "then", "else", "while", "do",
    "read", "write", "and", "or", "not",
    "array", "function", "procedure", "of"
};

int isKeyword(char *word)
{
    char temp[100];
    strncpy(temp, word, 99);
    temp[99] = '\0';

    // Case-insensitive comparison
    for(int j = 0; j < (int)strlen(temp); j++)
        temp[j] = tolower(temp[j]);

    int size = sizeof(reserved_keywords) / sizeof(reserved_keywords[0]);

    for(int i = 0; i < size; i++)
    {
        if(strcmp(reserved_keywords[i], temp) == 0)
            return 1;
    }
    return 0;
}

int runLexer()
{
    FILE *fp = fopen("input.txt", "r");

    if(fp == NULL)
    {
        printf("Error: input.txt not found\n");
        return 1;
    }

    char ch;
    char buffer[256];
    int line = 1;   // FIX: start at 1, not 0
    int index = 0;

    while((ch = fgetc(fp)) != EOF)
    {
        // ---- Newline ----
        if(ch == '\n')
        {
            line++;
            continue;
        }

        // ---- Whitespace ----
        else if(ch == ' ' || ch == '\t')
            continue;

        // ---- Comment: starts with ! ----
        else if(ch == '!')
        {
            while(ch != '\n' && ch != EOF)
                ch = fgetc(fp);
            line++;
        }

        // ---- Identifier or Keyword ----
        else if(isalpha(ch))
        {
            index = 0;

            while(isalnum(ch))
            {
                // FIX: enforce 32-char identifier limit
                if(index < 32)
                    buffer[index] = ch;
                index++;
                ch = fgetc(fp);
            }

            // Clamp stored length to 32
            int storeLen = index < 32 ? index : 32;
            buffer[storeLen] = '\0';
            index = 0;

            if(isKeyword(buffer) == 1)
                insert("KEYWORD", buffer, line);
            else
                insert("IDENTIFIER", buffer, line);

            fseek(fp, -1, SEEK_CUR);
        }

        // ---- Numbers ----
        else if(isdigit(ch))
        {
            index = 0;
            buffer[index++] = ch;

            while(isdigit(ch = fgetc(fp)))
                buffer[index++] = ch;

            // FIX: detect invalid number like 123abc
            if(isalpha(ch))
            {
                buffer[index] = '\0';
                printf("Lexical Error at line %d: Invalid number format '%s'\n", line, buffer);
            }
            else
            {
                buffer[index] = '\0';
                ungetc(ch, fp);
                insert("NUMBER", buffer, line);
            }
        }

        // ---- String Literal ----
        else if(ch == '\'')
        {
            index = 0;
            buffer[index++] = '\'';

            while((ch = fgetc(fp)) != '\'')
            {
                if(ch == '\n' || ch == EOF)
                {
                    printf("Lexical Error at line %d: Unterminated string\n", line);
                    break;
                }

                // FIX: handle escape sequences \n \t \'
                if(ch == '\\')
                {
                    char next = fgetc(fp);
                    if(next == 'n')       buffer[index++] = '\n';
                    else if(next == 't')  buffer[index++] = '\t';
                    else                  buffer[index++] = next; // handles \' and others
                }
                else
                {
                    buffer[index++] = ch;
                }
            }

            buffer[index++] = '\'';
            buffer[index]   = '\0';
            index = 0;

            insert("STRING", buffer, line);
        }

        // ---- := or : ----
        else if(ch == ':')
        {
            char next = fgetc(fp);
            if(next == '=')
                insert("ASSIGN", ":=", line);
            else
            {
                insert("COLON", ":", line);
                fseek(fp, -1, SEEK_CUR);
            }
        }

        // ---- <, <=, <> ----
        else if(ch == '<')
        {
            char next = fgetc(fp);
            if(next == '=')
                insert("LE", "<=", line);
            else if(next == '>')
                insert("NE", "<>", line);
            else
            {
                insert("LT", "<", line);
                fseek(fp, -1, SEEK_CUR);
            }
        }

        // ---- >, >= ----
        else if(ch == '>')
        {
            char next = fgetc(fp);
            if(next == '=')
                insert("GE", ">=", line);
            else
            {
                insert("GT", ">", line);
                fseek(fp, -1, SEEK_CUR);
            }
        }

        // FIX: = sign was never handled
        else if(ch == '=')
            insert("EQ", "=", line);

        // ---- Arithmetic ----
        else if(ch == '+') insert("PLUS",     "+", line);
        else if(ch == '-') insert("MINUS",    "-", line);
        else if(ch == '*') insert("MULTIPLY", "*", line);
        else if(ch == '/') insert("DIVIDE",   "/", line);

        // ---- Punctuation ----
        else if(ch == ';') insert("SEMICOLON", ";",  line);
        else if(ch == ',') insert("COMMA",     ",",  line);  // FIX: was missing
        else if(ch == '.') insert("DOT",       ".",  line);
        else if(ch == '(') insert("LPAREN",    "(",  line);
        else if(ch == ')') insert("RPAREN",    ")",  line);
        else if(ch == '[') insert("LBRACKET",  "[",  line);
        else if(ch == ']') insert("RBRACKET",  "]",  line);

        // ---- Unknown ----
        else
        {
            printf("Lexical Error at line %d: Unknown character '%c'\n", line, ch);
        }
    }

    fclose(fp);
    return 0;
}