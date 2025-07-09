#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_EXPRESSION_LEN 200
#define X_RES 50
#define Y_RES 30

enum TokenKind {
    CONSTANT,
    IDENTIFIER,
    OPERATOR,
    END
};

struct Token {
    enum TokenKind type;
    void *value;
};

struct Token* create_constant_token(double value) {
    struct Token *token = malloc(sizeof(struct Token));
    token->type = CONSTANT;
    token->value = malloc(sizeof(double));
    *(double *)(token->value) = value;
    return token;
}

double get_constant_value(const struct Token *token) {
    if (token->type == CONSTANT) {
        return *(double *)(token->value);
    }

    return -(1<<30);
}

struct Token* create_identifier_token(char* variable) {
    struct Token *token = malloc(sizeof(struct Token));
    token->type = IDENTIFIER;
    token->value = strdup(variable);
    return token;
}

struct Token* create_operator_token(char* operator) {
    struct Token *token = malloc(sizeof(struct Token));
    token->type = OPERATOR;
    token->value = strdup(operator);
    return token;
}

void free_token(struct Token *token) {
    if (token) {
        if (token->type == IDENTIFIER || token->type == OPERATOR) {
            if (token->value) free(token->value);
        } else if (token->type == CONSTANT) {
            if (token->value) free(token->value);
        }
        free(token);
    }
}

void add_token(struct Token **tokens[], int *token_count, struct Token *token) {
    *tokens = realloc(*tokens, (*token_count + 1) * sizeof(struct Token*));
    (*tokens)[*token_count] = token;
    (*token_count)++;
}

void free_tokens(struct Token **tokens) {
    for (int i = 0; tokens[i]->type != END; i++) {
        free_token(tokens[i]);
    }
    free(tokens);
}

struct Token** tokenize(const char *expression) {
    struct Token **tokens = malloc(sizeof(struct Token*));
    int token_count = 0;
    int i = 0;

    while (1) {
        if ('0' <= expression[i] && expression[i] <= '9') {
            struct Token* token = create_constant_token(0.0);

            while ('0' <= expression[i] && expression[i] <= '9') {
                *(double*)token->value = get_constant_value(token) * 10 + (expression[i] - '0');
                i++;
            }
            if (expression[i] == '.') {
                i++;
                double decimal_place = 0.1;
                while ('0' <= expression[i] && expression[i] <= '9') {
                    *(double*)token->value += (expression[i] - '0') * decimal_place;
                    decimal_place *= 0.1;
                    i++;
                }
            }

            add_token(&tokens, &token_count, token);
        } else if ('a' <= expression[i] && expression[i] <= 'z' || ('A' <= expression[i] && expression[i] <= 'Z') || (expression[i] == '_')) {
            char *name = malloc(sizeof(char));

            int j = 0;
            while (('a' <= expression[i] && expression[i] <= 'z') || ('A' <= expression[i] && expression[i] <= 'Z') || (expression[i] == '_') || ('0' <= expression[i] && expression[i] <= '9')) {
                name = realloc(name, (j + 2) * sizeof(char));
                name[j++] = expression[i++];
            }
            name[j] = '\0';

            add_token(&tokens, &token_count, create_identifier_token(name));
            free(name);
        } else if (strchr("+-*/()^", expression[i]) != NULL) {
            char *operator = malloc(2 * sizeof(char));
            operator[0] = expression[i++];
            operator[1] = '\0';
            add_token(&tokens, &token_count, create_operator_token(operator));
            free(operator);
        } else if (expression[i] == ' ') {
            i++;
        } else {
            break;
        }

        if (expression[i] == '\0') {
            break;
        }
    }

    struct Token *end_token = malloc(sizeof(struct Token));
    end_token->type = END;
    end_token->value = NULL;
    add_token(&tokens, &token_count, end_token);

    return tokens;
}

void print_tokens(struct Token **tokens) {
    int i = 0;
    while (tokens[i]->type != END) {
        if (tokens[i]->type == CONSTANT) {
            printf("Constant: %f\n", get_constant_value(tokens[i]));
        } else if (tokens[i]->type == IDENTIFIER) {
            printf("Identifier: %s\n", (char *)(tokens[i]->value));
        } else if (tokens[i]->type == OPERATOR) {
            printf("Operator: %s\n", (char *)(tokens[i]->value));
        }
        i++;
    }
}

enum ASTExpressions {
    AST_CONSTANT,
    AST_VARIABLE,
    AST_BINARY_OPERATION,
    AST_UNARY_OPERATION,
    AST_FUNCTION
};

struct ASTNode {
    enum ASTExpressions type;
    void *value;
};

struct ASTBinaryOperation {
    struct ASTNode *left;
    struct ASTNode *right;
    char operator;
};

struct ASTUnaryOperation {
    struct ASTNode *operand;
    char *operator;
};

struct ASTFunction {
    char *name;
    struct ASTNode *argument;
};

struct ASTNode* create_constant_node(double value) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = AST_CONSTANT;
    node->value = malloc(sizeof(double));
    *(double *)(node->value) = value;
    return node;
}

struct ASTNode* create_variable_node(char *name) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = AST_VARIABLE;
    node->value = strdup(name);
    return node;
}

struct ASTNode* create_binary_operation_node(struct ASTNode *left, struct ASTNode *right, char operator) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = AST_BINARY_OPERATION;
    struct ASTBinaryOperation *op = malloc(sizeof(struct ASTBinaryOperation));
    op->left = left;
    op->right = right;
    op->operator = operator;
    node->value = op;
    return node;
}

struct ASTNode* create_unary_operation_node(struct ASTNode *operand, char* operator) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = AST_UNARY_OPERATION;
    struct ASTUnaryOperation *op = malloc(sizeof(struct ASTUnaryOperation));
    op->operand = operand;
    op->operator = strdup(operator);
    node->value = op;
    return node;
}

struct ASTNode* create_function_node(char *name, struct ASTNode *argument) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = AST_FUNCTION;
    struct ASTFunction *func = malloc(sizeof(struct ASTFunction));
    func->name = strdup(name);
    func->argument = argument;
    node->value = func;
    return node;
}

void free_ast_node(struct ASTNode *node) {
    if (node) {
        switch (node->type) {
            case AST_CONSTANT:
                free(node->value);
                break;
            case AST_VARIABLE:
                if (node->value) free(node->value);
                break;
            case AST_BINARY_OPERATION: {
                struct ASTBinaryOperation *op = (struct ASTBinaryOperation *)(node->value);
                free_ast_node(op->left);
                free_ast_node(op->right);
                free(op);
                break;
            }
            case AST_UNARY_OPERATION: {
                struct ASTUnaryOperation *op = (struct ASTUnaryOperation *)(node->value);
                free_ast_node(op->operand);
                if (op->operator) free(op->operator);
                free(op);
                break;
            }
            case AST_FUNCTION: {
                struct ASTFunction *func = (struct ASTFunction *)(node->value);
                if (func->name) free(func->name);
                free_ast_node(func->argument);
                free(func);
                break;
            }
        }
        free(node);
    }
}

struct Parser {
    struct Token **tokens;
    int current;
};

struct Parser* create_parser(struct Token **tokens) {
    struct Parser *parser = malloc(sizeof(struct Parser));
    parser->tokens = tokens;
    parser->current = 0;
    return parser;
}

struct Token* parser_cur(const struct Parser *parser) {
    return parser->tokens[parser->current];
}

struct Token* parser_peek(const struct Parser *parser) {
    return parser->tokens[parser->current + 1];
}

struct Token* parser_next(struct Parser *parser) {
    if (parser->tokens[parser->current]->type == END) {
        return NULL;
    }
    return parser->tokens[parser->current++];
}

int op_precedence(char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) {
        return 1;
    } else if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        return 2;
    } else if (strcmp(op, "^") == 0) {
        return 3;
    } else if (strcmp(op, "·") == 0) {
        return 200;
    } else if (strcmp(op, "(") == 0) {
        return 100;
    } else if (strcmp(op, ")") == 0) {
        return -100;
    }
    return 0;
}

int parser_precedence(struct Parser *parser) {
    struct Token *token = parser_cur(parser);
    if (token->type == OPERATOR) {
        char *op = (char *)(token->value);

        return op_precedence(op);
    }
    return 0;
}

int parser_peek_precedence(struct Parser *parser) {
    if (parser->tokens[parser->current]->type == END) {
        return -1;
    }

    if (parser->tokens[parser->current + 1]->type == END) {
        return -1;
    }

    struct Token *token = parser_peek(parser);
    if (token->type == OPERATOR) {
        char *op = (char *)(token->value);

        return op_precedence(op);
    } else if (token->type == IDENTIFIER) {
        return op_precedence("·");
    } else if (token->type == CONSTANT) {
        return 0;
    }
}

void print_expressions(struct ASTNode *node, int depth);

char *function_names[] = {
    "sin", "cos", "tan", "log", "exp", "sqrt"
};

int is_function_name(const char *name) {
    for (int i = 0; i < sizeof(function_names) / sizeof(function_names[0]); i++) {
        if (strcmp(name, function_names[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

struct ASTNode* parse_expression(struct Parser *parser, int precedence) {
    struct Token *token = parser_cur(parser);
    struct ASTNode *left = NULL;

    if (token->type == CONSTANT) {
        left = create_constant_node(get_constant_value(token));
    } else if (token->type == IDENTIFIER) {
        left = create_variable_node((char *)(token->value));
    } else if (token->type == OPERATOR && (strcmp((char *)(token->value), "+") == 0 || strcmp((char *)(token->value), "-") == 0)) {
        char *operator = (char *)token->value;
        parser_next(parser);
        struct ASTNode *right = parse_expression(parser, op_precedence(operator));
        left = create_unary_operation_node(right, operator);
    } else if (token->type == OPERATOR && strcmp((char *)(token->value), "(") == 0) {
        parser_next(parser);
        left = parse_expression(parser, 0);
        parser_next(parser);
        if (parser_cur(parser)->type != OPERATOR || strcmp(parser_cur(parser)->value, ")") != 0) {
            fprintf(stderr, "Expected ')'\n");
            exit(EXIT_FAILURE);
        }
    }

    if (left == NULL) {
        fprintf(stderr, "Unexpected token: %s\n", parser_cur(parser)->type == END ? "END" : (char *)(parser_cur(parser)->value));
        exit(EXIT_FAILURE);
    }

    while (parser_peek_precedence(parser) > precedence) {
        struct Token *op_token = parser_peek(parser);
        if (op_token == NULL) break;

        parser_next(parser);

        if (op_token->type == OPERATOR) {
            char *op = op_token->value;

            if (strcmp(op, ")") == 0) break;
            if (strcmp(op, "(") == 0) {
                parser_next(parser);

                if (left->type == AST_CONSTANT) {
                    struct ASTNode *argument = parse_expression(parser, 0);

                    left = create_binary_operation_node(left, argument, '*');
                } else {
                    char *left_name = (char *)(left->value);

                    struct ASTNode *argument = parse_expression(parser, 0);

                    struct ASTNode *func_node = create_function_node(left_name, argument);

                    parser_next(parser);
                    if (parser_cur(parser)->type != OPERATOR || strcmp(parser_cur(parser)->value, ")") != 0) {
                        fprintf(stderr, "Expected ')'\n");
                        exit(EXIT_FAILURE);
                    }

                    left = func_node;
                }
            } else {
                parser_next(parser);

                struct ASTNode *right = parse_expression(parser, op_precedence(op));

                left = create_binary_operation_node(left, right, op[0]);
            }
        } else if (op_token->type == IDENTIFIER) {
            char *identifier = (char *)(op_token->value);

            if (!is_function_name(identifier)) {
                struct ASTNode *right = parse_expression(parser, op_precedence("·"));
                left = create_binary_operation_node(left, right, '*');
            } else {
                parser_next(parser);

                if (parser_cur(parser)->type != OPERATOR || strcmp((char *)(parser_cur(parser)->value), "(") != 0) {
                    fprintf(stderr, "Expected '('\n");
                    exit(EXIT_FAILURE);
                }
                parser_next(parser);

                struct ASTNode *argument = parse_expression(parser, 0);
                if (parser_cur(parser)->type != OPERATOR || strcmp(parser_cur(parser)->value, ")") != 0) {
                    fprintf(stderr, "Expected ')'\n");
                    exit(EXIT_FAILURE);
                }

                struct ASTNode *to_mul = left;
                left = create_function_node(identifier, argument);
                left = create_binary_operation_node(to_mul, left, '*');
            }
        } else if (op_token->type == CONSTANT) {
            fprintf(stderr, "Unexpected constant in the middle of expression.\n");
            exit(EXIT_FAILURE);
        }
    }

    return left;
}

void print_expressions(struct ASTNode *node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    switch (node->type) {
        case AST_CONSTANT:
            printf("Constant: %f\n", *(double *)(node->value));
            break;
        case AST_VARIABLE:
            printf("Variable: %s\n", (char *)(node->value));
            break;
        case AST_BINARY_OPERATION: {
            struct ASTBinaryOperation *op = (struct ASTBinaryOperation *)(node->value);
            printf("Binary Operation: %c\n", op->operator);
            print_expressions(op->left, depth + 1);
            print_expressions(op->right, depth + 1);
            break;
        }
        case AST_UNARY_OPERATION: {
            struct ASTUnaryOperation *op = (struct ASTUnaryOperation *)(node->value);
            printf("Unary Operation: %c\n", op->operator[0]);
            print_expressions(op->operand, depth + 1);
            break;
        }
        case AST_FUNCTION: {
            struct ASTFunction *func = (struct ASTFunction *)(node->value);
            printf("Function: %s\n", func->name);
            print_expressions(func->argument, depth + 1);
            break;
        }
    }
}

void print_expressions_simple(struct ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_CONSTANT:
            printf("%f", *(double *)(node->value));
            break;
        case AST_VARIABLE:
            printf("%s", (char *)(node->value));
            break;
        case AST_BINARY_OPERATION: {
            struct ASTBinaryOperation *op = (struct ASTBinaryOperation *)(node->value);
            printf("(");
            print_expressions_simple(op->left);
            printf(" %c ", op->operator);
            print_expressions_simple(op->right);
            printf(")");
            break;
        }
        case AST_UNARY_OPERATION: {
            struct ASTUnaryOperation *op = (struct ASTUnaryOperation *)(node->value);
            printf("%c", op->operator[0]);
            print_expressions_simple(op->operand);
            break;
        }
        case AST_FUNCTION: {
            struct ASTFunction *func = (struct ASTFunction *)(node->value);
            printf("%s(", func->name);
            print_expressions_simple(func->argument);
            printf(")");
            break;
        }
    }
}

double evaluate_expression(struct ASTNode *node, double x) {
    if (!node) return 0.0;

    switch (node->type) {
        case AST_CONSTANT:
            return *(double *)(node->value);
        case AST_VARIABLE:
            char *name = node->value;
            if (strcmp(name, "x") == 0) {
                return x;
            } else {
                if (strcmp(name, "pi") == 0) return M_PI;
                else if (strcmp(name, "e") == 0) return M_E;

                fprintf(stderr, "Unknown variable: %s\n", name);
                exit(EXIT_FAILURE);

                return 0.0;
            }
        case AST_BINARY_OPERATION: {
            struct ASTBinaryOperation *op = (struct ASTBinaryOperation *)(node->value);
            double left_value = evaluate_expression(op->left, x);
            double right_value = evaluate_expression(op->right, x);
            switch (op->operator) {
                case '+': return left_value + right_value;
                case '-': return left_value - right_value;
                case '*': return left_value * right_value;
                case '/': return left_value / right_value;
                case '^': return pow(left_value, right_value);
                default: return 0.0;
            }
        }
        case AST_UNARY_OPERATION: {
            struct ASTUnaryOperation *op = (struct ASTUnaryOperation *)(node->value);
            double operand_value = evaluate_expression(op->operand, x);
            if (strcmp(op->operator, "-") == 0) {
                return -operand_value;
            }
            return operand_value;
        }
        case AST_FUNCTION: {
            struct ASTFunction *func = (struct ASTFunction *)(node->value);
            double argument_value = evaluate_expression(func->argument, x);
            if (strcmp(func->name, "sin") == 0) {
                return sin(argument_value);
            } else if (strcmp(func->name, "cos") == 0) {
                return cos(argument_value);
            } else if (strcmp(func->name, "tan") == 0) {
                return tan(argument_value);
            } else if (strcmp(func->name, "log") == 0) {
                return log(argument_value);
            } else if (strcmp(func->name, "exp") == 0) {
                return exp(argument_value);
            } else if (strcmp(func->name, "sqrt") == 0) {
                return sqrt(argument_value);
            } else {
                fprintf(stderr, "Unknown function: %s\n", func->name);
                return 0.0;
            }
        }
    }

    return 0.0;
}

double x_min = -20.0;
double x_max = 20.0;
double y_min = -10.0;
double y_max = 10.0;

void draw(struct ASTNode *ast) {
    int matrix[Y_RES][X_RES] = {0};
    double x_step = (x_max - x_min) / X_RES;

    for (int i = 0; i < X_RES; i++) {
        double x = x_min + i * x_step;
        double y = evaluate_expression(ast, x);

        int y_index = (int)((y - y_min) / (y_max - y_min) * Y_RES);
        if (y_index >= 0 && y_index < Y_RES) {
            matrix[y_index][i] = 1;
        }
    }

    for (int j = Y_RES - 1; j >= 0; j--) {
        for (int i = 0; i < X_RES; i++) {
            if (matrix[j][i] == 1) {
                printf("＊");
            } else if (j == Y_RES / 2 && i == X_RES / 2) {
                printf("＋");
            } else if (j == Y_RES / 2) {
                printf("－");
            } else if (i == X_RES / 2) {
                printf("｜");
            } else {
                printf("　");
            }
        }
        printf("\n");
    }
}

int main() {
    char expression[MAX_EXPRESSION_LEN];

    printf("Enter y=f(x) (example: y=2x, y=sqrt(x-2)+3) to see the graph.\n");
    printf("Allowed functions: sin, cos, tan, log, exp, and sqrt.\n");
    printf("Allowed constants: pi, e (* operator is required between 'x' and others)");
    printf("%.2f <= x <= %.2f, %.2f <= y <= %.2f\n", x_min, x_max, y_min, y_max);
    while (1) {
        printf("y=");

        scanf("%201[^\n]", expression);
        if (strcmp(expression, "exit") == 0) {
            break;
        }

        struct Token **tokens = tokenize(expression);
        struct Parser *parser = create_parser(tokens);
        struct ASTNode *ast = parse_expression(parser, 0);
        printf("Parsed Expression:");
        print_expressions_simple(ast);
        printf("\n");
        draw(ast);
        free_ast_node(ast);
        free_tokens(tokens);

        while (getchar() != '\n');
    }

    return 0;
}
