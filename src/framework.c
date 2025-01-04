// Framework
#include "framework.h"
#include "SDL2/SDL.h"
#include <stdlib.h>
#include <stdio.h>

void init(RendererParameters *params, Textures *textures, int width, int height, int fps) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    params->window = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!params->window) {
        printf("Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // Autres initialisations
}
