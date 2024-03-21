CXX = g++

EXE = mandelbrot

SOURCES = src/source/*
INCLUDE = -Isrc/include/

FLAGS_GCC = -ggdb -std=c++17 -Wall -Wextra -Waggressive-loop-optimizations    \
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
-Wno-varargs -Wstack-protector -fcheck-new                                    \
-fstack-protector -fstrict-overflow 					                      \
-fno-omit-frame-pointer -Wstack-usage=8192                                    \

ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,$\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr

# NO_ASAN_FLAGS = 

# FLAGS_CLANG = -Wall -Wextra -std=c11
# LIBRARIES = -lm

O_LEVEL = -O2
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
MARCH = -march=znver1

FLAGS = $(FLAGS_GCC) $(NO_ASAN_FLAGS) $(O_LEVEL) $(SFML_FLAGS) $(MARCH)

all:
	@$(CXX) $(INCLUDE) $(SOURCES) $(FLAGS) -o $(EXE) 

analyze:
	@clang-tidy $(SOURCES) -checks=clang-analyzer-*,performance-*

# clang:
# 	@clang $(INCLUDE) $(SOURCES) $(FLAGS_CLANG) $(LIBRARIES) -o $(EXE)