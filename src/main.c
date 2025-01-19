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
#define INKY_RANDOMNESS 3
#define BLINKY_RANDOMNESS 4  // Ajustable pour le rendre plus ou moins imprévisible



// Structure pour représenter une position sur la grille
typedef struct {
    int x;
    int y;
} Coord;

typedef enum {
    CLYDE,
    PINKY,
    INKY,
    BLINKY
} GhostName;

// Structure pour représenter un fantôme
typedef struct {
    Coord pos;
    Coord dir;
    GhostName name;
    SDL_Texture *texture;
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

// Fonction pour vérifier s'il y a un mur entre deux positions
bool murEntre(Coord from, Coord to, char **level) {
    // Si les deux points sont sur la même ligne
    if (from.x == to.x) {
        int minY = from.y < to.y ? from.y : to.y;
        int maxY = from.y > to.y ? from.y : to.y;
        for (int y = minY + 1; y < maxY; y++) {
            if (level[y][from.x] == 'H') {
                return true; // Un mur bloque la vue
            }
        }
    }
        // Si les deux points sont sur la même colonne
    else if (from.y == to.y) {
        int minX = from.x < to.x ? from.x : to.x;
        int maxX = from.x > to.x ? from.x : to.x;
        for (int x = minX + 1; x < maxX; x++) {
            if (level[from.y][x] == 'H') {
                return true; // Un mur bloque la vue
            }
        }
    }
    return false; // Pas de mur entre les deux points
}

// Fonction pour trouver la prochaine intersection que rencontrera Pac-Man
Coord getPacmanNextIntersec(char **level, Coord pacmanPos, Coord pacmanDir) {
    static int recursionDepth = 0;
    if (recursionDepth > 50) {  // Limite pour éviter une boucle infinie
        recursionDepth = 0;
        return pacmanPos;
    }

    recursionDepth++;

    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, &(Ghost){pacmanPos, pacmanDir, BLINKY, NULL}, &nbMoves);

    if (nbMoves > 2 || nbMoves == 0) {
        free(potentialMoves);
        recursionDepth = 0;
        return pacmanPos;
    }

    free(potentialMoves);

    Coord nextPos = {pacmanPos.x + pacmanDir.x, pacmanPos.y + pacmanDir.y};

    if (nextPos.x < 0) nextPos.x = 27;
    if (nextPos.x > 27) nextPos.x = 0;
    if (nextPos.y < 0) nextPos.y = 30;
    if (nextPos.y > 30) nextPos.y = 0;

    return getPacmanNextIntersec(level, nextPos, pacmanDir);
}




// Fonction pour déplacer Clyde (déplacement aléatoire)
void clydeMove(char **level, Ghost *ghost) {
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
        ghost->dir = (Coord){
                potentialMoves[0].x - ghost->pos.x,
                potentialMoves[0].y - ghost->pos.y
        };
        ghost->pos = potentialMoves[0];
    } else {
        // Choisir une direction aléatoire
        srand(time(NULL)); // Initialiser le générateur aléatoire
        int randomIndex = rand() % nbMoves;
        ghost->dir = (Coord){
                potentialMoves[randomIndex].x - ghost->pos.x,
                potentialMoves[randomIndex].y - ghost->pos.y
        };
        ghost->pos = potentialMoves[randomIndex];
    }

    free(potentialMoves);
}

// Fonction pour déplacer Pinky (poursuite de Pac-Man si visible)
void pinkyMove(char **level, Ghost *ghost, Coord pacmanPos) {
    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, ghost, &nbMoves);

    if ((ghost->pos.x == pacmanPos.x || ghost->pos.y == pacmanPos.y) &&
        !murEntre(ghost->pos, pacmanPos, level)) {
        ghost->dir = (Coord){
                pacmanPos.x > ghost->pos.x ? 1 : (pacmanPos.x < ghost->pos.x ? -1 : 0),
                pacmanPos.y > ghost->pos.y ? 1 : (pacmanPos.y < ghost->pos.y ? -1 : 0)
        };
        ghost->pos.x += ghost->dir.x;
        ghost->pos.y += ghost->dir.y;
    } else {
        clydeMove(level, ghost);
    }

    free(potentialMoves);  // Ajout pour éviter une fuite mémoire
}


void inkyMove(char **level, Ghost *ghost, Coord pacmanPos) {
    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, ghost, &nbMoves);

    if (nbMoves == 0) {
        // Inky est coincé, il fait demi-tour
        ghost->dir.x = -ghost->dir.x;
        ghost->dir.y = -ghost->dir.y;
        ghost->pos.x += ghost->dir.x;
        ghost->pos.y += ghost->dir.y;
    } else {
        srand(time(NULL));
        if (rand() % INKY_RANDOMNESS == 0) {
            // 🔀 1 chance sur INKY_RANDOMNESS de prendre une direction aléatoire
            int randomIndex = rand() % nbMoves;
            ghost->dir = (Coord){
                    potentialMoves[randomIndex].x - ghost->pos.x,
                    potentialMoves[randomIndex].y - ghost->pos.y
            };
            ghost->pos = potentialMoves[randomIndex];
        } else {
            // 📌 7.1, 7.2, 7.3 : Trouver la meilleure direction selon Pac-Man

            int dx = pacmanPos.x - ghost->pos.x;
            int dy = pacmanPos.y - ghost->pos.y;

            int bestIndex = -1;
            int minDist = 9999;  // Grande valeur par défaut

            // 🔍 Parcourir les directions possibles et choisir la meilleure
            for (int i = 0; i < nbMoves; i++) {
                int moveDx = potentialMoves[i].x - ghost->pos.x;
                int moveDy = potentialMoves[i].y - ghost->pos.y;

                int newDx = abs(dx - moveDx);
                int newDy = abs(dy - moveDy);

                int dist = newDx + newDy;

                // 📌 7.1 : Priorité à réduire la plus grande différence (horizontale ou verticale)
                if (abs(dx) > abs(dy) && moveDx != 0) {
                    // Si la distance horizontale est plus grande, on privilégie un déplacement horizontal
                    bestIndex = i;
                    break;  // On a trouvé la meilleure option, on s'arrête ici
                } else if (abs(dy) > abs(dx) && moveDy != 0) {
                    // Sinon, si la distance verticale est plus grande, on privilégie un déplacement vertical
                    bestIndex = i;
                    break;
                } else if (dist < minDist) {
                    // Si aucune direction ne rapproche vraiment Inky, on prend la plus courte
                    minDist = dist;
                    bestIndex = i;
                }
            }

            //  7.3 : Si aucune direction ne rapproche de Pac-Man, choisir une au hasard parmi les possibles
            if (bestIndex == -1) {
                bestIndex = rand() % nbMoves;
            }

            // Appliquer le meilleur mouvement trouvé
            ghost->dir = (Coord){
                    potentialMoves[bestIndex].x - ghost->pos.x,
                    potentialMoves[bestIndex].y - ghost->pos.y
            };
            ghost->pos = potentialMoves[bestIndex];
        }
    }

    free(potentialMoves);
}


void blinkyMove(char **level, Ghost *ghost, Coord pacmanPos, Coord pacmanDir) {
    int nbMoves;
    Coord *potentialMoves = getPotentialDirections(level, ghost, &nbMoves);

    if (nbMoves == 0) {
        // Blinky est bloqué, il fait demi-tour
        ghost->dir.x = -ghost->dir.x;
        ghost->dir.y = -ghost->dir.y;
        ghost->pos.x += ghost->dir.x;
        ghost->pos.y += ghost->dir.y;
    } else {
        // Trouver la prochaine intersection que rencontrera Pac-Man
        Coord target = getPacmanNextIntersec(level, pacmanPos, pacmanDir);

        srand(time(NULL));
        if (rand() % BLINKY_RANDOMNESS == 0) {
            // Prend une direction aléatoire
            int randomIndex = rand() % nbMoves;
            ghost->dir = (Coord){
                    potentialMoves[randomIndex].x - ghost->pos.x,
                    potentialMoves[randomIndex].y - ghost->pos.y
            };
            ghost->pos = potentialMoves[randomIndex];
        } else {
            // Prend la direction qui rapproche le plus de l'intersection
            int minDistIndex = 0;
            int minDist = abs(target.x - potentialMoves[0].x) + abs(target.y - potentialMoves[0].y);

            for (int i = 1; i < nbMoves; i++) {
                int dist = abs(target.x - potentialMoves[i].x) + abs(target.y - potentialMoves[i].y);
                if (dist < minDist) {
                    minDist = dist;
                    minDistIndex = i;
                }
            }
            ghost->dir = (Coord){
                    potentialMoves[minDistIndex].x - ghost->pos.x,
                    potentialMoves[minDistIndex].y - ghost->pos.y
            };
            ghost->pos = potentialMoves[minDistIndex];
        }
    }
    free(potentialMoves);
}


// Fonction pour gérer les mouvements spécifiques aux fantômes
// Fonction pour gérer les mouvements spécifiques aux fantômes
void specificGhostMovement(char **level, Ghost *ghost, Coord pacmanPos, Coord pacmanDir) {
    switch (ghost->name) {
        case CLYDE:
            clydeMove(level, ghost);
            break;
        case PINKY:
            pinkyMove(level, ghost, pacmanPos);
            break;
        case INKY:
            inkyMove(level, ghost, pacmanPos);
            break;
        case BLINKY:
            blinkyMove(level, ghost, pacmanPos, pacmanDir);
            break;
        default:
            clydeMove(level, ghost);
            break;
    }
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
    textures.texturePinky = GetTexture("pinky.bmp", &params);
    textures.textureInky = GetTexture("inky.bmp", &params);
    textures.textureBlinky = GetTexture("blinky.bmp", &params); // Suppression de ghost_blue.bmp

    if (textures.texturePacman == NULL || textures.textureDot == NULL || textures.textureWall == NULL ||
        textures.textureClyde == NULL || textures.texturePinky == NULL || textures.textureInky == NULL ||
        textures.textureBlinky == NULL) {
        printf("Erreur : Impossible de charger les textures !\n");
        return 1;
    }




    // Chargement du niveau initial
    loadFirstLevel(&level);

    int running = 1;
    int pacmanX = 23; // Position initiale de Pac-Man (x)
    int pacmanY = 14; // Position initiale de Pac-Man (y)

    // Initialisation des fantômes
    // Initialisation des fantômes
    Ghost ghosts[4] = {
            {{13, 14}, {1, 0}, CLYDE, textures.textureClyde},
            {{15, 14}, {0, 1}, PINKY, textures.texturePinky},
            {{14, 14}, {0, -1}, INKY, textures.textureInky}, // Correction : texture correcte
            {{14, 13}, {0, -1}, BLINKY, textures.textureBlinky} // Ajout de Blinky
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

        // Déplacement des fantômes
        for (int i = 0; i < 4; i++) {
            specificGhostMovement(level, &ghosts[i], (Coord){pacmanX, pacmanY}, (Coord){dirX, dirY});
        }




        // Affichage du score actuel
        displayScore();

        // Vérification des conditions de victoire
        if (win(level, 31, 28)) {
            printf("Vous avez gagné !\n");
            running = 0;
        }

        // Vérification des conditions de défaite
        for (int i = 0; i < 4; i++) { // Vérifie avec tous les fantômes
            if (defeat(pacmanX, pacmanY, &ghosts[i])) {
                printf("Vous avez perdu !\n");
                running = 0;
                break;
            }
        }



        // Rendu des sprites et mise à jour
        drawLevel(level, 31, 28, &params, &textures);
        drawSpriteOnGrid(textures.texturePacman, pacmanX, pacmanY, 0, &params);
        for (int i = 0; i < 4; i++) {
            drawSpriteOnGrid(ghosts[i].texture, ghosts[i].pos.x, ghosts[i].pos.y, 0, &params);
        }


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
    SDL_DestroyTexture(textures.texturePinky);
    SDL_DestroyTexture(textures.textureDot);
    SDL_DestroyTexture(textures.textureWall);
    SDL_DestroyRenderer(params.renderer);
    SDL_DestroyWindow(params.window);
    SDL_Quit();

    printf("Fermeture du programme.\n");
    return 0;
}