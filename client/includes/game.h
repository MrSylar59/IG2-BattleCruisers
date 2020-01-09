#ifndef _H_GAME_
#define _H_GAME_

#include <SDL2/SDL.h>

/**
 * Enum représentant les différents états possibles de l'application
*/
enum gameState {
    MAIN_MENU,
    PLACING,
    PLAYING
};

/**
 * Structure représentant la position de la souris à l'intérieur de la grille de jeu
*/
typedef struct mousePos {
    u_int8_t x;
    u_int8_t y;
} t_mousePos;

/**
 * Fonction appellée après l'initialisation de la SDL et la création de la fenêtre
*/
void gameInit();

/**
 * Fonction appellée en continue afin de mettre à jour la logique du jeu
 * TODO: Réguler la fréquence de rafraichissement (FPS cap)
*/
void gameUpdate();

/**
 * Fonction appellée en continue afin de dessiner le jeu, elle est appellée après chaque update
 * TODO: Réguler la fréquence de rafraichissement (FPS cap)
*/
void gameDraw(SDL_Renderer*);

//////////////////////////////////////////
// Events
/////////////////////////////////////////
/**
 * Fonction appellée à chaque fois qu'un événement du type SDL_MOUSEBUTTONDOWN est récupéré par SDL
*/
void gameMouseEvent(SDL_MouseButtonEvent);

/**
 * Fonction appellée à chaque fois qu'un événement du type SDL_MOUSEMOTION est récupéré par SDL
*/
void gameMouseMotion(SDL_MouseMotionEvent);

/**
 * Fonction appellée à chaque fois qu'un événement du type SDL_KEYDOWN est récupéré par SDL
*/
void gameKeyPressed(SDL_KeyboardEvent);

/**
 * Fonction utilitaire qui renvoie le minimum entre deux entiers
*/
int min(int, int);

#endif