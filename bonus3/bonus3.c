#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char password[65];      // Premier bloc lu depuis le fichier
    char message[66];       // Deuxième bloc lu depuis le fichier
    FILE *file;

    // Ouvre le fichier de mot de passe
    file = fopen("/home/user/end/.pass", "r");

    // Initialise les deux buffers à zéro (132 octets contigus)
    memset(password, 0, 132);

    // Vérifie que le fichier existe ET qu'on a exactement 1 argument
    if (file == NULL || argc != 2) {
        return -1;
    }

    // Lit 66 octets depuis le fichier dans password[]
    fread(password, 1, 0x42, file);
    password[64] = '\0';    // Force la terminaison (local_57 = 0)

    // Tronque password à la longueur donnée en argument
    int n = atoi(argv[1]);
    password[n] = '\0';     // ⚠️ Pas de vérification des bornes !

    // Lit 65 octets supplémentaires dans message[]
    fread(message, 1, 0x41, file);

    fclose(file);

    // Compare password (tronqué) avec argv[1]
    if (strcmp(password, argv[1]) == 0) {
        execl("/bin/sh", "sh", NULL);   // Shell si match !
    } else {
        puts(message);                  // Affiche un message sinon
    }

    return 0;
}