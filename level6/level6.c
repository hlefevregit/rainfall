#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void n(void) {
    system("/bin/cat /home/user/level7/.pass");
}

void m(void) {
    puts("Nope");
}

int main(int argc, char **argv) {
    char *input_buffer = malloc(64);
    void (**function_pointer)(void) = malloc(sizeof(void *));

    *function_pointer = m;

    strcpy(input_buffer, argv[1]);

    (*function_pointer)();

    return 0;
}