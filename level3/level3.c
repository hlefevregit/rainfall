#include <stdio.h>
#include <stdlib.h>

extern int m;

static void v(void) {
    char buf[520];

    fgets(buf, 512, stdin);
    printf(buf);   // vulnérabilité de format string

    if (m == 0x40) {
        fwrite("Wait what?!\n", 1, 12, stdout);
        system("/bin/sh");
    }
}

int main(void) {
    v();
    return 0;
}