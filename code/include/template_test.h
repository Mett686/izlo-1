// test.h
#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef void (*test_function_t)(void);

typedef struct test_case {
    const char* test_name;
    test_function_t function;
    struct test_case* next;
} test_case_t;


extern test_case_t* base_test;


static inline void activate_test(const char* name, test_function_t function) {
    test_case_t* test = (test_case_t *)malloc(sizeof(*test));
    if(!test) { exit(1); } 
    test->test_name = name;
    test->function = function;
    test->next = base_test;
    base_test = test;
}

static inline int run_all_tests(void) {
    int passed_tests = 0;
    
    for(test_case_t *test = base_test; test; test = test->next) {
        ++passed_tests;
        printf("[ RUN      ] %s \n", test->test_name);
        test->function();
        printf("[       OK ] %s \n", test->test_name);
    }
    printf("\n[  PASSED  ] %d test(s)\n", passed_tests);
    return 0;
}

#define CHECK(expression) \
    do { \
        if (!(expression)) { \
            fprintf(stderr, "[  FAILED  ] %s:%d: CHECK(%s)\n", __FILE__, __LINE__, #expression); \
            abort(); \
        } \
    } while (0)


#define CAT2(a,b) a##b
#define CAT(a,b)  CAT2(a,b)

#define TEST_CASE(test_name) \
    static void CAT(test__, __LINE__)(void); \
    static void CAT(reg__,  __LINE__)(void) __attribute__((constructor)); \
    static void CAT(reg__,  __LINE__)(void) { activate_test((test_name), CAT(test__, __LINE__)); } \
    static void CAT(test__, __LINE__)(void)

