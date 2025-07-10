#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern struct Token** tokenize(const char *expression);
extern struct Parser* create_parser(struct Token **tokens);
extern struct ASTNode* parse_expression(struct Parser *parser, int precedence);
extern double evaluate_expression(struct ASTNode *node, double x);
extern void free_ast_node(struct ASTNode *node);
extern void free_tokens(struct Token **tokens);

typedef int (*TestFunc)();
typedef struct {
    const char *name;
    TestFunc func;
} TestCase;

int test_arithmetic_evaluator() {
    const char *expr = "2 + 3 * 4";
    struct Token **tokens = tokenize(expr);
    struct Parser *parser = create_parser(tokens);
    struct ASTNode *ast = parse_expression(parser, 0);

    double result = evaluate_expression(ast, 0);
    if (fabs(result - 14) > 0.0001) {
        printf("TEST FAILED: Expected 14, got %f\n", result);
        free_ast_node(ast);
        free_tokens(tokens);
        free(parser);
        return 1;
    }

    free_ast_node(ast);
    free_tokens(tokens);
    free(parser);
    printf("Arithmetic Evaluator test PASSED!\n");
    return 0;
}

int test_implicit_multiplication() {
    const char *expr = "2(3 + 4)";
    struct Token **tokens = tokenize(expr);
    struct Parser *parser = create_parser(tokens);
    struct ASTNode *ast = parse_expression(parser, 0);

    double result = evaluate_expression(ast, 0);
    if (fabs(result - 14) > 0.0001) {
        printf("TEST FAILED: Expected 14, got %f\n", result);
        free_ast_node(ast);
        free_tokens(tokens);
        free(parser);
        return 1;
    }

    free_ast_node(ast);
    free_tokens(tokens);
    free(parser);
    printf("Implicit Multiplication test PASSED!\n");
    return 0;
}

int test_function_call() {
    const char *expr = "sin(0) + cos(0)";
    struct Token **tokens = tokenize(expr);
    struct Parser *parser = create_parser(tokens);
    struct ASTNode *ast = parse_expression(parser, 0);

    double result = evaluate_expression(ast, 0);
    if (fabs(result - 2) > 0.0001) {
        printf("TEST FAILED: Expected 2, got %f\n", result);
        free_ast_node(ast);
        free_tokens(tokens);
        free(parser);
        return 1;
    }

    free_ast_node(ast);
    free_tokens(tokens);
    free(parser);
    printf("Function Call test PASSED!\n");
    return 0;
}

int test_implicit_mul_with_function() {
    const char *expr = "3sin(2) + 3(4 + 5)";
    struct Token **tokens = tokenize(expr);
    struct Parser *parser = create_parser(tokens);
    struct ASTNode *ast = parse_expression(parser, 0);

    double result = evaluate_expression(ast, 0);
    if (fabs(result - (3*sin(2) + 27)) > 0.0001) {
        printf("TEST FAILED: Expected %f, got %f\n", (3*sin(2) + 27), result);
        free_ast_node(ast);
        free_tokens(tokens);
        free(parser);
        return 1;
    }

    free_ast_node(ast);
    free_tokens(tokens);
    free(parser);
    printf("Implicit Multiplication with Function test PASSED!\n");
    return 0;
}

int main() {
    TestCase tests[] = {
        {"Arithmetic Evaluator", test_arithmetic_evaluator},
        {"Implicit Multiplication", test_implicit_multiplication},
        {"Implicit Multiplication with Function", test_implicit_mul_with_function},
    };
    int num_tests = sizeof(tests) / sizeof(TestCase);
    int failures = 0;
    for (int i = 0; i < num_tests; i++) {
        printf("Running test: %s\n", tests[i].name);
        if (tests[i].func() != 0)
            failures++;
    }
    if (failures == 0) {
        printf("All Evaluator tests PASSED!\n");
    } else {
        printf("%d Evaluator test(s) FAILED!\n", failures);
    }
    return failures;
}
