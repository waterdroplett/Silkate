#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 100

typedef enum
{
    TOKEN_IDENTIFIER,
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

typedef enum
{
    AST_PROGRAM,
    AST_INTEGER,
    AST_IDENTIFIER,
    AST_BINARY,
    AST_DECLARATION
} ASTType;

typedef struct AST AST;

struct AST
{
    ASTType type;

    union
    {
        struct
        {
            AST **statements;
            int statementCount;
        } program;

        int integer;

        char identifier[64];

        struct
        {
            AST *left;
            AST *right;
            TokenType operator;
        } binary;

        struct
        {
            char name[64];
            AST *value;
        } declaration;
    };
};
typedef struct
{
    Token *tokens;
    int count;
    int current;
} Parser;

Token *peek(Parser *parser)
{
    return &parser->tokens[parser->current];
}
Token *advance(Parser *parser)
{
    return &parser->tokens[parser->current++];
}

void add_statement(AST *program, AST *statement)
{
    int count = program->program.statementCount;

    program->program.statements =
        realloc(
            program->program.statements,
            sizeof(AST *) * (count + 1)
        );

    program->program.statements[count] = statement;
    program->program.statementCount++;
}

void setstrchar(char *__restrict__ _str, char _chr)
{
    _str[0] = _chr;
    _str[1] = '\0';
}

int tokenize(const char *source, Token *tokens)
{
    int tokenCount = 0;
    
    int i = 0;
    while (source[i] != '\0')
    {
        if (tokenCount >= MAX_TOKENS) {
            printf("silkate: too many tokens\n");
            return -1;
        }
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
            setstrchar(tokens[tokenCount].value,'/');
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
            lexeme[n] = '\0';
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
                tokens[tokenCount].type = TOKEN_IDENTIFIER;
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
                return -1;
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
            return -1;
        }
    }
    for (int j = 0; j < tokenCount; j++)
    {
        printf("Token #%d: Type: %d | Value: %s\n", j + 1, tokens[j].type, tokens[j].value);
    }
    return tokenCount;
}

const char *operator_string(TokenType type)
{
    switch (type)
    {
        case TOKEN_PLUS:         return "+";
        case TOKEN_MINUS:        return "-";
        case TOKEN_STAR:         return "*";
        case TOKEN_SLASH:        return "/";
        case TOKEN_EQUAL_EQUAL:  return "==";
        case TOKEN_NOT_EQUAL:    return "~=";
        default:                 return "?";
    }
}

AST *parse_expression(Parser *parser)
{
    Token *token = peek(parser);

    if (token->type != TOKEN_INTEGER)
    {
        printf("silkate: expected expression\n");
        return NULL;
    }

    AST *left = malloc(sizeof(AST));

    if (left == NULL)
    {
        return NULL;
    }

    left->type = AST_INTEGER;
    left->integer = atoi(token->value);

    advance(parser);

    while (peek(parser)->type == TOKEN_PLUS)
    {
        TokenType operator = peek(parser)->type;
        advance(parser);

        token = peek(parser);

        if (token->type != TOKEN_INTEGER)
        {
            printf("silkate: expected integer after '+'\n");
            free(left);
            return NULL;
        }

        AST *right = malloc(sizeof(AST));

        if (right == NULL)
        {
            free(left);
            return NULL;
        }

        right->type = AST_INTEGER;
        right->integer = atoi(token->value);

        advance(parser);

        AST *binary = malloc(sizeof(AST));

        if (binary == NULL)
        {
            free(left);
            free(right);
            return NULL;
        }

        binary->type = AST_BINARY;
        binary->binary.left = left;
        binary->binary.right = right;
        binary->binary.operator = operator;

        left = binary;
    }

    return left;
}

AST *parse_declaration(Parser *parser)
{
    AST *declaration = malloc(sizeof(AST));

    if (declaration == NULL)
    {
        return NULL;
    }

    declaration->type = AST_DECLARATION;

    // Skip the "int"
    advance(parser);

    // We should now be looking at the identifier
    Token *name = peek(parser);

    if (name->type != TOKEN_IDENTIFIER)
    {
        printf("silkate: expected identifier after 'int'\n");
        free(declaration);
        return NULL;
    }

    strcpy(declaration->declaration.name, name->value);
    advance(parser);

    // We should now have '='
    if (peek(parser)->type != TOKEN_EQUAL)
    {
        printf("silkate: expected '=' after identifier\n");
        free(declaration);
        return NULL;
    }

    advance(parser);

    // For now, only allow an integer as the value
    AST *value = parse_expression(parser);

    if (value == NULL)
    {
        free(declaration);
        return NULL;
    }

    declaration->declaration.value = value;

    advance(parser);

    return declaration;
}

AST *parse_statement(Parser *parser)
{
    Token *token = &parser->tokens[parser->current];

    if (token->type == TOKEN_INT)
    {
        return parse_declaration(parser);
    }

    return NULL;
}

AST *parse(Parser *parser)
{
    AST *program = malloc(sizeof(AST));

    if (program == NULL)
    {
        return NULL;
    }

    program->type = AST_PROGRAM;
    program->program.statements = NULL;
    program->program.statementCount = 0;

    while (parser->current < parser->count)
    {
        Token *token = peek(parser);

        if (token->type == TOKEN_NEWLINE)
        {
            advance(parser);
            continue;
        }

        AST *statement = parse_statement(parser);

        if (statement == NULL)
        {
            free(program);
            return NULL;
        }

        add_statement(program, statement);
    }

    return program;
}

void print_ast(AST *node, int indent)
{
    if (node == NULL)
    {
        return;
    }

    for (int i = 0; i < indent; i++)
    {
        printf("\t");
    }

    switch (node->type)
    {
        case AST_PROGRAM:
            printf("PROGRAM\n");

            for (int i = 0; i < node->program.statementCount; i++)
            {
                print_ast(node->program.statements[i], indent + 1);
            }
            break;

        case AST_DECLARATION:
            printf("DECLARATION: %s\n", node->declaration.name);
            print_ast(node->declaration.value, indent + 1);
            break;

        case AST_INTEGER:
            printf("INTEGER: %d\n", node->integer);
            break;

        case AST_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->identifier);
            break;

        case AST_BINARY:
            printf("BINARY: %s\n", operator_string(node->binary.operator));
            print_ast(node->binary.left, indent + 1);
            print_ast(node->binary.right, indent + 1);
            break;
    }
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
    
    Token tokens[MAX_TOKENS];

    int tokenCount = tokenize(source,tokens);
    
    if (tokenCount == -1)
    {
        return 1;
    }

    free(source);

    Parser parser;

    parser.tokens = tokens;
    parser.count = tokenCount;
    parser.current = 0;

    AST *program = parse(&parser);

    print_ast(program,0);

    return 0;
}
