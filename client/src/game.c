#include "../../shared/includes.h"
#include "../includes/game.h"
#include "../includes/gameCell.h"

#define CELL_SIZE 80
#define X_PADDING 240
#define Y_PADDING 80
#define SPACING 2

t_gameCell cells[10][10];
enum gameState gs = MAIN_MENU;
t_mousePos mpos = {0, 0};
u_int8_t rotate = 0;
int shipSizes[5] = {5, 4, 3, 3, 2};
int ship = 0;
int valid = 1;

void gameInit() {
    for (int x = 0; x < 10; x++)
        for (int y = 0; y < 10; y++){
            cells[x][y] = createGameCell(X_PADDING + x*CELL_SIZE,
                                        Y_PADDING + y*CELL_SIZE,
                                        CELL_SIZE - SPACING,
                                        CELL_SIZE - SPACING);
        }
}

void gameUpdate() {
    switch(gs){
        case MAIN_MENU:

        break;

        case PLACING:
            for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++){
                if (cells[x][y].hasShip)
                    setGameCellState(SHIP, &cells[x][y]);
                else
                    setGameCellState(NEUTRAL, &cells[x][y]);
            }

            if (ship < 5) {
                if (rotate && mpos.y+shipSizes[ship] > 10 || !rotate && mpos.x+shipSizes[ship] > 10) 
                    valid = 0;
                else valid = 1;

                if (!rotate){
                    for (int i = mpos.x; i < min(mpos.x+shipSizes[ship], 10); i++)
                        if (cells[i][mpos.y].hasShip) valid = 0;
                }
                else{
                    for (int i = mpos.y; i < min(mpos.y+shipSizes[ship], 10); i++)
                        if (cells[mpos.x][i].hasShip) valid = 0;
                }
                    
            }

            if (!rotate)
                for (int i = mpos.x; i < min(mpos.x+shipSizes[ship], 10); i++)
                    if(valid)
                        setGameCellState(VALID, &cells[i][mpos.y]);
                    else
                        setGameCellState(INVALID, &cells[i][mpos.y]);
            else 
                for (int i = mpos.y; i < min(mpos.y+shipSizes[ship], 10); i++)
                    if(valid)
                        setGameCellState(VALID, &cells[mpos.x][i]);
                    else
                        setGameCellState(INVALID, &cells[mpos.x][i]);
        break;

        case PLAYING:
            for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++){
                setGameCellState(NEUTRAL, &cells[x][y]);
            }

            setGameCellState(HOVERED, &cells[mpos.x][mpos.y]);
        break;
    }
}

void gameDraw(SDL_Renderer* pRend) {
    switch(gs){
        case MAIN_MENU:

        break;

        case PLACING:
            for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++){
                renderGameCell(pRend, &cells[x][y]);
            }
        break;

        case PLAYING:
            for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++){
                renderGameCell(pRend, &cells[x][y]);
            }
        break;
    }
}

// Gestion des events
void gameMouseEvent(SDL_MouseButtonEvent e){
    switch(gs){
        case MAIN_MENU:

        break;

        case PLACING:
            if (valid) {
                if (!rotate)
                    for (int i = mpos.x; i < min(mpos.x+shipSizes[ship], 10); i++)
                        setGameCellShip(1, &cells[i][mpos.y]);
                else 
                    for (int i = mpos.y; i < min(mpos.y+shipSizes[ship], 10); i++)
                        setGameCellShip(1, &cells[mpos.x][i]);
                ship++;
            }

            if (ship >= 5)
                gs = PLAYING;
        break;

        case PLAYING:
            if (cells[mpos.x][mpos.y].hasShip)
                setGameCellState(DESTROYED, &cells[mpos.x][mpos.y]);
            else
                setGameCellState(WATER, &cells[mpos.x][mpos.y]);
        break;
    }
}

void gameMouseMotion(SDL_MouseMotionEvent e){
    if (gs != MAIN_MENU) {
        int x = (e.x-240)/80;
        int y = (e.y-80)/80;

        if (x >= 0 && x < 10){
            mpos.x = x;
        }

        if (y >= 0 && y < 10){
            mpos.y = y;
        }
    }
}

void gameKeyPressed(SDL_KeyboardEvent e){
    // Equivalences touches tapés
    // 13 = Entrer
    // 114 = R
    if (e.repeat) return;

    switch(gs){
        case MAIN_MENU:
            if (e.keysym.sym == 13)
                gs = PLACING;
        break;

        case PLACING:
            if (e.keysym.sym == 13)
                gs = PLAYING;
            if (e.keysym.sym == 114)
                rotate = !rotate;
        break;

        case PLAYING:
            if (e.keysym.sym == 13)
                gs = MAIN_MENU;
        break;
    }
}

int min(int a, int b){
    return (a<b) ? a : b;
}