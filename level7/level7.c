#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char c[68];  // buffer global

typedef struct {
    int type;
    char *data;
} Item;

void m(void) {
    time_t now = time(NULL);
    printf("%s - %ld\n", c, (long)now);
}

int main(int argc, char **argv) {
    Item *a;
    Item *b;
    FILE *fp;

    a = malloc(sizeof(Item));
    a->type = 1;
    a->data = malloc(8);

    b = malloc(sizeof(Item));
    b->type = 2;
    b->data = malloc(8);

    strcpy(a->data, argv[1]);
    strcpy(b->data, argv[2]);

    fp = fopen("/home/user/level8/.pass", "r");
    fgets(c, 0x44, fp);

    puts("~~");
    return 0;
}