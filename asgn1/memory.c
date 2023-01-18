// Coded by Nihal Ernest
// CSE 130, Winter 2023
// Assignment 1

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER 4096

int main() {
    // Input takes in the mode (get/set) and file name
    char mode[3]; // PATH_MAX defined as the file name size limit
    char file_name[PATH_MAX];
    scanf("%s %s", mode, file_name);

    // Opening the file
    int get = open(file_name, O_RDONLY);
    if (get < 0) {
        printf("Invalid Filename\n");
        return 1;
    }

    // If the entered command was set
    if (strcmp(mode, "set") == 0) {
        char text[1024];
        scanf("%[^\n]", text);

        printf("File name: %s\n", file_name);
        printf("Text: %s\n", text);
    }

    // If the entered command was get
    else if (strcmp(mode, "get") == 0) {
        // The buffer stores each line of the file temporarily.
        char buffer[BUFFER];
        int bytes_read = 0;

        do {
            bytes_read = read(get, buffer + bytes_read, BUFFER); //Tested adding buffer and bytes_read
            if (bytes_read < 0) {
                fprintf(stderr, "Failed to read: %d\n", errno);
                return 1;
            }

            else if (bytes_read > 0) {

                int bytes_written = 0;
                do {
                    int bytes = write(STDOUT_FILENO, buffer + bytes_written, bytes_read - bytes_written); // Fix the same problem in read
                    if (bytes <= 0) {
                        fprintf(stderr, "Cannot write to STDOUT\n");
			return 1;
                    }

                    bytes_written += bytes;
                } while (bytes_written < bytes_read);
            }
        } while (bytes_read > 0);

        // Reads each line of the file and stores it in buffer.
        // Buffer is then printed
    } else {
        fprintf(stderr, "Invalid Command\n");
        return 1;
    }

    // No memory leaks?
    return 0;
}
