#include "../../shared/includes.h"
#include "../includes/game.h"
#include "../includes/cltNet.h"

int main(int argc, char** argv) {
    int running = 1;

    /*  INIT DE SDL  */
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "Erreur lors de l'initialisation de SDL!");
        return -1;
    }

    /*  CREATION DE LA FENETRE DE JEU  */
    SDL_Window* pWin = SDL_CreateWindow("IG2-BattleCruisers", 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        1280, 960, 
                                        SDL_WINDOW_SHOWN);
    
    if (pWin){
        SDL_Renderer* pRend = SDL_CreateRenderer(pWin, -1, 0);
        if (!pRend){
            fprintf(stderr, "Erreur lors de la création du renderer:\n%s", SDL_GetError());
            SDL_Quit();
            return -1;
        }

        SDL_Event e;

        // TODO: A remplacer ! Code pour démo multi !
        if (argc > 1){
            connectionInfo_t infos;
            infos.port = SERVER_PORT;
            infos.ipaddr = malloc(strlen(argv[1]));
            strcpy(infos.ipaddr, argv[1]);
            printf("Client: Connexion au serveur %s:%i\n", infos.ipaddr, infos.port);

            joinServer(&infos);
        }
        else {
            createServer();
        }

        gameInit();

        while(running) {
            /*  UPDATING GAME LOGIC  */
            gameUpdate();

            /*  RENDERING FRAMES  */
            SDL_SetRenderDrawColor(pRend, 100, 210, 230, 255);
            SDL_RenderClear(pRend);

            gameDraw(pRend);

            SDL_RenderPresent(pRend);

            /*  EVENT POLLING  */
            while(SDL_PollEvent(&e) > 0) {
                switch(e.type){
                    case SDL_QUIT:
                        running = 0;
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        gameMouseEvent(e.button);
                    break;
                    case SDL_MOUSEMOTION:
                        gameMouseMotion(e.motion);
                    break;
                    case SDL_KEYDOWN:
                        gameKeyPressed(e.key);
                    break;
                }
            }
        }

    }
    else {
        fprintf(stderr, "Erreur lors de la création de la fenêtre:\n%s", SDL_GetError());
    }

    if (isHost())
        closeServer();
    else
        disconnect();

    SDL_Quit();

    return 0;
}