#include "../includes/cltNet.h"

// TODO: retravailler pour architecture multiclient (spectateurs)

u_int8_t op = 0;
u_int8_t host = 0;
u_int8_t connected = 0;
u_int8_t clientConnected = 0;

pthread_mutex_t mutex, mutexRD;
pthread_t netThread;

int se, sd;
struct sockaddr_in srv, clt;
socklen_t cltLen;
packet_t packet_buffer;
struct pollfd fd;
int res;

////////////////////////
// FONCTIONS INTERNES
////////////////////////
void th_dialogue(int sd){
    do {
        #ifdef _DEBUG
        printf("Attente d'une opération\n");
        #endif

        // Attente d'une opération par l'application
        pthread_mutex_lock(&mutex);

        switch(op) {
            case 0: // déconnexion, fin du dialogue
                #ifdef _DEBUG
                printf("Déconnexion demandée.\n");
                #endif
            break;

            case 1: // écriture sur le réseau demandé
                #ifdef _DEBUG
                printf("Ecriture demandée\n");
                #endif
                CHECK(write(sd, &packet_buffer, sizeof packet_buffer), "Error: Unable to write to network!");
            break;

            case 2: // lecture sur le réseau demandé
                #ifdef _DEBUG
                printf("Lecture demandée\n");
                #endif
                
                fd.fd = sd;
                fd.events = POLLIN;

                CHECK(res=poll(&fd, 1, READ_TO), "Error: Unable de poll events on socket");

                if (res==0) // Il n'y avait rien à lire sur le réseau dans le temps imparti
                    packet_buffer.flag = -1;
                else 
                    CHECK(read(sd, &packet_buffer, sizeof packet_buffer), "Error: Unable to read from network!");

                // On prévient le thread principal
                pthread_mutex_unlock(&mutexRD);
            break;

            default: fprintf(stderr, "Error: Recieved OP %i -> unrecognized therefore ignored.\n", op); break;
        }

    }while(op != 0);
}

void* th_createServer(void* unused){
    // Création de la socket d'écoute
    CHECK(se=socket(PF_INET, SOCK_STREAM, 0), "Error: Failed to create socket");

    // Préparation de l'adressage
    srv.sin_family = PF_INET;
    srv.sin_port = htons(SERVER_PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&srv.sin_zero, 0, 8);

    // Association de l'adressage avec la socket
    CHECK(bind(se, (struct sockaddr*)&srv, (socklen_t)sizeof srv), "Error: Failed to bind socket");

    // Mise en écoute de la socket
    CHECK(listen(se, 1), "Error: Failed to listen");

    // Boucle de service
    while (connected) {
        cltLen = sizeof clt;

        // Attente d'un client
        CHECK(sd=accept(se, (struct sockaddr*)&clt, &cltLen), "Error: Failed to connect to client");

        clientConnected = 1;

        // Début du dialogue avec le client
        th_dialogue(sd);

        shutdown(sd, SHUT_RDWR);

        clientConnected = 0;
    }

    close(se);

    pthread_exit(EXIT_SUCCESS);
}

void* th_joinServer(void* pInfos){
    connectionInfo_t* infos = (connectionInfo_t*) pInfos;

    // Création de la socket de dialogue
    CHECK(sd=socket(PF_INET, SOCK_STREAM, 0), "Error: Failed to create dialogue socket");

    // Préparation de l'adressage à contacter
    srv.sin_family = PF_INET;
    srv.sin_port = htons(infos->port);
    srv.sin_addr.s_addr = inet_addr(infos->ipaddr);
    memset(&srv.sin_zero, 0, 8);

    // Demande de connection avec le serveur
    CHECK(connect(sd, (struct sockaddr*)&srv, (socklen_t)sizeof srv), "Error: Failed to connect to server");

    // Début du dialogue avec le serveur
    th_dialogue(sd);

    shutdown(sd, SHUT_RDWR);

    pthread_exit(EXIT_SUCCESS);
}

////////////////////////
// FONCTIONS PUBLIQUES
////////////////////////
void createServer(){
    host = 1;
    connected = 1;

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexRD, NULL);
    pthread_mutex_lock(&mutex);
    pthread_mutex_lock(&mutexRD);
    pthread_create(&netThread, NULL, th_createServer, NULL);
}

void joinServer(connectionInfo_t* infos) {
    connected = 1;


    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexRD, NULL);
    pthread_mutex_lock(&mutex);
    pthread_mutex_lock(&mutexRD);
    pthread_create(&netThread, NULL, th_joinServer, (void*)infos);
}

void closeServer() {
    void* ret = NULL;

    host = 0;
    connected = 0;
    op = 0;
    pthread_mutex_unlock(&mutex);
    pthread_join(netThread, &ret);
}

void disconnect(){
    void* ret = NULL;

    connected = 0;
    op = 0;
    pthread_mutex_unlock(&mutex);
    pthread_join(netThread, &ret);
}

void netSend(packet_t packet){
    // On place le paquet dans le buffer
    memcpy(&packet_buffer, &packet, sizeof packet);
    // On demande une écriture sur le réseau

#ifdef _DEBUG
    printf("Buffer:\n");
    printf("\tflag: %i\n\tsize: %i\n\tdata: %i %i\n", packet_buffer.flag, packet_buffer.size, packet_buffer.data[0], packet_buffer.data[1]);
#endif

    op = 1;
    pthread_mutex_unlock(&mutex);
}

packet_t netRead(){
    packet_t packet;

    // On demande une lecture sur le réseau
    op = 2;
    //pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    // On attend que la lecture se soit terminée
    pthread_mutex_lock(&mutexRD);

#ifdef _DEBUG
    printf("Buffer:\n");
    printf("\tflag: %i\n\tsize: %i\n\tdata: %i %i\n", packet_buffer.flag, packet_buffer.size, packet_buffer.data[0], packet_buffer.data[1]);
#endif

    // On lit le résultat de la lecture
    memcpy(&packet, &packet_buffer, sizeof packet_buffer);

    return packet;
}

u_int8_t isHost(){
    return host;
}

u_int8_t isConnected(){
    return connected;
}

u_int8_t hasClient(){
    return clientConnected;
}