objects = pomodoro.o

CFLAGS= -g -O0 -std=c99 -Wall -Wextra -pedantic  

ASAN_FLAGS= -fsanitize=address,undefined

MSAN_FLAGS= -fsanitize=memory,undefined

all: main

main: $(objects)

a_sanitize: $(objects)
	clang $(CFLAGS) $(ASAN_FLAGS) -o main $(objects)

m_sanitize: $(objects)
	clang $(CFLAGS) $(MSAN_FLAGS) -o main $(objects)

clean: 
	rm -f main $(objects)

.PHONY: all
