#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// argc = param_1, argv = param_2
int main(int argc, char **argv)
{
    char buffer[40];   // Buffer de 40 octets sur la stack
    int n;

    n = atoi(argv[1]);        // Convertit le 1er argument en entier

    if (n < 10) {
        // Copie n*4 octets depuis argv[2] dans buffer[40]
        memcpy(buffer, argv[2], n * 4);

        // Vérifie si n vaut magiquement 0x574f4c46 ("WOLF" en ASCII)
        if (n == 0x574f4c46) {
            execl("/bin/sh", "sh", NULL);   // Spawn un shell
        }

        return 0;
    }
    else {
        return 1;
    }
}