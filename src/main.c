#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdbool.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "firstLevel.h"
#include <time.h>

// Structure pour représenter une position sur la grille
typedef struct {
    int x;
    int y;
} Coord;

// Structure pour représenter un fantôme
typedef struct {
    Coord pos;
    Coord dir;
} Ghost;

// Variable pour le score de Pac-Man
int score = 0;

// Fonction qui mange les gums pour Pac-Man
void eat(char **level, int x, int y) {
    if (level[y][x] == '.') {
        level[y][x] = ' ';
        score += 10; // Chaque gum rapporte 10 points
    }
}

// Fonction pour afficher le score actuel
void displayScore() {
    printf("Score actuel : %d\n", score);
}

// Fonction pour vérifier si le niveau est gagné
bool win(char **level, int rows, int cols) {
    int i = 0;
    while (i < rows * cols) {
        if (level[i / cols][i % cols] == '.') {
            return false; // Il reste des gums à manger
        }
        i++;
    }
    return true; // Tous les gums ont été mangés
}

// Fonction pour vérifier si Pac-Man a perdu
bool defeat(int pacmanX, int pacmanY, Ghost *ghost) {
    return pacmanX == ghost->pos.x && pacmanY == ghost->pos.y;
}

// Fonction pour obtenir les directions possibles pour un fantôme
Coord *getPotentialDirections(char **level, Ghost *ghost, int *nbDir) {
    static Coord directions[4] = {
            {1, 0},  // Droite
            {-1, 0}, // Gauche
            {0, -1}, // Haut
            {0, 1}   // Bas
    };

    Coord *potentialMoves = malloc(4 * sizeof(Coord));
    *nbDir = 0;

    for (int i = 0; i < 4; i++) {
        int newX = ghost->pos.x + directions[i].x;
        int newY = ghost->pos.y + directions[i].y;

        // Vérifier que la direction est dans les limites et non bloquée
        if (newX >= 0 && newY >= 0 && newX < 28 && newY < 31 &&
            level[newY][newX] != 'H' &&
            !(directions[i].x == -ghost->dir.x && directions[i].y == -ghost->dir.y)) {
            potentialMoves[(*nbDir)++] = (Coord){newX, newY};
        }
    }

    return potentialMoves;
}

// Fonction pour déplacer le fantôme
void ghostMove(char **level, Ghost *ghost) {
    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, ghost, &nbMoves);

    if (nbMoves == 0) {
        // Cul-de-sac : faire demi-tour
        ghost->dir.x = -ghost->dir.x;
        ghost->dir.y = -ghost->dir.y;
        ghost->pos.x += ghost->dir.x;
        ghost->pos.y += ghost->dir.y;
    } else if (nbMoves == 1) {
        // Une seule direction possible : avancer
        ghost->dir.x = potentialMoves[0].x - ghost->pos.x;
        ghost->dir.y = potentialMoves[0].y - ghost->pos.y;
        ghost->pos = potentialMoves[0];
    } else {
        // Plusieurs directions : choisir aléatoirement
        int randomIndex = rand() % nbMoves;
        ghost->dir.x = potentialMoves[randomIndex].x - ghost->pos.x;
        ghost->dir.y = potentialMoves[randomIndex].y - ghost->pos.y;
        ghost->pos = potentialMoves[randomIndex];
    }

    free(potentialMoves);
}

// Variables globales pour les directions de Pac-Man
int dirX = 0, dirY = 0;
int nextDirX = 0, nextDirY = 0;

int main(int argc, char *argv[]) {
    RendererParameters params;
    Textures textures;
    char **level;

    // Initialisation des paramètres du framework
    printf("Initialisation du framework...\n");
    init(&params, &textures, 587, 652, 5);

    if (params.renderer == NULL || params.window == NULL) {
        printf("Erreur : Impossible d'initialiser le renderer ou la fenêtre : %s\n", SDL_GetError());
        return 1;
    }

    // Chargement des textures
    printf("Chargement des textures...\n");
    textures.texturePacman = GetTexture("pacman.bmp", &params);
    textures.textureDot = GetTexture("dot.bmp", &params);
    textures.textureWall = GetTexture("wall.bmp", &params);
    textures.textureClyde = GetTexture("clyde.bmp", &params);

    if (textures.texturePacman == NULL || textures.textureDot == NULL || textures.textureWall == NULL ||
        textures.textureClyde == NULL) {
        printf("Erreur : Impossible de charger les textures : %s\n", SDL_GetError());
        return 1;
    }

    // Chargement du niveau initial
    loadFirstLevel(&level);

    int running = 1;
    int pacmanX = 23; // Position initiale de Pac-Man (x)
    int pacmanY = 14; // Position initiale de Pac-Man (y)

    // Initialisation du fantôme Clyde
    Ghost clyde = {
            .pos = {13, 14},
            .dir = {1, 0}
    };

    printf("Entrée dans la boucle principale...\n");

    while (running) {
        int input = getInput();

        // Gestion des entrées utilisateur pour Pac-Man
        switch (input) {
            case SDLK_ESCAPE: // Fermeture du jeu si Échap est pressé
                running = 0;
                break;
            case SDLK_UP:
                nextDirX = 0;
                nextDirY = -1;
                break;
            case SDLK_DOWN:
                nextDirX = 0;
                nextDirY = 1;
                break;
            case SDLK_LEFT:
                nextDirX = -1;
                nextDirY = 0;
                break;
            case SDLK_RIGHT:
                nextDirX = 1;
                nextDirY = 0;
                break;
        }

        // Validation de la prochaine direction
        if (pacmanY + nextDirY >= 0 && pacmanY + nextDirY < 31 &&
            pacmanX + nextDirX >= 0 && pacmanX + nextDirX < 28 &&
            level[pacmanY + nextDirY][pacmanX + nextDirX] != 'H') {
            dirX = nextDirX;
            dirY = nextDirY;
        }

        // Mise à jour de la position de Pac-Man
        int newX = pacmanX + dirX;
        int newY = pacmanY + dirY;

        if (newY >= 0 && newY < 31 && newX >= 0 && newX < 28 &&
            level[newY][newX] != 'H') {
            pacmanX = newX;
            pacmanY = newY;
            eat(level, pacmanX, pacmanY); // Pac-Man mange une pacgum
        }

        // Gestion des téléportations de Pac-Man aux bords
        if (pacmanX < 0) pacmanX = 27; // Passage du côté gauche au droit
        if (pacmanX > 27) pacmanX = 0; // Passage du côté droit au gauche
        if (pacmanY < 0) pacmanY = 30; // Passage du haut vers le bas
        if (pacmanY > 30) pacmanY = 0; // Passage du bas vers le haut

        // Déplacement du fantôme Clyde
        ghostMove(level, &clyde);

        // Affichage du score actuel
        displayScore();

        // Vérification des conditions de victoire
        if (win(level, 31, 28)) {
            printf("Vous avez gagné !\n");
            running = 0;
        }

            // Vérification des conditions de défaite
        else if (defeat(pacmanX, pacmanY, &clyde)) {
            printf("Vous avez perdu !\n");
            running = 0;
        }

        // Rendu des sprites et mise à jour
        drawLevel(level, 31, 28, &params, &textures);
        drawSpriteOnGrid(textures.texturePacman, pacmanX, pacmanY, 0, &params);
        drawSpriteOnGrid(textures.textureClyde, clyde.pos.x, clyde.pos.y, 0, &params);
        update(&params);
    }

    printf("Libération des ressources...\n");

    // Libération des ressources allouées pour le niveau
    for (int i = 0; i < 31; i++) {
        if (level[i] != NULL) {
            free(level[i]);
        }
    }
    free(level);

    SDL_DestroyTexture(textures.texturePacman);
    SDL_DestroyTexture(textures.textureClyde);
    SDL_DestroyTexture(textures.textureDot);
    SDL_DestroyTexture(textures.textureWall);
    SDL_DestroyRenderer(params.renderer);
    SDL_DestroyWindow(params.window);
    SDL_Quit();

    printf("Fermeture du programme.\n");
    return 0;
}
