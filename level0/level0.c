#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char **argv) {
    if (atoi(argv[1]) == 0x1a7) {   // 0x1a7 = 423
        char *path = strdup("/bin/sh");
        char *args[] = { path, NULL };

        gid_t egid = getegid();
        uid_t euid = geteuid();

        setresgid(egid, egid, egid);
        setresuid(euid, euid, euid);

        execv(path, args);
    } else {
        fwrite("No !\n", 1, 5, stderr);
    }

    return 0;
}