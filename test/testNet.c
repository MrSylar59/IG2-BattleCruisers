#include "../shared/includes.h"
#include "../client/includes/cltNet.h"

int main(int argc, char** argv) {
    connectionInfo_t infos;
    packet_t p;
    infos.port = SERVER_PORT;
    
    if (argc > 1){
        infos.ipaddr = argv[1];
        joinServer(&infos);

        printf("Attente d'un paquet\n");

        sleep(3);

        p = netRead();

        printf("Réception d'un paquet flag: %i, size: %i\n", p.flag, p.size);

        for (int i = 0; i < p.size; i++){
            printf("[%i] => %i\n", i, p.data[i]);
        }

        sleep(3);

        disconnect();
        printf("Déconnexion\n");
    }
    else {
        createServer();

        while(!hasClient());

        printf("Envoie de données\n");
        p.flag = 3;
        p.size = 4;

        p.data[0] = 1;
        p.data[1] = 2;
        p.data[2] = 3;
        p.data[3] = 4;

        for (int i = 0; i < p.size; i++){
            printf("[%i] => %i\n", i, p.data[i]);
        }

        netSend(p);
        printf("Paquet envoyé flag: %i\n", p.flag);

        sleep(3);

        closeServer();
        printf("Serveur fermé\n");
    }

    return 0;
}