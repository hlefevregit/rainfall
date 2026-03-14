#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void p(void) {
    char buf[76];

    fflush(stdout);
    gets(buf);   // dangereux : aucune limite de taille

    unsigned int ret = (unsigned int)__builtin_return_address(0);

    if ((ret & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n", (void *)ret);
        _exit(1);
    }

    puts(buf);
    strdup(buf);
}

int main(void) {
    p();
    return 0;
}