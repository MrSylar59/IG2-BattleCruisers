/**
 * \file game.h
 * \brief Déclaration pour le jeu
 * \author Thomas De Maen
 * \version 1.0
 * \date 14 Janvier 2020
 * 
 * Il s'agit du fichier de délaration des différentes structures de données et fonctions
 * utilisées pour le bon fonctionnement du jeu.
*/
#ifndef _H_GAME_
#define _H_GAME_

#include <SDL2/SDL.h>
#include "../includes/cltNet.h"

/**
 * \enum gameState
 * Enum représentant les différents états possibles du jeu
*/
enum gameState {
    MAIN_MENU,
    PLACING,
    PLAYING
};

/**
 * \struct mousePos
 * Structure représentant la position de la souris à l'intérieur de la grille de jeu
*/
typedef struct mousePos {
    u_int8_t x;
    u_int8_t y;
} t_mousePos;

/**
 * \fn gameInit (void)
 * Fonction appellée après l'initialisation de la SDL et la création de la fenêtre
 * Le premier entier permet de savoir si le client est un hôte ou non, dans le cas où ce
 * n'est pas l'hôte, les infos de connexion sont passés en deuxième argument
*/
void gameInit();

/**
 * \fn gameUpdate (void)
 * Fonction appellée en continue afin de mettre à jour la logique du jeu
 * TODO: Réguler la fréquence de rafraichissement (FPS cap)
*/
void gameUpdate();

/**
 * \fn gameDraw (SDL_Renderer* pRend)
 * Fonction appellée en continue afin de dessiner le jeu, elle est appellée après chaque update
 * TODO: Réguler la fréquence de rafraichissement (FPS cap)
 * \param pRend Pointeur sur le renderer utilisé par SDL pour dessiner sur la fenêtre de jeu
*/
void gameDraw(SDL_Renderer*);

//////////////////////////////////////////
// Events
/////////////////////////////////////////
/**
 * \fn gameMouseEvent (SDL_MouseButtonEvent e)
 * Fonction appellée à chaque fois qu'un événement du type SDL_MOUSEBUTTONDOWN est récupéré par SDL
 * \param e Evénement capturé par SDL
*/
void gameMouseEvent(SDL_MouseButtonEvent);

/**
 * \fn gameMouseMotion (SDL_MouseMotionEvent e)
 * Fonction appellée à chaque fois qu'un événement du type SDL_MOUSEMOTION est récupéré par SDL
 * \param e Evénement capturé par SDL
*/
void gameMouseMotion(SDL_MouseMotionEvent);

/**
 * \fn gameKeyPressed (SDL_KeyboardEvent e)
 * Fonction appellée à chaque fois qu'un événement du type SDL_KEYDOWN est récupéré par SDL
 * \param e Evénement capturé par SDL
*/
void gameKeyPressed(SDL_KeyboardEvent);

/**
 * \fn min (int a, int b)
 * Fonction utilitaire qui renvoie le minimum entre deux entiers
 * \param a Premier nombre à comparer au second
 * \param b Second nombre à comparer au premier
 * \return Le nombre le plus petit entre a et b
*/
int min(int, int);

#endif