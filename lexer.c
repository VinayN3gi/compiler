#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "token.h"

char reserved_keywords[][20] = {
    "program",
    "var",
    "integer",
    "begin",
    "end",
    "if",
    "then",
    "else",
    "while",
    "do",
    "read",
    "write"
};


enum TokenType {
    // Identifiers,keywords & literals
    IDENTIFIER,
    NUMBER,
    STRING,
    KEYWORD,


    // Arithmetic operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    // Relational operators
    GT,     // >
    LT,     // <
    GE,     // >=
    LE,     // <=
    EQ,     // =
    NE,     // <>

    // Assignment
    ASSIGN, // :=

    // Punctuation
    LPAREN, RPAREN,
    LBRACKET, RBRACKET,
    SEMICOLON,
    COMMA,
    COLON,
    DOT
};

int isKeyword(char *word)
{
    char temp[100];
    strcpy(temp, word);

    for(int j = 0; j < strlen(temp); j++)
        temp[j] = tolower(temp[j]);

    int size = sizeof(reserved_keywords)/sizeof(reserved_keywords[0]);

    for(int i = 0; i < size; i++)
    {
        if(strcmp(reserved_keywords[i], temp) == 0)
            return 1;
    }
    return 0;
}



int runLexer()
{
    FILE *fp=fopen("input.txt","r");
    
    if(fp==NULL)
    {
        printf("File not found \n");
        return 1;
    }

    char ch;
    char buffer[100];
    int line=0;
    int index=0;
    while((ch=fgetc(fp))!=EOF)
    {
        if(ch=='\n') //new line 
        {
            line++;
            continue;
        }

        else if(ch==' ' || ch=='\t') continue; //skip space
        
        else if(isalpha(ch)) // scan for keyword or identifier
        {
            index = 0;
            while(isalnum(ch))
            {
                buffer[index++] = ch;
                ch = fgetc(fp);
            }
            buffer[index] = '\0';
            index=0;

            if(isKeyword(buffer)==1) insert("KEYWORD",buffer,line);
            else insert("IDENTIFIER",buffer,line);
            fseek(fp,-1,SEEK_CUR);
        }

        else if(ch==':')
        {
            char next=fgetc(fp);
            if(next=='=') insert("ASSIGN",":=",line);
            else 
            {
                insert("COLON",":",line);
                fseek(fp,-1,SEEK_CUR);
            }
        }

        else if(ch=='<') // check for <> , <= , <  
        {
            char next=fgetc(fp);
            if(next=='=')
            {
                insert("LE","<=",line);
            }
            else if(next=='>')
            {
                insert("NE","<>",line);
            } 
            else 
            {
                insert("LT","<",line);
                fseek(fp,-1,SEEK_CUR);
            }
        }

        else if(ch=='>') //check for > and >=
        {
            char next=fgetc(fp);
            if(next=='=')
            {
                insert("GE",">=",line);
            }
            else 
            {
                insert("GT",">",line);
                fseek(fp,-1,SEEK_CUR);
            }
        }

        else if(ch==';')
        {
            insert("SEMICOLON",";",line);
        }

        // Parenthesis
        else if(ch=='(')
            insert("LPAREN","(",line);
        else if(ch==')')
            insert("RPAREN",")",line);
         else if(ch=='[')
            insert("LBRACKET","[",line);
        else if(ch==']')
            insert("RBRACKET","]",line);


        else if(ch == '\'')
        {
            buffer[index++] = ch;

            while((ch = fgetc(fp)) != '\'' )
            {
                if(ch == '\n' || ch == EOF)
                {
                    printf("Unterminated string at line %d\n", line);
                    break;
                }

                buffer[index++] = ch;
            }

            buffer[index++] = '\'';
            buffer[index] = '\0';
            index = 0;

            insert("STRING", buffer, line);
        }

        else if(ch == '+') insert("PLUS", "+", line);
        else if(ch == '-') insert("MINUS", "-", line);
        else if(ch == '*') insert("MULTIPLY", "*", line);
        else if(ch == '/') insert("DIVIDE", "/", line);
        
        else if(ch == '!')
        {
            while(ch != '\n' && ch != EOF)
                ch = fgetc(fp);

            line++;
        }

        else if(isdigit(ch))
        {
            index = 0;
            buffer[index++] = ch;

            while(isdigit(ch = fgetc(fp)))
                buffer[index++] = ch;

            if(isalpha(ch))
            {
                printf("Lexical Error at line %d: Invalid number format\n", line);
            }

            buffer[index] = '\0';
            ungetc(ch, fp);

            insert("NUMBER", buffer, line);
        }

        else if(ch == '.')
        {
            insert("DOT", ".", line);
        }
            
        else
        {
            printf("Lexical Error at line %d : Unknown character %c\n",line,ch);
        }
    }
}