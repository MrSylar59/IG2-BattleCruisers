#include "../../shared/includes.h"
#include "../includes/game.h"
#include "../includes/gameCell.h"

#define CELL_SIZE 80
#define X_PADDING 240
#define Y_PADDING 80
#define SPACING 2
#define WAIT_TIME 250

t_gameCell cells[10][10];
enum gameState gs = PLACING;
t_mousePos mpos = {0, 0};
u_int8_t rotate = 0;
int shipSizes[5] = {5, 4, 3, 3, 2};
int ship;           // Nombre de bâteaux que j'ai placé si >= 5, je suis prêt !
int valid;          // L'emplacement est-il valide pour un bâteau ?
int myTurn;         // Est-ce mon tour de jeu ?
int ready;          // L'autre joueur est-il prêt ?
int waitAnswer;     // Est-ce que j'attends une réponse de l'autre joueur ?
int score;          // Combien de cases ai-je touché ? (si = 15 alors je gagne)
int oScore;         // Combien de cases l'autre à touché (si = 15 alors je perd)

void gameInit() {
    for (int x = 0; x < 10; x++)
        for (int y = 0; y < 10; y++){
            cells[x][y] = createGameCell(X_PADDING + x*CELL_SIZE,
                                        Y_PADDING + y*CELL_SIZE,
                                        CELL_SIZE - SPACING,
                                        CELL_SIZE - SPACING);
        }

    // TODO : à supprimer, pour démo
    if (isHost()){
        myTurn = 1;
    }
    else {
        myTurn = 0;
    }

    ship = 0;
    valid = 1;
    ready = 0;
    waitAnswer = 0;
    score = 0;
    oScore = 0;
}

float nextAsk = WAIT_TIME;
void gameUpdate() {
    switch(gs){
        case MAIN_MENU:

        break;

        case PLACING:
            if (ship < 5) {
                for (int x = 0; x < 10; x++)
                for (int y = 0; y < 10; y++){
                    if (cells[x][y].hasShip)
                        setGameCellState(SHIP, &cells[x][y]);
                    else
                        setGameCellState(NEUTRAL, &cells[x][y]);
                }

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
            }
            else
            {
                for (int x = 0; x < 10; x++)
                for (int y = 0; y < 10; y++){
                    setGameCellState(NEUTRAL, &cells[x][y]);
                }
            }
            

            if (!ready && nextAsk <= 0){
                nextAsk = WAIT_TIME;

                packet_t p;
                p = netRead();
                if (p.flag == 1 && p.size == 1 && p.data[0] == 1){
                    ready = 1;
                }

                if (p.flag == 4 || score >= 17 || oScore >= 17){
                    SDL_Event e;
                    e.type = SDL_QUIT;
                    SDL_PushEvent(&e);
                }
            }

            if (ship >= 5 && ready)
                gs = PLAYING;
        break;

        case PLAYING:
            for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++){
                setGameCellState(NEUTRAL, &cells[x][y]);
            }

            if (myTurn) {
                setGameCellState(HOVERED, &cells[mpos.x][mpos.y]);

                if (waitAnswer && nextAsk <= 0) {
                    nextAsk = WAIT_TIME;

                    packet_t p;
                    p = netRead();

                    if (p.flag == 3 && p.size == 3){
                        if (p.data[2]){
                            setGameCellState(DESTROYED, &cells[p.data[0]][p.data[1]]);
                            score++;
                        }
                        else {
                            setGameCellState(WATER, &cells[p.data[0]][p.data[1]]);
                        }

                        waitAnswer = 0;
                        myTurn = 0;
                    }

                    if (p.flag == 4 || score >= 17 || oScore >= 17){
                        SDL_Event e;
                        e.type = SDL_QUIT;
                        SDL_PushEvent(&e);
                    }
                }
            }
            else {
                waitAnswer = 1;

                if (nextAsk <= 0){
                    nextAsk = WAIT_TIME;

                    packet_t p;
                    p = netRead();

                    if (p.flag == 2 && p.size == 2){
                        packet_t ans;
                        ans.flag = 3;
                        ans.size = 3;
                        ans.data[0] = p.data[0];
                        ans.data[1] = p.data[1];

                        if (cells[p.data[0]][p.data[1]].hasShip){
                            ans.data[2] = 1;
                            oScore++;
                        }
                        else
                            ans.data[2] = 0;

                        netSend(ans);

                        waitAnswer = 0;
                        myTurn = 1;
                    }

                    if (p.flag == 4 || score >= 17 || oScore >= 17){
                        SDL_Event e;
                        e.type = SDL_QUIT;
                        SDL_PushEvent(&e);
                    }
                }
            }
            
        break;
    }

    if (waitAnswer || !ready)
        nextAsk--;
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
    packet_t p;

    switch(gs){
        case MAIN_MENU:

        break;

        case PLACING:
            if (valid && ship < 5) {
                if (!rotate)
                    for (int i = mpos.x; i < min(mpos.x+shipSizes[ship], 10); i++)
                        setGameCellShip(1, &cells[i][mpos.y]);
                else 
                    for (int i = mpos.y; i < min(mpos.y+shipSizes[ship], 10); i++)
                        setGameCellShip(1, &cells[mpos.x][i]);
                ship++;
            }

            if (ship == 5){
                p.flag = 1;
                p.size = 1;
                p.data[0] = 1;

                netSend(p);
                ship++; // to send only once
            }
                
        break;

        case PLAYING:

            if (myTurn) {
                p.flag = 2;
                p.size = 2;
                p.data[0] = mpos.x;
                p.data[1] = mpos.y;

                netSend(p);
                waitAnswer = 1;
            }

            //if (cells[mpos.x][mpos.y].hasShip)
            //    setGameCellState(DESTROYED, &cells[mpos.x][mpos.y]);
            //else
            //    setGameCellState(WATER, &cells[mpos.x][mpos.y]);
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
           // if (e.keysym.sym == 13)
             //   gs = PLACING;
        break;

        case PLACING:
            //if (e.keysym.sym == 13)
               // gs = PLAYING;
            if (e.keysym.sym == 114)
                rotate = !rotate;
        break;

        case PLAYING:
           // if (e.keysym.sym == 13)
             //   gs = MAIN_MENU;
        break;
    }
}

void gameQuitEvent(){
    printf("On quitte le jeu\n");

    packet_t p;
    p.flag = 4;
    p.size = 1;
    p.data[0] = 1;
    netSend(p);
}

int min(int a, int b){
    return (a<b) ? a : b;
}

int hasWon() {
    return score >= 17;
}

int hasLost() {
    return oScore >= 17;
}