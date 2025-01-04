#define SDL_MAIN_HANDLED
#include "main.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    RendererParameters *params = malloc(sizeof(RendererParameters));
    Textures *textures = malloc(sizeof(Textures));
    init(params, textures, 500, 500, 60);

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            // Gestion du clavier
            if (event.type == SDL_KEYDOWN) {
                // Ajout des mouvements Pacman
            }
        }
        // Mise à jour des rendus
    }

    free(params);
    free(textures);
    return 0;
}
