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

// Fonction qui mange les gums pour Pac-Man
void eat(char **level, int x, int y) {
    if (level[y][x] == '.') {
        level[y][x] = ' ';
    }
}

// Fonction pour vérifier si le niveau est gagné
bool win(char **level, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (level[i][j] == '.') {
                return false;
            }
        }
    }
    return true;
}

// Fonction pour vérifier si Pac-Man a perdu
bool defeat(int pacmanX, int pacmanY, Ghost *ghost) {
    return pacmanX == ghost->pos.x && pacmanY == ghost->pos.y;
}

// Fonction pour obtenir les directions possibles pour un fantôme
Coord *getPotentialDirections(char **level, Ghost *ghost, int *nbDir) {
    static Coord directions[4] = {
            {1, 0},
            {-1, 0},
            {0, -1},
            {0, 1}
    };

    Coord *potentialMoves = malloc(4 * sizeof(Coord));
    *nbDir = 0;

    for (int i = 0; i < 4; i++) {
        int newX = ghost->pos.x + directions[i].x;
        int newY = ghost->pos.y + directions[i].y;

        if (newX >= 0 && newY >= 0 && newX < 28 && newY < 31 &&
            level[newY][newX] != 'H' &&
            !(directions[i].x == -ghost->dir.x && directions[i].y == -ghost->dir.y)) {
            if (*nbDir < 4) {  // Empêche l'écriture hors limite
                potentialMoves[(*nbDir)++] = (Coord){newX, newY};
            }
        }
    }

    return potentialMoves;
}

void ghostMove(char **level, Ghost *ghost) {
    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, ghost, &nbMoves);

    if (nbMoves > 0) {
        int randomIndex = rand() % nbMoves;
        Coord newPos = potentialMoves[randomIndex];
        ghost->dir.x = newPos.x - ghost->pos.x;
        ghost->dir.y = newPos.y - ghost->pos.y;
        ghost->pos = newPos;
    }

    free(potentialMoves);
}

int dirX = 0, dirY = 0;
int nextDirX = 0, nextDirY = 0;

int main(int argc, char *argv[]) {
    RendererParameters params;
    Textures textures;
    char **level;

    printf("Initialisation du framework...\n");
    init(&params, &textures, 587, 652, 5);

    if (params.renderer == NULL || params.window == NULL) {
        printf("Erreur : Impossible d'initialiser le renderer ou la fenêtre : %s\n", SDL_GetError());
        return 1;
    }

    printf("Chargement des textures...\n");
    textures.texturePacman = GetTexture("pacman.bmp", &params);
    textures.textureDot = GetTexture("dot.bmp", &params);
    textures.textureWall = GetTexture("wall.bmp", &params);
    textures.textureClyde = GetTexture("clyde.bmp", &params);

    if (textures.texturePacman == NULL || textures.textureDot == NULL || textures.textureWall == NULL || textures.
            textureClyde == NULL) {
        printf("Erreur : Impossible de charger les textures : %s\n", SDL_GetError());
        return 1;
    }

    loadFirstLevel(&level);

    int running = 1;
    int pacmanX = 23;
    int pacmanY = 14;

    Ghost clyde = {
            .pos = {13, 14},
            .dir = {1, 0}
    };

    printf("Entrée dans la boucle principale...\n");

    while (running) {
        int input = getInput();

        int newX = pacmanX;
        int newY = pacmanY;

        switch (input) {
            case SDLK_ESCAPE:
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

        if (pacmanY + nextDirY >= 0 && pacmanY + nextDirY < 31 &&
            pacmanX + nextDirX >= 0 && pacmanX + nextDirX < 28 &&
            level[pacmanY + nextDirY][pacmanX + nextDirX] != 'H') {
            dirX = nextDirX;
            dirY = nextDirY;
        }

        newX = pacmanX + dirX;
        newY = pacmanY + dirY;

        if (newY >= 0 && newY < 31 && newX >= 0 && newX < 28 &&
            level[newY][newX] != 'H') {
            pacmanX = newX;
            pacmanY = newY;
            eat(level, pacmanX, pacmanY);
        }

        ghostMove(level, &clyde);

        if (win(level, 31, 28)) {
            printf("Vous avez gagné !\n");
            running = 0;
        } else if (defeat(pacmanX, pacmanY, &clyde)) {
            printf("Vous avez perdu !\n");
            running = 0;
        }

        drawLevel(level, 31, 28, &params, &textures);
        drawSpriteOnGrid(textures.texturePacman, pacmanX, pacmanY, 0, &params);
        drawSpriteOnGrid(textures.textureClyde, clyde.pos.x, clyde.pos.y, 0, &params);
        update(&params);
    }

    printf("Libération des ressources...\n");

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
