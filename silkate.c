#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TOKEN_IDENIFIER,
    TOKEN_INTEGER,
    TOKEN_STRING,

    TOKEN_INT,

    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    TOKEN_NEWLINE,
} TokenType;

typedef struct
{
    TokenType type;
    char value[64];
} Token;

void setstrchar(char *__restrict__ _str, char _chr)
{
    _str[0] = _chr;
    _str[1] = '\0';
}

int tokenize(const char *source)
{
    Token tokens[50];
    int tokenCount = 0;
    
    int i = 0;
    while (source[i] != '\0')
    {
        if (source[i] == '\n')
        {
            tokens[tokenCount].type = TOKEN_NEWLINE;
            setstrchar(tokens[tokenCount].value,'\n');
            tokenCount++;
            i++;
        }
        else if (source[i] == ' ' || source[i] == '\t')
        {
            i++;
        }
        else if (source[i] >= '0' && source[i] <= '9')
        {
            int start = i;
            while (source[i] >= '0' && source[i] <= '9')
            {
                i++;
            }
            int n = i - start;
            tokens[tokenCount].type = TOKEN_INTEGER;
            strncpy(tokens[tokenCount].value,source + start,n);
            tokens[tokenCount].value[n] = '\0';
            tokenCount++;
            i++;
        }
        else if (source[i] == '+')
        {
            tokens[tokenCount].type = TOKEN_PLUS;
            setstrchar(tokens[tokenCount].value,'+');
            tokenCount++;
            i++;
        }
        else if (source[i] == '-')
        {
            tokens[tokenCount].type = TOKEN_MINUS;
            setstrchar(tokens[tokenCount].value,'-');
            tokenCount++;
            i++;
        }
        else if (source[i] == '*')
        {
            tokens[tokenCount].type = TOKEN_STAR;
            setstrchar(tokens[tokenCount].value,'*');
            tokenCount++;
            i++;
        }
        else if (source[i] == '/')
        {
            tokens[tokenCount].type = TOKEN_SLASH;
            setstrchar(tokens[tokenCount].value,'+');
            tokenCount++;
            i++;
        }
        else if (source[i] == '=')
        {
            if (source[i+1] != '=')
            {
                tokens[tokenCount].type = TOKEN_EQUAL;
                setstrchar(tokens[tokenCount].value,'=');
            }
            else
            {
                i++;
                tokens[tokenCount].type = TOKEN_EQUAL_EQUAL;
                strcpy(tokens[tokenCount].value,"==");
            }
            tokenCount++;
            i++;
        }
        else if (source[i] == '~' && source[i+1] == '=')
        {
            tokens[tokenCount].type = TOKEN_NOT_EQUAL;
            strcpy(tokens[tokenCount].value,"~=");
            tokenCount++;
            i += 2;
        }
        else if (source[i] == '(')
        {
            tokens[tokenCount].type = TOKEN_LPAREN;
            setstrchar(tokens[tokenCount].value,'(');
            tokenCount++;
            i++;
        }
        else if (source[i] == ')')
        {
            tokens[tokenCount].type = TOKEN_RPAREN;
            setstrchar(tokens[tokenCount].value,')');
            tokenCount++;
            i++;
        }
        else if (source[i] == '{')
        {
            tokens[tokenCount].type = TOKEN_LBRACE;
            setstrchar(tokens[tokenCount].value,'{');
            tokenCount++;
            i++;
        }
        else if (source[i] == '}')
        {
            tokens[tokenCount].type = TOKEN_RBRACE;
            setstrchar(tokens[tokenCount].value,'}');
            tokenCount++;
            i++;
        }
        else if (source[i] >= 'a' && source[i] <= 'z')
        {
            int start = i;
            while (source[i] >= 'a' && source[i] <= 'z')
            {
                i++;
            }
            int n = i - start;
            char lexeme[64];
            strncpy(lexeme,source + start,n);
            if (!strcmp(lexeme,"int")) {
                tokens[tokenCount].type = TOKEN_INT;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            else if (!strcmp(lexeme,"if")) {
                tokens[tokenCount].type = TOKEN_IF;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            else if (!strcmp(lexeme,"else")) {
                tokens[tokenCount].type = TOKEN_ELSE;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            else if (!strcmp(lexeme,"while")) {
                tokens[tokenCount].type = TOKEN_WHILE;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            else if (!strcmp(lexeme,"return")) {
                tokens[tokenCount].type = TOKEN_RETURN;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            else {
                tokens[tokenCount].type = TOKEN_IDENIFIER;
                strcpy(tokens[tokenCount].value,lexeme);
            }
            tokenCount++;
        }
        else if (source[i] == '"')
        {
            i++;
            int start = i;
            while (source[i] != '"' && source[i] != '\0')
            {
                i++;
            }
            if (source[i] == '\0')
            {
                printf("silkate: unclosed string starting at char #%d", start);
                return 0;
            }
            int n = i - start;
            tokens[tokenCount].type = TOKEN_STRING;
            strncpy(tokens[tokenCount].value,source + start,n);
            tokens[tokenCount].value[n] = '\0';
            tokenCount++;
            i++;
        }
        else
        {
            printf("silkate: unexpected token: %c", source[i]);
            return 0;
        }
    }
    for (int j = 0; j < tokenCount; j++)
    {
        printf("Token #%d: Type: %d | Value: %s\n", j + 1, tokens[j].type, tokens[j].value);
    }
    return 1;
}

int main(int argc, char const *argv[])
{
    int argi = 1;
    if (argc < 2) {
        printf("usage: silkate [flags] <filename>\n");
        return 0;
    }

    FILE *file = fopen(argv[argi],"rb");
    if (!file) {
        printf("silkate: failed to open file: %s", argv[argi]);
        return 1;
    }

    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *source = malloc(size + 1);

    if (source == NULL) {
        printf("silkate: failed to read file");
        return 1;
    }

    fread(source,1,size,file);
    source[size] = '\0';

    fclose(file);

    tokenize(source);

    free(source);

    return 0;
}
