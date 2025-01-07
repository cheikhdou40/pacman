// Framework
// Bibliothèque encapsulant l'utilisation de SDL 2

#include "framework.h"
#include <stdio.h>
#include <string.h>


// pour construire les chemins de fichiers
const char* pathSeparator =
#ifdef _WIN32
        "\\";
#else
"/";
#endif

#define ZOOM 1.5
#define GRID_CELL_SIZE 14





void initFullscreen(RendererParameters *params, Textures *textures)
{
    init(params, textures, -1, -1, 60);
}


void initWindowed(RendererParameters *params, Textures *textures, int levelHeight, int levelWidth)
{
    params->width = levelWidth * ZOOM * GRID_CELL_SIZE;
    params->height = levelHeight * ZOOM * GRID_CELL_SIZE;
    init(params, textures, params->width, params->height, FPS);
}




void init(RendererParameters *params, Textures *textures, int width, int height, int fps)
{
    // Initialisation de SDL2
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Framework -> SDL_Init failed: %s",   SDL_GetError());
        exit(1);
    }

    // Fenêtre en pleine écran
    if (width == -1 && height == -1) {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        width = DM.w;
        height = DM.h;
        SDL_SetWindowFullscreen(params->window, SDL_WINDOW_FULLSCREEN);
    }
    else if (width != params->width || height != params->height)
    {
        params->width = width;
        params->height = height;
    }

    // Création de la fenêtre
    params->window = SDL_CreateWindow("Pacman", 0, 0, width, height, SDL_WINDOW_OPENGL);
    if (params->window == NULL) {
        printf("Framework -> SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Création du moteur de rendu graphique
    params->renderer = SDL_CreateRenderer(params->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (params->renderer == NULL) {
        printf("Framework -> SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        SDL_DestroyWindow(params->window);
        exit(1);
    }

    // Textures de chacun des sprites du jeu
    textures->texturePacman = GetTexture("pacman.bmp", params);
    textures->textureWall = GetTexture("wall.bmp", params);
    textures->textureDot = GetTexture("dot.bmp", params);
    textures->textureSuperPacgum = GetTexture("super_pacgum.bmp", params);
    textures->textureBlinky = GetTexture("blinky.bmp", params);
    textures->textureBlinkyL = GetTexture("blinkyL.bmp", params);
    textures->textureBlinkyU = GetTexture("blinkyU.bmp", params);
    textures->textureBlinkyD = GetTexture("blinkyD.bmp", params);
    textures->texturePinky = GetTexture("pinky.bmp", params);
    textures->texturePinkyL = GetTexture("pinkyL.bmp", params);
    textures->texturePinkyU = GetTexture("pinkyU.bmp", params);
    textures->texturePinkyD = GetTexture("pinkyD.bmp", params);
    textures->textureInky = GetTexture("inky.bmp", params);
    textures->textureInkyL = GetTexture("inkyL.bmp", params);
    textures->textureInkyU = GetTexture("inkyU.bmp", params);
    textures->textureInkyD = GetTexture("inkyD.bmp", params);
    textures->textureClyde = GetTexture("clyde.bmp", params);
    textures->textureClydeL = GetTexture("clydeL.bmp", params);
    textures->textureClydeU = GetTexture("clydeU.bmp", params);
    textures->textureClydeD = GetTexture("clydeD.bmp", params);
    textures->textureBLue = GetTexture("blue.bmp", params);

    textures->cellSize = ZOOM * GRID_CELL_SIZE;

    //SDL_RenderSetLogicalSize(params.renderer, 800, 600);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2" );

    // Délai de rafraichissement
    params->ticks_for_next_frame = 1000 / fps;
    params->lastTimeScreenUpdate = 0;

}

int getInput() {
    SDL_Event e;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            SDL_Quit();
            exit(0);
        }
        if (e.type == SDL_KEYDOWN) {
            return e.key.keysym.sym;
        }
    }
    return 0;
}


void update(RendererParameters *params) {
    // On attend le temps nécessaire pour atteindre le taux de FPS
    int nbTicksWait = 0;
    int now = SDL_GetTicks();
    while (now-params->lastTimeScreenUpdate < params->ticks_for_next_frame) {
        SDL_Delay(1);
        nbTicksWait++;
        now = SDL_GetTicks();
    }

    if (nbTicksWait == 0)
        printf("Warning, FPS too high, lagged for %d ms\n", now-params->lastTimeScreenUpdate );

    // On met à jour l'écran
    SDL_RenderPresent(params->renderer);
    SDL_SetRenderDrawColor(params->renderer, 0, 0, 0, 0);
    SDL_RenderClear(params->renderer);

    // On réarme le temps d'attente pour le prochain rafraichissement
    params->lastTimeScreenUpdate = SDL_GetTicks();
}


void drawSpriteOnGrid(SDL_Texture * texture, int x, int y, float angle, RendererParameters *params)
{
    SDL_Rect dest = {x * GRID_CELL_SIZE * ZOOM,  y * GRID_CELL_SIZE * ZOOM, GRID_CELL_SIZE * ZOOM, GRID_CELL_SIZE * ZOOM};
    // SDL_Rect dest = {x * 16,  y * 16, 16, 16};
    if (SDL_RenderCopyEx(params->renderer, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE))
        printf("DrawSpriteOnGrid -> SDL_RenderCopyEx failed: %s", SDL_GetError());
}

void drawSprite(SDL_Texture * texture, int x, int y, int w, int h, float angle, RendererParameters *params)
{
    SDL_Rect dest = {x * GRID_CELL_SIZE * ZOOM,  y * GRID_CELL_SIZE * ZOOM, GRID_CELL_SIZE * ZOOM, GRID_CELL_SIZE * ZOOM};
    // SDL_Rect dest = {x * 16,  y * 16, 16, 16};
    if (SDL_RenderCopyEx(params->renderer, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE))
        printf("DrawSpriteAlpha -> SDL_RenderCopyEx failed: %s", SDL_GetError());
}

void drawSpriteAlpha(SDL_Texture * texture, int x, int y, int w, int h, float angle, float alpha, RendererParameters *params)
{
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (alpha < 1.0) {
        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND))
            printf("DrawSpriteAlpha -> SDL_SetTextureBlendMode failed: %s", SDL_GetError());
        if (SDL_SetTextureAlphaMod(texture, 255*alpha) == -1)
            printf("DrawSpriteAlpha -> SDL_SetTextureAlphaMod failed: Alpha mode not supported");
    }

    SDL_Rect dest = {x-(w/2),  y-(h/2), w, h};
    if (SDL_RenderCopyEx(params->renderer, texture, NULL, &dest, angle, NULL, flip))
        printf("DrawSpriteAlpha -> SDL_RenderCopyEx failed: %s", SDL_GetError());
}

void drawLevel(char** level, int row, int col, RendererParameters *params, Textures *textures)
{
    SDL_Texture * textureToDraw;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            switch (level[i][j])
            {
                case 'H':
                    drawSpriteOnGrid(textures->textureWall, j, i, 0, params);
                    break;
                case '.':
                    drawSpriteOnGrid(textures->textureDot, j, i, 0, params);
                    break;
                case 'G':
                    drawSpriteOnGrid(textures->textureSuperPacgum, j, i, 0, params);
                    break;
                case ' ':
                    break;
                case 'O':
                case 'P':
                case 'I':
                case 'C':
                case 'B':
                    // L'affichage de pacman et des fantomes n'est pas gere ici.
                    break;

                default:
                    printf("ERROR: Wrong element in the given level : %c", level[i][j]);
            }
        }
    }
}


// Création d'une texture de sprite
// --------
// * imgName : nom du fichier de l'image (format BMP avec alpha)
SDL_Texture * GetTexture(char* imageName, RendererParameters *params)
{
    char imagePath[80];

    strcpy(imagePath, "../resources/images");
    strcat(imagePath, pathSeparator);
    strcat(imagePath, imageName);
    SDL_Surface * sprite = SDL_LoadBMP(imagePath);

    if (!sprite) {
        printf("SDL_LoadBMP failed: %s", SDL_GetError());
        exit(1);
    }
    SDL_Texture * texture = SDL_CreateTextureFromSurface(params->renderer, sprite);
    if (!texture) {
        printf("DrawSpriteAlpha -> SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        exit(1);
    }

    return texture;
}