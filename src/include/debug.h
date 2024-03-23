#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdio.h>

#include "color.h"

// #define DEBUG_ON
// #define SINGLE_DOLLAR_ON

#define USE_VAR(var) (void)var;

#ifdef DEBUG_ON
#define PRINT_BYTE(n)    (void)fprintf(stderr, "[line: %d, type: byte] %s %x\n",     __LINE__, #n, n);
#define PRINT_INT(n)     (void)fprintf(stderr, "[line: %d, type: int] %s %d\n",      __LINE__, #n, n);
#define PRINT_UINT(n)    (void)fprintf(stderr, "[line: %d, type: uint] %s %u\n",     __LINE__, #n, n);
#define PRINT_LONG(n)    (void)fprintf(stderr, "[line: %d, type: long] %s %ld\n",    __LINE__, #n, n);
#define PRINT_ULONG(n)   (void)fprintf(stderr, "[line: %d, type: ulong] %s %lu\n",   __LINE__, #n, n);
#define PRINT_ULX(n)     (void)fprintf(stderr, "[line: %d, type: ulong_b] %s %lX\n", __LINE__, #n, n);
#define PRINT_DOUBLE(n)  (void)fprintf(stderr, "[line: %d, type: double] %s %lf\n",  __LINE__, #n, n);
#define PRINT_FLOAT(n)   (void)fprintf(stderr, "[line: %d, type: float] %s %f\n",    __LINE__, #n, n);
#define PRINT_POINTER(p) (void)fprintf(stderr, "[line: %d, type: pointer] %s %p\n",  __LINE__, #p, p);
#define PRINT_CHAR(n)    (void)fprintf(stderr, "[line: %d, type: char] %s %c\n",     __LINE__, #n, n);
#define PRINT_SIZE(n)    (void)fprintf(stderr, "[line: %d, type: byte] %s %lu\n",    __LINE__, #n, n);
#else
#define PRINT_BYTE(n)   ;
#define PRINT_INT(n)    ;
#define PRINT_UINT(n)   ;
#define PRINT_LONG(n)   ;
#define PRINT_ULONG(n)  ;
#define PRINT_ULX(n)    ;
#define PRINT_DOUBLE(n) ;
#define PRINT_FLOAT(n)  ;
#endif // DEBUG_ON

#ifdef SINGLE_DOLLAR_ON
#define $         (void)fprintf(stderr, BOLD MAGENTA ">>> %s(%d) %s\n"  RESET,\
                         __FILE__, __LINE__, __PRETTY_FUNCTION__);
#else
#define $ ;
#endif // SINGLE_DOLLAR_ON

#ifdef DOUBLE_DOLLAR_ON
#define $$(...) { (void)fprintf(stderr,                                       \
                                BOLD GREEN"  [ %s ][ %d ][ %s ][ %s ]\n" RESET,\
                                __FILE__,                                     \
                                __LINE__,                                     \
                                __PRETTY_FUNCTION__,                          \
                                #__VA_ARGS__);                                \
                  __VA_ARGS__; }
#else
#define $$(...) { __VA_ARGS__; }
#endif // DOUBLE_DOLLAR_ON

#ifdef TRIPLE_DOLLAR_ON
#define $$$       (void)fprintf(stderr,                                       \
                                BOLD BLUE "{ %s }{ %d }{ %s }\n" RESET,     \
                                __PRETTY_FUNCTION__,                          \
                                __LINE__,                                     \
                                __FILE__);
#else
#define $$$ ;
#endif // TRIPLE_DOLLAR_ON

#endif // LIB_DEBUG_H_
