CC = clang
CFLAGS = -Wall -pedantic -Werror -Wextra


all: queue.o

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c

clean:
	rm -f queue.o


format:
	clang-format -i -style=file *.[ch]

