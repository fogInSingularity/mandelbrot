CXX = g++

EXE = mandelbrot

SOURCES = src/source/*
INCLUDE = -Isrc/include/

FLAGS_GCC = -std=c++17 -Wall -Wextra -Waggressive-loop-optimizations          \
-Wmissing-declarations -Wcast-align -Wcast-qual                               \
-Wchar-subscripts -Wconversion                                                \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security              \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op                          \
-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self                            \
-Wredundant-decls -Wshadow -Wsign-conversion                                  \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn                              \
-Wsuggest-final-methods -Wsuggest-final-types                                 \
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code         \
-Wunused -Wvariadic-macros                                                    \
-Wno-missing-field-initializers -Wno-narrowing                                \
-Wno-varargs -fstack-protector-strong -fcheck-new -fstrict-overflow -Wstack-usage=8192 -Wstack-protector \

ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,$\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr

O_LEVEL = -O2
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
MARCH = -march=znver1

PROFILE = -fprofile-use
VEC_FLAGS = -ftree-vectorize -fopt-info-vec-optimized -fopt-info-vec-missed

DEBUG_FLAGS = $(FLAGS_GCC) $(_ASAN_FLAGS) $(O_LEVEL) $(SFML_FLAGS) $(MARCH) -g -ggdb -D_FORTIFY_SOURCE=2
RELEASE_FLAGS = $(FLAGS_GCC) $(O_LEVEL) $(SFML_FLAGS) $(MARCH) -s -flto -DNDEBUG -fno-omit-frame-pointer 

all:
	@$(CXX) $(INCLUDE) $(SOURCES) $(DEBUG_FLAGS) -o $(EXE) 

release:
	@$(CXX) $(INCLUDE) $(SOURCES) $(RELEASE_FLAGS) -o $(EXE) 

asm:
	@$(CXX) $(INCLUDE) $(SOURCES) $(RELEASE_FLAGS) -s -S

analyze:
	@clang-tidy $(SOURCES) -checks=clang-analyzer-*,performance-*
