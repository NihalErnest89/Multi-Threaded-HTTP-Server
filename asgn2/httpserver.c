#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "asgn2_helper_funcs.h"
#include <regex.h>

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

    //printf("Port Number: %d\n", port);

    Listener_Socket sock;

    int li = listener_init(&sock, port);

    if (li < 0) {
	//printf("listener_init = %d\n", li);
        custom_error(1);
	return 1;
    }
    //printf("listener_init succeeded\n");
    
    regex_t regex;
//    regcomp(&regex, "[a-zA-Z0-9.-]", REG_EXTENDED);
    regcomp(&regex, "(GET|PUT) (/[^ ]+) +(HTTP/[1][.][1])", REG_EXTENDED);
    
//    char method[3];
//    char file[20];
//    regmatch_t matches[3];

    

    while (1) {

    	int connfd = listener_accept(&sock);
        printf("connection was just opened\n");

	// Read in header char by char
	char rq[2048];
	char c;
        
	int rq_line_size = 0;
	int is_slash_r = 0;
	int is_slash_n = 0;

	while (read(connfd, &c, 1) > 0) {
	    if (c == '\r') {
	        is_slash_r = 1;
	    }
	    else if (c == '\n' && is_slash_r == 1) {
	        is_slash_n = 1;
		break;
	    }

	    else {
	    	rq[rq_line_size] = c;
		is_slash_r = 0;
		rq_line_size += 1;
	    }

	}

	// If theres no \r\n separating the request line
	if (is_slash_r == 0 || is_slash_n == 0) {
	    // replace with vaid error
	    printf("invalid request");
	    return 1;
	}

	char m[3];
	char f[2048];
	regmatch_t matches[3];

	// Following two lines are testers
	printf("Request Line: %s\n", rq);

        if (regexec(&regex, rq, 3, matches, 0) == 0) {
	    int m_len = matches[1].rm_eo - matches[1].rm_so;
	    int f_len = matches[2].rm_eo - matches[2].rm_so;
	    strncpy(m, rq + matches[1].rm_so, m_len);

	    // + 1 to skip past the slash in /foo.txt
	    strncpy(f, rq + matches[2].rm_so + 1, f_len - 1);
	    write(connfd, "\nSuccess", 8);    
	}	
        else {
	    char error_msg[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
	    write(connfd, error_msg, strlen(error_msg));
	}
	//write(connfd, rq, rq_line_size);
	write(connfd, "\n", 1);

	// Read the rest
	
	char buf[BUF + 1];
	int bytes_read = 0;
	char *token;
	char *rest;
	// printf("test\n");
	
	bytes_read = read(connfd, buf, BUF);
	token = strtok_r(buf, "\r\n", &rest);



	printf("Content: %s\n", token);
	printf("Function: %s\n", m);
	printf("File Name: %s\n", f);
        
	//fix this later. writing this one line without breaks can be too much	
	//write(connfd, rest, bytes_read - strlen(token));

        bytes_read = 0;
	do {
	    buf[bytes_read] = 0;
	    bytes_read = read(connfd, buf, BUF);
	    token = strtok(buf, "\r\n");

	    write(connfd, buf, bytes_read);
	} while (bytes_read > 0);
        
	// printf("endgame\n"); 
	close(connfd);
    }
    
    regfree(&regex);
    return 0;
}
