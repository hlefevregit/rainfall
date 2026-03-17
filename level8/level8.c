#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *auth = NULL;
char *service = NULL;

int main(void) {
    char input[128];

    while (1) {
        printf("%p, %p\n", auth, service);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        if (strncmp(input, "auth ", 5) == 0) {
            auth = malloc(4);
            if (auth != NULL) {
                memset(auth, 0, 4);

                // copie ce qui suit "auth "
                if (strlen(input + 5) < 31) {
                    strcpy(auth, input + 5);
                }
            }
        }

        if (strncmp(input, "reset", 5) == 0) {
            free(auth);
        }

        if (strncmp(input, "service", 6) == 0) {
            service = strdup(input + 7);
        }

        if (strncmp(input, "login", 5) == 0) {
            if (*(int *)(auth + 32) == 0) {
                fwrite("Password:\n", 1, 10, stdout);
            } else {
                system("/bin/sh");
            }
        }
    }
}