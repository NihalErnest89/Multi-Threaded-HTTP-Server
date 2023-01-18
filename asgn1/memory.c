#include <stdio.h>
#include <string.h>
#include <limits.h>

int main() {
    // There should only be 1 command-line arguments.

    char input[PATH_MAX + 3];
    scanf("%[^\n]", input);
    char *mode = strtok(input, " ");
    char *file_name = strtok(NULL, " ");

    // Opening the file

    FILE *get = fopen(file_name, "r");
    if (get == NULL) {
        printf("File does not exist\n");
        return 0;
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
