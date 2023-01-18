#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Invalid Command\n");
	}



	// Opening the file
	FILE *get = fopen(argv[2], "r");
	if (get == NULL) {
		printf("File does not exist\n");
		return 0;
	}

	if (strcmp(argv[1], "set") == 0) {
		char text[1024];
		scanf("%[^\n]", text);

		printf("File name: %s\n", argv[2]);
		printf("Text: %s\n", text);
	}
	else if (strcmp(argv[1], "get") == 0) {
		char buffer[1024];
		while (fscanf(get, "%[^\n] ", buffer) != EOF) {
			printf("%s\n", buffer);
		}
	}
	fclose(get);
	return 0;
}
