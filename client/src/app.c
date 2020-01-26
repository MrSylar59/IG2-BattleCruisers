#include "../../shared/includes.h"
#include "../includes/game.h"
#include "../includes/cltNet.h"
#include "../includes/term.h"

void printTitle();
void printMenu();
void printError(char*);
int connexionServeurMM();
int dialogueServeurMM(int);

int main(int argc, char** argv) {
    int running;
    int sd;
    int quit;

    do {

        running = 1;
        quit = 0;

        printTitle();

        if ((sd=connexionServeurMM()) == -1){
            printError("Impossible de se connecter au serveur de matchmaking.\n\tVerifiez votre connexion.\n");
            return -1;
        }

        quit = dialogueServeurMM(sd);

        if (!quit){
            /*  INIT DE SDL  */
            if (SDL_Init(SDL_INIT_VIDEO) != 0){
                fprintf(stderr, "Erreur lors de l'initialisation de SDL!");
                return -1;
            }

            /*  CREATION DE LA FENETRE DE JEU  */
            SDL_Window* pWin;
            if (isHost()){
                pWin = SDL_CreateWindow("IG2-BattleCruisers (Server)", 
                                                SDL_WINDOWPOS_CENTERED, 
                                                SDL_WINDOWPOS_CENTERED, 
                                                1280, 960, 
                                                SDL_WINDOW_SHOWN);
            }
            else{
                pWin = SDL_CreateWindow("IG2-BattleCruisers", 
                                                SDL_WINDOWPOS_CENTERED, 
                                                SDL_WINDOWPOS_CENTERED, 
                                                1280, 960, 
                                                SDL_WINDOW_SHOWN);
            }
            
            
            if (pWin){
                SDL_Renderer* pRend = SDL_CreateRenderer(pWin, -1, 0);
                if (!pRend){
                    fprintf(stderr, "Erreur lors de la création du renderer:\n%s", SDL_GetError());
                    SDL_Quit();
                    return -1;
                }

                SDL_Event e;

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
                                gameQuitEvent();
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

                SDL_Quit();

            }
            else {
                fprintf(stderr, "Erreur lors de la création de la fenêtre:\n%s", SDL_GetError());
            }
        }

        if (isConnected()){
            if (isHost())
                closeServer();
            else
                disconnect();
        }
        
        if (!quit){
            clearScreen();
            printTitle();

            if (hasWon()){
                setTerm(GREEN);
                printf("\tVous avez gagné!\n");
                resetTerm();
            }
            else if(!hasWon() && !hasLost()){
                printError("Partie anulée : l'adversaire s'est déconnecté!");
            }
            else{
                printError("Vous avez perdu!");
            }

             printf("\n\n\tAppuyez sur ENTRER pour revenir au menu principal...\n");
            getchar();
        }

    }while(!quit);

    return 0;
}

void printTitle(){
    clearScreen();
    setTerm(YELLOW);
    printf("  _____ _____ ___        ____        _   _   _       _____            _                   \n");
    printf(" |_   _/ ____|__ \\      |  _ \\      | | | | | |     / ____|          (_)                  \n");
    printf("   | || |  __   ) |_____| |_) | __ _| |_| |_| | ___| |     _ __ _   _ _ ___  ___ _ __ ___ \n");
    printf("   | || | |_ | / /______|  _ < / _` | __| __| |/ _ \\ |    | '__| | | | / __|/ _ \\ '__/ __|\n");
    printf("  _| || |__| |/ /_      | |_) | (_| | |_| |_| |  __/ |____| |  | |_| | \\__ \\  __/ |  \\__ \\\n");
    printf(" |_____\\_____|____|     |____/ \\__,_|\\__|\\__|_|\\___|\\_____|_|   \\__,_|_|___/\\___|_|  |___/\n");
    printf("                                                                                          v1.0\n");
    resetTerm();
}

void printMenu() {
    setTerm(HIGH);
    printf("\t1 - Héberger une partie\n");
    printf("\t2 - Rejoindre une partie\n\n");
    printf("\t0 - Quitter\n");
    resetTerm();
}

void printError(char* err){
    setTerm(RED);
    printf("\t%s\n", err);
    resetTerm();
}

int connexionServeurMM(){
    int sock;
    struct sockaddr_in svc;

    CHECK(sock=socket(PF_INET, SOCK_STREAM, 0), "Can't create socket");
    svc.sin_family = PF_INET;
	svc.sin_port = htons (PORT_SRV);
	svc.sin_addr.s_addr = inet_addr(ADDR_SRV);
	memset(&svc.sin_zero, 0, 8);

    if (connect(sock, (struct sockaddr *)&svc, sizeof svc) == -1)
        return -1;
    return sock;
}

int dialogueServeurMM(int sd){
    int gameStart = 0;
    int err = 0, err2 = 0;
    char rep[MAX_BUFF];
    char req[MAX_BUFF];
    char commande;

    do {
        clearScreen();
        printTitle();
        printf("\n\n\n");
        printMenu();
        if (err) {printError("Reçu charactère invalide!\n"); err=0;}
        if (err2) {printError("Connexion refusée!\n"); err2=0;}
        printf("\n\n\tCommande: ");
        commande = getchar();
        getchar(); // pour retirer le \n

        packet_t p;
        int mdp = 0;

        switch (commande){
            case '0':
                clearScreen();
                printf("Aurevoir\n");
                strcpy(req, "0 : Déconnexion");
                CHECK(write(sd, req, strlen(req)+1), "Can't send");
                shutdown(sd, SHUT_WR);
                return 1;
                break;

            case '1':
                sprintf(req, "1 : %i", SERVER_PORT);
                CHECK(write(sd, req, strlen(req)+1), "Can't send");
			    CHECK(read(sd, rep, sizeof(rep)), "Can't send");

                int id = -1;
                sscanf(rep, "%i :", &id);

                if (id == -1){
                    printError("Impossible de créer une room avec le serveur de matchmaking.\n\tFermeture automatique dans 3 secondes\n");
                    sleep(3);
                    clearScreen();
                    printf("Aurevoir\n");
                    strcpy(req, "0 : Déconnexion");
                    CHECK(write(sd, req, strlen(req)+1), "Can't send");
                    shutdown(sd, SHUT_WR);
                    exit(0);
                }

                createServer();

                setTerm(HIGH);
                printf("\tRoom %i : ", id);
                resetTerm();
                printf("Attente de l'arrivée d'un autre joueur");
                setTerm(BLINK);
                printf("...\n");
                resetTerm();

                //do {
                    while(!hasClient());

                    do {
                        p = netRead();
                    }while(p.flag == 255);

                    printf("p: %i : %i - %i - %i - %i : %i\n", p.flag, p.data[0], p.data[1], p.data[2], p.data[3], p.size);

                    if (p.flag == 1 && p.size == 4){
                        memcpy(&mdp, p.data, sizeof(int));
                    }

                    printf("%i\n", mdp);
                    sprintf(req, "5 : %i : %i", id, mdp);
                    printf("%s\n", req);

                    CHECK(write(sd, req, strlen(req)+1), "Can't send");
                    CHECK(read(sd, rep, sizeof(rep)), "Can't send");
                    
                    if (rep[0] = '1')
                        p.flag = 1;
                    else
                        p.flag = 0;

                    printf("%i\n", p.flag);
                    netSend(p);
                //} while (req[0] != '1');
                

                sprintf(req, "4 : %i", SERVER_PORT);
                CHECK(write(sd, req, strlen(req)+1), "Can't send");
                CHECK(read(sd, rep, sizeof(rep)), "Can't send");
                printf("test\n");

                gameStart = 1;

                break;

            case '2':
                strcpy(req, "2");
                CHECK(write(sd, req, strlen(req)+1), "Can't send");
			    CHECK(read(sd, rep, sizeof(rep)), "Can't send");

                clearScreen();
                printTitle();
                printf("\n\n");

                setTerm(HIGH);
                printf("Liste des rooms disponnibles:\n");
                resetTerm();
                printf("%s\n",rep);
                printf("0 : retour au menu principal\n\n");

                do {
                    printf("\tcommande: ");
                    int m_id;
                    scanf("%i", &m_id);

                    if (m_id == 0)
                        break;

                    sprintf(req, "3 : %i", m_id);
                    CHECK(write(sd, req, strlen(req)+1), "Can't send");
                    CHECK(read(sd, rep, sizeof(rep)), "Can't send");

                    char res;
                    connectionInfo_t infos;
                    infos.ipaddr = malloc(INET_ADDRSTRLEN);
                    sscanf(rep, "%c : %i : %s : %hi", &res, &mdp, infos.ipaddr, &infos.port);
                    printf("Client: Connexion au serveur %s:%i\n", infos.ipaddr, infos.port);

                    joinServer(&infos);
                    
                    if (res == '0')
                        printError("Numéro de room non valide.");

                }while(rep[0] != '1');

                p.flag = 1;
                p.data[0] = mdp & 0xff;
                p.data[1] = (mdp>>8) & 0xff;
                p.data[2] = (mdp>>16) & 0xff;
                p.data[3] = (mdp>>24) & 0xff;
                p.size = 4;

                netSend(p);

                sleep(1); // Pour s'assurer qu'on écrase pas notre paquet à envoyer
                // Car la mutex dans la lib net.c semble ne pas fonctionner

                do {
                    p = netRead();
                }while(p.flag == 255);

                printf("%i\n", p.flag);

                if (p.flag == 1){
                    printf("\tPartie autorisée\n");
                    gameStart = 1;
                }
                else {
                    disconnect();
                    err2 = 1;
                }

                break;
            
            default:
                err = 1;
                break;
        }

    }while(!gameStart);

    clearScreen();
    printTitle();
    setTerm(HIGH);
    printf("\n\n\tPartie en cours.\n");
    resetTerm();

    return 0;
}