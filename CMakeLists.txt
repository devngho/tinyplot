cmake_minimum_required(VERSION 3.31)
project(tinyplot C)

set(CMAKE_C_STANDARD 11)

enable_testing()

add_executable(tinyplot main.c)
target_compile_definitions(tinyplot PRIVATE MAIN_BUILD)

# 테스트 실행 파일 (main.c의 코드를 함께 사용)
add_executable(test_tokenizer test_tokenizer.c main.c)
target_compile_definitions(test_tokenizer PRIVATE UNIT_TEST_BUILD)

add_executable(test_parser test_parser.c main.c)
target_compile_definitions(test_parser PRIVATE UNIT_TEST_BUILD)

add_executable(test_evaluator test_evaluator.c main.c)
target_compile_definitions(test_evaluator PRIVATE UNIT_TEST_BUILD)

if (UNIX AND NOT APPLE)
    target_link_libraries(tinyplot m)
    target_link_libraries(test_tokenizer m)
    target_link_libraries(test_parser m)
    target_link_libraries(test_evaluator m)
endif ()

add_test(NAME TokenizerTest COMMAND test_tokenizer)
add_test(NAME ParserTest COMMAND test_parser)
add_test(NAME EvaluatorTest COMMAND test_evaluator)
