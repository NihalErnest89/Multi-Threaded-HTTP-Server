// Coded by Nihal Ernest
// CSE 130, Winter 2023
// Assignment 1

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER 4096

int main() {
    // Input takes in the mode (get/set) and file name
    //    char mode[3]; // PATH_MAX defined as the file name size limit
    //    char file_name[PATH_MAX];

    char temp[PATH_MAX + 4];
    int br = 0;

    // read it all into buffer
    // process the buffer character by character
    	br = read(STDIN_FILENO, temp, BUFFER);
	
	if (strlen(temp) <= 4 || temp[4] == ' ') {
	    fprintf(stderr, "Invalid Command\n");
	    return 1;
	}
	


	// Check for newline
	if (strchr(temp, '\n') == NULL) {
//	    printf("%s!", temp);
//	    printf("apparently there is no newline\n");
	    fprintf(stderr, "Invalid Command\n");
	    return 1;
	}

        char *mode = strtok(temp, " ");


        char *file_name = strtok(NULL, "\n");	


	if (br < 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }



//	printf("test with:%s!\n", mode);


//    scanf("%[^\n]", temp);



    // If the entered command was set
    if (strcmp(mode, "set") == 0) {
        char text[BUFFER];

        int set = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
        int bytes_read = 0;

        if (set < 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        do {
            bytes_read = read(STDIN_FILENO, text, BUFFER);
            if (bytes_read < 0) {
                fprintf(stderr, "Invalid Command\n");
                return 1;
            }

            else if (bytes_read > 0) {
                int bytes_written = 0;
                do { //(bytes_written < bytes_read) {
                    int bytes = write(set, text + bytes_written, bytes_read - bytes_written);
                    if (bytes <= 0) {
                        fprintf(stderr, "Invalid Command\n");
                        return 1;
                    }
                    bytes_written += bytes;
                } while (bytes_written < bytes_read);
            }
        } while (bytes_read > 0);
        fprintf(stdout, "OK\n");
        close(set);
    }

    // If the entered command was get
    else if (strcmp(mode, "get") == 0) {
        // The buffer stores each line of the file temporarily.

        int get = open(file_name, O_RDONLY);
        if (get < 0) {
            fprintf(stderr, "Invalid Command\n");
            return 1;
        }

        char buffer[BUFFER];
        int bytes_read = 0;
        do {
            bytes_read = read(get, buffer, BUFFER); //Tested adding buffer and bytes_read
            if (bytes_read < 0) {
                fprintf(stderr, "Invalid Command\n");
                return 1;
            }

            else if (bytes_read > 0) {

                int bytes_written = 0;
                do {
                    int bytes = write(STDOUT_FILENO, buffer + bytes_written,
                        bytes_read - bytes_written); // Fix the same problem in read
                    if (bytes <= 0) {
                        fprintf(stderr, "Cannot write to STDOUT\n");
                        return 1;
                    }

                    bytes_written += bytes;
                } while (bytes_written < bytes_read);
            }
        } while (bytes_read > 0);
        close(get);
        // Reads each line of the file and stores it in buffer.
        // Buffer is then printed
    } else {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    // No memory leaks?
    return 0;
}
