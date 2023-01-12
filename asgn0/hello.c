#include <unistd.h>

int main() {
    write(STDOUT_FILENO, "Hello World!\n", sizeof("Hello World!\n") - 1);
    return 0;
}
