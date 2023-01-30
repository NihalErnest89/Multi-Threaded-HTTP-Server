CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra


all: httpserver


httpserver: httpserver.o asgn2_helper_funcs.a
	$(CC) -o httpserver httpserver.o asgn2_helper_funcs.a


httpserver.o: httpserver.c
	$(CC) $(CFLAGS) -c httpserver.c

clean:
	rm -f httpserver httpserver.o


format:
	clang-format -i -style=file *.[ch]
