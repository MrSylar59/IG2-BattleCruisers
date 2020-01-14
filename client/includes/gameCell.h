/**
 * \file gameCell.h
 * \brief Déclaration pour les cases de jeu
 * \author Thomas De Maen
 * \version 1.0
 * \date 14 Janvier 2020
 * 
 * Il s'agit du fichier de délaration des différentes structures de données et fonctions
 * utilisées pour le bon fonctionnement des cases de jeu.
*/
#ifndef _H_GAMECELL_
#define _H_GAMECELL_

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

/**
 * \enum cellState
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
 * \struct gameCell
 * \def t_gameCell
 * Structure représentant une case de jeu à l'aide de rectangles SDL
*/
typedef struct gameCell {
    SDL_Rect rect;          // représentation graphique de la case
    enum cellState state;   // état actuel de la case
    u_int8_t hasShip;       // si la case possède un morceau de navir dessus
} t_gameCell;

/**
 * \fn createGameCell (int x, int y, int w, int h)
 * Fonction permettant de créer une nouvelle case de jeu
 * \param x La coordonnée X de la case
 * \param y La coordonnée Y de la case
 * \param w La largeur de la case
 * \param h La hauteur de la case
 * \return La case qui a été créée
*/
t_gameCell createGameCell(int, int, int, int);

/**
 * \fn renderGameCell (SDL_Renderer* pRend, t_gameCell* pCell)
 * Fonction permettant de dessiner une case de jeu à l'écran, la couleur de la case dépend de son état
 * \param pRend Pointeur sur le renderer SDL utilisé pour dessiner sur la fenêtre de jeu
 * \param pCell Pointeur sur la case à dessiner
*/
void renderGameCell(SDL_Renderer*, t_gameCell*);

/**
 * \fn setGameCellState (enum cellState state, t_gameCell* pCell)
 * Permet de modifier l'état d'une case. Une case détruite (DESTROYED ou WATER) ne peut plus changer d'état
 * réduisant ainsi l'espace de jeu.
 * \param state Le nouvel état de la case
 * \param pCell Pointeur sur la case dont on souhaite modifier l'état
*/
void setGameCellState(enum cellState, t_gameCell*);

/**
 * \fn setGameCellShip (u_int8_t hasShip, t_gameCell* pCell)
 * Permet de définir qu'une case possède un morceau de navir. Le premier paramètre doit être égal à 1 ou 0
 * \param hasShip 0 si la case ne doit pas contenir de bâteau, n'importe quel autre nombre sinon
 * \param pCell Pointeur sur la case sur laquelle on souhaite ajouter ou supprimer un bâteau
*/
void setGameCellShip(u_int8_t, t_gameCell*);

#endif