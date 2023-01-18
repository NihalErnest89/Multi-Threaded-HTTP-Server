#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    // There should only be 2 command-line arguments.
    if (argc != 3) {
        fprintf(stderr, "Invalid Command\n");
        return 0
    }

    // Opening the file
    FILE *get = fopen(argv[2], "r");
    if (get == NULL) {
        printf("File does not exist\n");
        return 0;
    }

    // If the entered command was set
    if (strcmp(argv[1], "set") == 0) {
        char text[1024];
        scanf("%[^\n]", text);

        printf("File name: %s\n", argv[2]);
        printf("Text: %s\n", text);
    }

    // If the entered command was get
    else if (strcmp(argv[1], "get") == 0) {
        // The buffer stores each line of the file temporarily.
        char buffer[1024];

        // Reads each line of the file and stores it in buffer.
        // Buffer is then printed
        while (fscanf(get, "%[^\n] ", buffer) != EOF) {
            printf("%s\n", buffer);
        }
    }

    // No memory leaks?
    fclose(get);
    return 0;
}
