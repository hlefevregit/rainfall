#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Affiche un prompt, lit une ligne depuis stdin, et la copie (tronquée à 20 chars) dans 'dest'
void read_input(char *dest, char *prompt)
{
    char buffer[4104];

    puts(prompt);
    read(0, buffer, 0x1000);          // Lit jusqu'à 4096 octets depuis stdin

    char *newline = strchr(buffer, '\n');
    *newline = '\0';                  // Supprime le '\n' final

    strncpy(dest, buffer, 20);        // Copie max 20 caractères dans dest
}

// Demande un prénom et un nom, puis construit "prénom nom" dans 'full_name'
void read_full_name(char *full_name)
{
    char first_name[20];
    char last_name[20];

    read_input(first_name, &DAT_080486a0);   // Prompt pour le prénom
    read_input(last_name,  &DAT_080486a0);   // Prompt pour le nom

    // Copie le prénom dans full_name
    strcpy(full_name, first_name);

    // Calcule la longueur du prénom avec strlen "manuel" (équivalent à strlen)
    // puis ajoute un espace après le prénom
    int len = strlen(full_name);
    full_name[len]     = ' ';
    full_name[len + 1] = '\0';

    // Concatène le nom de famille
    strcat(full_name, last_name);
}

int main(void)
{
    char full_name[54];

    read_full_name(full_name);
    puts(full_name);         // Affiche "prénom nom"

    return 0;
}

// shellcode \x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80