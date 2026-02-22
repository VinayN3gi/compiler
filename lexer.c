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
    int size=sizeof(reserved_keywords)/sizeof(reserved_keywords[0]);
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<strlen(word);j++) 
        {
            char temp=tolower(word[j]);
            word[j]=temp;
        }

        for(int i=0;i<size;i++)
        {
            if(strcmp(reserved_keywords[i],word)==0) return 1;
        }
    }
    return 0;
}



int main()
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
            buffer[index++]=ch;
            while(isalnum(ch))
            {
                buffer[index++]=ch;
                ch=fgetc(fp);
            }
            buffer[index]='\0';
            index=0;

            if(isKeyword(buffer)==1) insert("KEYWORD",buffer,line);
            else insert("INDETIFIER",buffer,line);
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


        //comment , numbers , sting and arithmatic operator remainging
    
        else
        {
            printf("Lexical Error at line %d : Unknown character %c\n",line,ch);
        }
    }

    display();

}