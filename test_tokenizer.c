#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ...existing declarations...
extern struct Token** tokenize(const char *expression);
extern double get_constant_value(const struct Token *token);
extern void free_tokens(struct Token **tokens);

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

// 새로 추가: 테스트 케이스를 위한 구조체
typedef int (*TestFunc)();
typedef struct {
    const char *name;
    TestFunc func;
} TestCase;

// 테스트 함수
int test_numeric_tokenizer() {
    const char *expr = "123.45 + 67";
    struct Token **tokens = tokenize(expr);

    if (tokens[0]->type != CONSTANT) {
        printf("TEST FAILED: First token not constant.\n");
        return 1;
    }

    double val = get_constant_value(tokens[0]);
    if (fabs(val - 123.45) > 0.0001) {
        printf("TEST FAILED: Expected 123.45, got %f\n", val);
        free_tokens(tokens);
        return 1;
    }

    free_tokens(tokens);
    printf("Numeric Tokenizer test PASSED!\n");
    return 0;
}

// main 함수: 테스트 목록을 구성하여 순차 실행
int main() {
    TestCase tests[] = {
        {"Numeric Tokenizer", test_numeric_tokenizer},
        // ...추가 테스트 함수들을 쉽게 추가할 수 있습니다...
    };
    int num_tests = sizeof(tests) / sizeof(TestCase);
    int failures = 0;
    for (int i = 0; i < num_tests; i++) {
        printf("Running test: %s\n", tests[i].name);
        if (tests[i].func() != 0) {
            failures++;
        }
    }
    if (failures == 0) {
        printf("All Tokenizer tests PASSED!\n");
    } else {
        printf("%d Tokenizer test(s) FAILED!\n", failures);
    }
    return failures;
}
