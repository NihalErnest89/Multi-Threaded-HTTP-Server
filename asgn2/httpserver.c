#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "asgn2_helper_funcs.h"
#include <regex.h>
#include <sys/stat.h>

#define BUF   2048
#define EMPTY 0

int blind_read(int connfd) {
    char blind[BUF];
    int result = read(connfd, blind, BUF);
    while(result > 0) {
        result = read(connfd, blind, BUF);
    }

    return 0;
}


int custom_error(int num, int connfd) {
    if (num == 1) {
        fprintf(stderr, "Invalid Port\n");
    } else if (num == 400) {
        char error_msg[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        write(connfd, error_msg, strlen(error_msg));
	blind_read(connfd);
	close(connfd);
    }

    else if (num == 200) {
        struct stat fileStat;
        fstat(connfd, &fileStat);

        char len[10];
        sprintf(len, "%ld", fileStat.st_size);

        char ok_msg[] = "HTTP/1.1 200 OK\r\nContent-Length: ";
        char rest[] = "\r\n\r\n";
        write(connfd, ok_msg, strlen(ok_msg));
        write(connfd, len, strlen(len));
        write(connfd, rest, strlen(rest));
    }

    else if (num == 403) {
        char error_msg[] = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
	write(connfd, error_msg, strlen(error_msg));
	blind_read(connfd);
	close(connfd);
    }

    else if (num == 404) {
        char error_msg[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
	write(connfd, error_msg, strlen(error_msg));
	blind_read(connfd);
	close(connfd);
    }

    return 0;
}


int main(int argc, char **argv) {

    // Throw an error if the number of command-line arguments is incorrect or the port number is not a number
    if (argc != 2) {
        custom_error(1, 0);
        return 1;
    }

    // Write a for loop that uses isdigit() to check if the entered port number is in fact a number.
    int is_number = 1;
    int l = strlen(argv[1]);
    for (int i = 0; i < l; i++) {
        if (!isdigit(argv[1][i])) {
            is_number = 0;
            break;
        }
    }

    int port = atoi(argv[1]);

    if (is_number == 0 || port < 1 || port > 65535) {
        custom_error(1, 0);
        return 1;
    }

    //printf("Port Number: %d\n", port);

    Listener_Socket sock;

    int li = listener_init(&sock, port);

    if (li < 0) {
        //printf("listener_init = %d\n", li);
        custom_error(1, 0);
        return 1;
    }
    //printf("listener_init succeeded\n");

    //    regex_t regex;
    //    regcomp(&regex, "[a-zA-Z0-9.-]", REG_EXTENDED);
    //    regcomp(&regex, "^(GET|PUT) (/[^ ]+) +(HTTP/[1][.][1+])", REG_EXTENDED);
    //    regcomp(&regex, "^([A-Za-z]+) (/[^ ]+) +(HTTP/[0-9]+.[0-9]+)", REG_EXTENDED);

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
                rq_line_size++;
            }
        }

        if (is_slash_r == 0 || is_slash_n == 0) {
            printf("invlaid req");
            return 1;
        }

        //int bytes_read_1 = read_until(connfd, rq, 2048, "\r\n");
        //printf("test: %s\n", rq);
        //printf("lol: %d\n", bytes_read_1);
        // If theres no \r\n separating the request line

        char m[9];
        char f[10];
        char h[9];
        regmatch_t matches[4];
        regex_t regex;
        regcomp(&regex, "^([A-Za-z]+) (/[^ ]+) +(HTTP/[0-9].[0-9]+)", REG_EXTENDED);

        // Following two lines are testers
        printf("Request Line: %s\n", rq);
        //char *file = NULL;

        if (regexec(&regex, rq, 4, matches, 0) == 0) {
            int m_len = matches[1].rm_eo - matches[1].rm_so;
            int f_len = matches[2].rm_eo - matches[2].rm_so;
            int h_len = matches[3].rm_eo - matches[3].rm_so;

            // h_len > 8 means that HTTP 1.1###...

            if (m_len > 8 || (f_len > 64) || (h_len > 8)) {
                printf("h_len = %d\n", h_len);
                custom_error(400, connfd);

		memset(rq, 0, sizeof(rq));
                regfree(&regex);

		continue;
            }

            strncpy(m, rq + matches[1].rm_so, m_len);
            m[m_len] = '\0';
            // + 1 to skip past the slash in /foo.txt

            strncpy(f, rq + matches[2].rm_so + 1, f_len - 1);
            f[f_len - 1] = '\0';

            strncpy(h, rq + matches[3].rm_so, h_len);
            h[h_len] = '\0';

        } else {
            memset(rq, 0, sizeof(rq));
            regfree(&regex);

            custom_error(400, connfd);
	    continue;
        }

	// Check if file is directory
	struct stat dir;
	stat(f, &dir);
	if (S_ISDIR(dir.st_mode)) {
	    printf("is dir");
	    memset(rq, 0, sizeof(rq));
            regfree(&regex);

	    custom_error(403, connfd);
	    continue;
	}

        // Read the rest
        char buf[BUF + 1];
        int bytes_read = 0;
        //char *token;
        //char *rest;
        // printf("test\n");

        int infile = 0;
        int outfile = 0;
        if (strcmp(m, "GET") == 0) {
            infile = open(f, O_RDWR);
            if (infile < 0) {
                custom_error(404, connfd);
		memset(rq, 0, sizeof(rq));
                memset(buf, 0, sizeof(buf));
                regfree(&regex);

		continue;
            }

            // do this to prevent errors
            bytes_read = read(connfd, buf, BUF);
            bytes_read = 0;

            outfile = connfd;

            char ok_msg[] = "HTTP/1.1 200 OK\r\nContent-Length: ";
            char rest[] = "\r\n\r\n";

            struct stat sb;
            stat(f, &sb);

            char len[10];
            sprintf(len, "%ld", sb.st_size);

            write(connfd, ok_msg, strlen(ok_msg));
            write(connfd, len, strlen(len));
            write(connfd, rest, strlen(rest));
        }

	else if (strcmp(m, "PUT") == 0) {
            outfile = open(f, O_RDWR | O_CREAT | O_TRUNC, 0644);
            if (outfile < 0) {
                printf("Error 404\n");
            }
            infile = connfd;
        }

	else {
	    blind_read(connfd);
	    close(connfd);
	    memset(rq, 0, sizeof(rq));
            memset(buf, 0, sizeof(buf));
            regfree(&regex);

	    continue;
	}

        //bytes_read = read(connfd, buf, BUF);
        //token = strtok_r(buf, "\r\n", &rest);

        //printf("Content: %s\n", token);
        printf("Function: %s\n", m);
        printf("File Name: %s\n\n", f);
        //fix this later. writing this one line without breaks can be too much
        //write(connfd, rest, bytes_read - strlen(token));

        char buf_2[BUF + 1];

        bytes_read = 0;
        printf("bytes_read: %d\n", bytes_read);

        // send 200 code

        do {
            bytes_read = read(infile, buf_2, BUF);
            if (bytes_read < 0) {
                printf("Error 400");
            }

            else {
                int bytes_written = 0;
                do {
                    int bytes = write(outfile, buf_2 + bytes_written, bytes_read - bytes_written);
                    if (bytes < 0) {
                        printf("Error 404");
                    }

                    bytes_written += bytes;
                } while (bytes_written < bytes_read);
            }
            memset(buf_2, 0, sizeof(buf_2));

        } while (bytes_read > 0);

        if (infile != 0) {
            close(infile);
        }

        if (outfile != 0) {
            close(outfile);
        }

        memset(rq, 0, sizeof(rq));
        memset(buf, 0, sizeof(buf));
        regfree(&regex);
    }

    return 0;
}
