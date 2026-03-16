#include <stdio.h>

void o(void)
{
    system("/bin/sh");
                    /* WARNING: Subroutine does not return */
    _exit(1);
}

void n(void)
{
    char ret [520];

    fgets(ret, 512 , stdin);
    printf(ret);
                    /* WARNING: Subroutine does not return */
    exit(1);
}

void main(void)
{
    n();
    return;
}
