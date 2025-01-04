#include "firstLevel.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void loadFirstLevel(char ***tab) {
    *tab = malloc(sizeof(char *) * 31);
    if (!*tab) {
        printf("Erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 31; i++) {
        (*tab)[i] = malloc(sizeof(char) * 28);
        if (!(*tab)[i]) {
            printf("Erreur allocation mémoire ligne %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    // Chargement de la carte
    strcpy((*tab)[0], "HHHHHHHHHHHHHHHHHHHHHHHHHHHH");
}
