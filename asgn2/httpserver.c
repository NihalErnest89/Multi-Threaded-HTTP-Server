#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "asgn2_helper_funcs.h"

#define BUF 2048

int custom_error(int num) {
    if (num == 1) {
        fprintf(stderr, "Invalid Port\n");
    }
    return 0;
}

int main(int argc, char **argv) {

    // Throw an error if the number of command-line arguments is incorrect or the port number is not a number
    if (argc != 2) {
        custom_error(1);
        return 1;
    }

    // Write a for loop that uses isdigit() to check if the entered port number is in fact a number.
    int is_number = 1;
    int l = strlen(argv[1]);
    for(int i = 0; i < l; i++) {
        if (!isdigit(argv[1][i])) {
            is_number = 0;
            break;
        }
    }

    int port = atoi(argv[1]);

    if (is_number == 0 || port < 1 || port > 65535) {
	    custom_error(1);
	    return 1;
    }

    printf("Port Number: %d\n", port);

    Listener_Socket sock;

    int li = listener_init(&sock, port);

    if (li < 0) {
	printf("listener_init = %d\n", li);
        custom_error(1);
	return 1;
    }
    printf("listener_init succeeded\n");
    while (1) {
    	int connfd = listener_accept(&sock);
        printf("connection was just opened\n");

	char buf[BUF + 1];
	int bytes_read = 0;

	printf("test\n");

	do {
	    buf[bytes_read] = 0;
	    bytes_read = read(connfd, buf, BUF);
	    write(connfd, buf, bytes_read);
	} while (bytes_read > 0);
        
	printf("endgame\n"); 
	close(connfd);
    }

    return 0;
}
