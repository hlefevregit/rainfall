#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int language = 0; // Variable globale

#include <stdio.h>
#include <string.h>

// Buffers contigus sur la stack :
// [local_4c : 4 bytes][local_48 : 4 bytes][local_44 : 64 bytes]
// = un buffer logique de 72 octets

void greetuser(void)
{
    char greeting[4];       // "Hell" / "Hyv" / "Goed"
    char greeting2[4];      // "o " / suite UTF-8 / "emid"
    char username[64];      // copié depuis main (le firstname)

    if (language == 1) {
        // Finnois : "Hyvää päivää <username>"
        greeting[0] = 'H';
        greeting[1] = 'y';
        greeting[2] = 'v';
        greeting[3] = 0xC3;   // début UTF-8 'ä'
        greeting2[0] = 0xA4;  // suite UTF-8 'ä'
        greeting2[1] = 0xC3;
        greeting2[2] = 0xA4;
        greeting2[3] = ' ';
        strncpy(username, "päivää ", 11);
    }
    else if (language == 2) {
        // Néerlandais : "Goedemiddag <username>"
        strncpy(greeting,  "Goed", 4);
        greeting2[0] = 'e';
        greeting2[1] = 'm';
        greeting2[2] = 'i';
        greeting2[3] = 'd';
        strncpy(username, "dag! ", 6);
    }
    else {
        // Anglais : "Hello <username>"
        strncpy(greeting, "Hell", 4);
        greeting2[2] = 'o';
        greeting2[3] = ' ';
    }

    // Concatène le username (copié depuis main) à la suite du greeting
    // username est à &stack0x00000004 = juste après greeting[4]
    strcat(greeting, username);   // ⚠️ strcat dans un buffer de 4 octets !
    puts(greeting);
}


int main(int argc, char **argv)
{
    char firstname[40];
    char lastname[36];
    char *lang_env;

    // Vérifie qu'on a exactement 2 arguments
    if (argc != 3) {
        return 1;
    }

    // Initialise les deux buffers à zéro (équivalent memset 0)
    memset(firstname, 0, 76);   // 40 + 36 = 76 octets contigus

    // Copie les arguments dans les buffers
    strncpy(firstname, argv[1], 0x28);  // max 40 chars
    strncpy(lastname,  argv[2], 0x20);  // max 32 chars

    // Détecte la langue via la variable d'environnement LANG
    lang_env = getenv("LANG");
    if (lang_env != NULL) {
        if (memcmp(lang_env, &DAT_0804873d, 2) == 0) {
            language = 1;   // "fr" probablement
        }
        else if (memcmp(lang_env, &DAT_08048740, 2) == 0) {
            language = 2;   // "nl", "de", ou autre
        }
    }

    // Copie firstname dans un buffer sur la stack (plus bas dans la stack)
    // via une boucle qui copie 0x13 (19) blocs de 4 octets = 76 octets
    // destination : stack0xffffff50 (zone plus basse que les buffers locaux)
    char dest[76];
    memcpy(dest, firstname, 76);

    return greetuser();
}