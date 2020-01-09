#ifndef _H_GAMECELL_
#define _H_GAMECELL_

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

/**
 * L'ensemble des états que peuvent avoir une case de jeu
*/
enum cellState {
    NEUTRAL,
    HOVERED,
    WATER,
    DESTROYED,
    VALID,
    SHIP,
    INVALID
};

/**
 * Structure représentant une case de jeu à l'aide de rectangles SDL
*/
typedef struct gameCell {
    SDL_Rect rect;          // représentation graphique de la case
    enum cellState state;   // état actuel de la case
    u_int8_t hasShip;       // si la case possède un morceau de navir dessus
} t_gameCell;

/**
 * Fonction permettant de créer une nouvelle case de jeu
*/
t_gameCell createGameCell(int, int, int, int);

/**
 * Fonction permettant de dessiner une case de jeu à l'écran, la couleur de la case dépend de son état
*/
void renderGameCell(SDL_Renderer*, t_gameCell*);

/**
 * Permet de modifier l'état d'une case. Une case détruite (DESTROYED ou WATER) ne peut plus changer d'état
 * réduisant ainsi l'espace de jeu.
*/
void setGameCellState(enum cellState, t_gameCell*);

/**
 * Permet de définir qu'une case possède un morceau de navir. Le premier paramètre doit être égal à 1 ou 0
*/
void setGameCellShip(u_int8_t, t_gameCell*);

#endif