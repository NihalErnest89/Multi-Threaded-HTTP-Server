// Asgn 2: A simple HTTP server.
// By: Eugene Chou
//     Andrew Quinn
//     Brian Zhao

#include "asgn2_helper_funcs.h"
#include "connection.h"
#include "debug.h"
#include "response.h"
#include "request.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#include <pthread.h>

#define OPTIONS "t: "

void handle_connection(int);

void handle_get(conn_t *);
void handle_put(conn_t *);
void handle_unsupported(conn_t *);

int worker_threads() {
    printf("Go to uglag\n");
	return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int opt = 0;
    int threads = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		switch(opt) {
			case 't':
			    threads = atoi(optarg);
				break;
			default:
			    threads = 4;
				break;
		}
	}

	printf("Threads: %d\n", threads);

    char *endptr = NULL;
    size_t port = (size_t) strtoull(argv[3], &endptr, 10);
    if (endptr && *endptr != '\0') {
        warnx("invalid port number: %s", argv[3]);
        return EXIT_FAILURE;
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket sock;
    listener_init(&sock, port);

    pthread_t wt[threads];
	
	for (int i = 0; i < threads; i ++) {
        pthread_create(&wt[i], NULL, (void *(*)(void*))worker_threads, NULL);
	    printf("Thread %d created\n", i);
	}

    while (1) {
        int connfd = listener_accept(&sock);
        handle_connection(connfd);
        close(connfd);
    }

    return EXIT_SUCCESS;
}



void handle_connection(int connfd) {

    conn_t *conn = conn_new(connfd);

    const Response_t *res = conn_parse(conn);

    if (res != NULL) {
        conn_send_response(conn, res);
    } else {
        debug("%s", conn_str(conn));
        const Request_t *req = conn_get_request(conn);
        if (req == &REQUEST_GET) {
            handle_get(conn);
        } else if (req == &REQUEST_PUT) {
            handle_put(conn);
        } else {
            handle_unsupported(conn);
        }
    }

    conn_delete(&conn);
}

void handle_get(conn_t *conn) {

    //debug("GET request not implemented. But, we want to get %s", uri);

    // What are the steps in here?

    // 1. Open the file.
    // If  open it returns < 0, then use the result appropriately
    //   a. Cannot access -- use RESPONSE_FORBIDDEN
    //   b. Cannot find the file -- use RESPONSE_NOT_FOUND
    //   c. other error? -- use RESPONSE_INTERNAL_SERVER_ERROR
    // (hint: check errno for these cases)!

    // 2. Get the size of the file.
    // (hint: checkout the function fstat)!

    // Get the size of the file.

    // 3. Check if the file is a directory, because directories *will*
    // open, but are not valid.
    // (hint: checkout the macro "S_IFDIR", which you can use after you call fstat!)


    // 4. Send the file
    // (hint: checkout the conn_send_file function!)
    char *uri = conn_get_uri(conn);

    const Response_t *res = NULL;
    debug("handling put request for %s", uri);
 
     // Check if file already exists before opening it.
    bool existed = access(uri, F_OK) == 0;
    debug("%s existed? %d", uri, existed);

    // Open the file..
	int fd = open(uri, O_RDONLY, 0666);
    if (fd < 0) {
		if (errno == EACCES) {
			res = &RESPONSE_FORBIDDEN;
			conn_send_response(conn, res);
		}
		else if (errno == ENOENT) {
			res = &RESPONSE_NOT_FOUND;
			conn_send_response(conn, res);
		}
		else {
			res = &RESPONSE_INTERNAL_SERVER_ERROR;
		    conn_send_response(conn, res);
		}

	}

	struct stat sb;
	fstat(fd, &sb);

	int content_length = sb.st_size;

    
	struct stat dir;
	stat(uri, &dir);
	
	if (S_ISDIR(dir.st_mode)) {
		conn_send_response(conn, &RESPONSE_FORBIDDEN);
	}

	conn_send_file(conn, fd, content_length);

    close(fd);

}

void handle_unsupported(conn_t *conn) {
    debug("handling unsupported request");

    // send responses
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
}

void handle_put(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    debug("handling put request for %s", uri);

    // Check if file already exists before opening it.
    bool existed = access(uri, F_OK) == 0;
    debug("%s existed? %d", uri, existed);

    // Open the file..
    int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) {
        debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            goto out;
        }
    }

    res = conn_recv_file(conn, fd);

    if (res == NULL && existed) {
        res = &RESPONSE_OK;
    } else if (res == NULL && !existed) {
        res = &RESPONSE_CREATED;
    }

    close(fd);

out:
    conn_send_response(conn, res);
}
