#include "../includes/gameCell.h"

t_gameCell createGameCell(int x, int y, int w, int h){
    t_gameCell cell;

    cell.hasShip = 0;
    cell.state = NEUTRAL;

    cell.rect.x = x;
    cell.rect.y = y;
    cell.rect.w = w;
    cell.rect.h = h;

    return cell;
}

void renderGameCell(SDL_Renderer* pRend, t_gameCell* cell){
    switch(cell->state){
        case NEUTRAL:
            SDL_SetRenderDrawColor(pRend, 0, 0, 255, 255);
        break;
        case HOVERED:
            SDL_SetRenderDrawColor(pRend, 0, 0, 128, 255);
        break;
        case WATER:
            SDL_SetRenderDrawColor(pRend, 0, 0, 0, 255);
        break;
        case DESTROYED:
            SDL_SetRenderDrawColor(pRend, 255, 0, 0, 255);
        break;
        case VALID:
            SDL_SetRenderDrawColor(pRend, 0, 128, 0, 255);
        break;
        case SHIP:
            SDL_SetRenderDrawColor(pRend, 0, 255, 0, 255);
        break;
        case INVALID:
            SDL_SetRenderDrawColor(pRend, 128, 0, 0, 255);
        break;
    }
    
    SDL_RenderFillRect(pRend, &(cell->rect));
}

void setGameCellState(enum cellState s, t_gameCell* cell){
    if (cell->state != DESTROYED && cell->state != WATER)
        cell->state = s;
}

void setGameCellShip(u_int8_t hasShip, t_gameCell* cell){
    if (hasShip <= 1) 
        cell->hasShip = hasShip;
}