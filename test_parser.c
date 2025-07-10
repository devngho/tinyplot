#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct Token** tokenize(const char *expression);
extern struct Parser* create_parser(struct Token **tokens);
extern struct ASTNode* parse_expression(struct Parser *parser, int precedence);
extern void free_ast_node(struct ASTNode *node);
extern void free_tokens(struct Token **tokens);
extern void sprint_expressions_simple(struct ASTNode *node, char *buffer);

enum ASTExpressions {
    AST_CONSTANT,
    AST_VARIABLE,
    AST_BINARY_OPERATION,
    AST_UNARY_OPERATION,
    AST_FUNCTION
};

struct ASTNode {
    int type;
    void *value;
};

typedef struct {
    const char *inp;
    const char *expected;
} TestCase;

int test_parser(const char* inp, const char* expected) {
    struct Token **tokens = tokenize(inp);
    struct Parser *parser = create_parser(tokens);
    struct ASTNode *ast = parse_expression(parser, 0);

    char *buffer = malloc(1024 * sizeof(char));

    sprint_expressions_simple(ast, buffer);

    if (strcmp(buffer, expected) != 0) {
        printf("TEST FAILED: Expected '%s', got '%s'\n", expected, buffer);
        free_ast_node(ast);
        free_tokens(tokens);
        free(parser);
        return 1;
    }

    free_ast_node(ast);
    free_tokens(tokens);
    free(parser);
    free(buffer);
    printf("Parser test PASSED!\n");
    return 0;
}

// main 함수: 테스트 목록을 구성하여 순차 실행
int main() {
    TestCase tests[] = {
        {"1+2", "(1.000000 + 2.000000)"},
        {"sin(x)", "sin(x)"},
        {"3(4 + 5)", "(3.000000 * (4.000000 + 5.000000))"},
        {"x^2 + y^2", "((x ^ 2.000000) + (y ^ 2.000000))"},
        {"sin(0) + cos(0)", "(sin(0.000000) + cos(0.000000))"},
        {"3sin(2) + 3(4 + 5)", "((3.000000 * sin(2.000000)) + (3.000000 * (4.000000 + 5.000000)))"},
        {"2x+2sin(x)", "((2.000000 * x) + (2.000000 * sin(x)))"},
        {"x^2 + 3x + 2", "(((x ^ 2.000000) + (3.000000 * x)) + 2.000000)"},
        {"(x+1)(x-1)", "((x + 1.000000) * (x - 1.000000))"},
        {"log(x) + exp(x)", "(log(x) + exp(x))"},
        {"sqrt(4) + abs(-5)", "(sqrt(4.000000) + abs(-5.000000))"},
        {"3 + 4 * 2 / (1 - 5) ^ 2 ^ 3", "(3.000000 + ((4.000000 * 2.000000) / ((1.000000 - 5.000000) ^ (2.000000 ^ 3.000000))))"},
        {"x^2 + y^2 + z^2", "(((x ^ 2.000000) + (y ^ 2.000000)) + (z ^ 2.000000))"},
        {"(x+y)(x-y)", "((x + y) * (x - y))"},
    };
    int num_tests = sizeof(tests) / sizeof(TestCase);
    int failures = 0;
    for (int i = 0; i < num_tests; i++) {
        printf("Running test: %s\n", tests[i].inp);
        if (test_parser(tests[i].inp, tests[i].expected) != 0) {
            failures++;
        }
    }
    if (failures == 0) {
        printf("All Parser tests PASSED!\n");
    } else {
        printf("%d Parser test(s) FAILED!\n", failures);
    }
    return failures;
}
