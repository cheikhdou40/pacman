#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "../include/SDL2/SDL.h"

#define FPS 10

typedef struct RendererParameters RendererParameters;
struct RendererParameters
{
    SDL_Window *   window;
    SDL_Renderer * renderer;
    int ticks_for_next_frame;
    int lastTimeScreenUpdate;
    int width;
    int height;
};

typedef struct Textures Textures;
struct Textures
{
    SDL_Texture * texturePacman;
    SDL_Texture * textureWall;
    SDL_Texture * textureDot;
    SDL_Texture * textureSuperPacgum;
    SDL_Texture * textureBlinky;
    SDL_Texture * textureBlinkyL;
    SDL_Texture * textureBlinkyU;
    SDL_Texture * textureBlinkyD;
    SDL_Texture * texturePinky;
    SDL_Texture * texturePinkyL;
    SDL_Texture * texturePinkyU;
    SDL_Texture * texturePinkyD;
    SDL_Texture * textureInky;
    SDL_Texture * textureInkyL;
    SDL_Texture * textureInkyU;
    SDL_Texture * textureInkyD;
    SDL_Texture * textureClyde;
    SDL_Texture * textureClydeL;
    SDL_Texture * textureClydeU;
    SDL_Texture * textureClydeD;
    SDL_Texture * textureBLue;
    int cellSize;

};

// Initialisation du framweork
//
// Lance l'affichage plein écran dans la résolution courante.
// --------
// * width, height : résolution d'affichage
// * fps : taux de rafraichissement de l'écran (nombre d'images par seconde)
void init(RendererParameters *params, Textures *textures, int width, int height, int fps);

// Initialisation du framweork
//
// Lance l'affichage plein écran dans la résolution courante.
// --------
void initFullscreen(RendererParameters *params, Textures *textures);

// Initialisation du framweork
//
// Lance l'affichage à la taille du niveau.
// --------
// * levelWidth, levelHeight : taille du niveau
void initWindowed(RendererParameters *params, Textures *textures, int levelHeight, int levelWidth);

// Entrée du joeur eu clavier
// Renvoie un code clavier SDL2 (https://wiki.libsdl.org/SDL2/SDL_Keycode)
int getInput();

// Fonction à appeler à chaque rafraichissement d'image
// Cette fonction effectue une attente active afin de caler l'affichage sur le fps
// -------
// * trackFrameDrop : indique s'il faut afficher un FPS trop important
void update(RendererParameters *params);

// Affichage de sprite
// --------
// * texture : texture du sprite à recopier à l'écran
// * x, y : coordonnées du sprite
// * angle : orientation, en degrés. 0 correspond à la direction nord
void drawSpriteOnGrid(SDL_Texture * texture, int x, int y, float angle, RendererParameters *params);

// Affichage de sprite
// --------
// * texture : texture du sprite à recopier à l'écran
// * x, y : coordonnées du sommet gauche du sprite, en pixels
// * w, h : taille du sprite, en pixels
// * angle : orientation, en degrés. 0 correspond à la direction nord
void drawSprite(SDL_Texture * texture, int x, int y, int w, int h, float angle, RendererParameters *params);

// Affichage de sprite avec transparence
// --------
// * texture : texture du sprite à recopier à l'écran
// * x, y : coordonnées du centre du sprite, en pixels
// * w, h : taille du sprite, en pixels
// * angle : orientation, en degrés. 0 correspond à la direction nord
// * alpha : niveau de transparence, entre 0.0 (transparent) et  1.0 (opaque)
void drawSpriteAlpha(SDL_Texture * texture, int x, int y, int w, int h, float angle, float alpha, RendererParameters *params);

// Affichage du fond d'un niveau
// --------
// * level : le niveau sous forme d'un tableau de char
// * row, col : nombre de ligne et de colonne
void drawLevel(char** level, int row, int col, RendererParameters *params, Textures *textures);

// Création d'une texture de sprite
// --------
// * imgName : nom du fichier de l'image (format BMP avec alpha)
SDL_Texture * GetTexture(char* imageName, RendererParameters *params);

#endif //FRAMEWORK_H
