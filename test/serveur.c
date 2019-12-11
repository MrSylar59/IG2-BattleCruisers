#include "includes.h"

void dialogueClt(int, struct sockaddr_in);
void derouteFinFils(int);

int main() {

    int se, sd;
	int sds[2];

	struct sockaddr_in srv;
	struct sockaddr_in clt[2];
	socklen_t cltLen;

    // Mise en place de la déroute
	struct sigaction act;
	
	act.sa_handler = derouteFinFils;
    CHECK(sigemptyset(&act.sa_mask), "appel 2 de sigemptyset");
    act.sa_flags = SA_RESTART;
    CHECK(sigaction(SIGCHLD, &act, NULL), "appel de sigaction");

    // Création de la socket d'écoute
	CHECK(se=socket(PF_INET, SOCK_STREAM, 0), "Failed to create socket");
	
	// Préparation de l'adressage
	srv.sin_family = PF_INET;
	srv.sin_port = htons(PORT_SRV);
	srv.sin_addr.s_addr = INADDR_ANY;
	memset(&srv.sin_zero, 0, 8);
	
	// Association de l'adressage avec la socket d'écoute
	CHECK(bind(se, (struct sockaddr*) &srv, (socklen_t) sizeof srv), "Failed to bind");
	
	// Mise en écoute de la socket
	CHECK(listen(se, 5), "Failed to listen");
	printf("Serveur lancé\n");

	int numClient = 0;

    //Boucle de service
	while(1) {
		cltLen = sizeof clt;
		// Attente d'un appel
		CHECK(sd=accept(se, (struct sockaddr*) &clt[numClient], &cltLen), "Failed to connect");
		printf("Arrivé d'un client.\n");
		numClient++;

		if (numClient <= 2)
			sds[numClient-1] = sd;

		if (numClient == 2){
			// Dialogue avec le client
			int pid = 0;
			CHECK(pid = fork(), "Failed to fork");

			if (pid == 0){
				dialogueClt(sds[0], clt[0]); // Dialogue avec premier client
				dialogueClt(sds[1], clt[1]); // Dialogue avec deuxième client

				getchar();
				printf("Passage en peer-to-peer\n");

				// Le serveur envoie les informations de connexion au client
				


				getchar();
				printf("La partie est terminée: fermeture des sockets\n");
				
				shutdown(sds[0], SHUT_RD);
				shutdown(sds[1], SHUT_RD);
				exit(EXIT_SUCCESS);
			}
			else {
				numClient = 0;
			}
		}
	}

    close(se);
    return 0;
}

void dialogueClt(int sd, struct sockaddr_in clt) {
	char msg[] = "Bonsoir";
    char rep[1024];
	
	// Envoi du message au serveur
	CHECK(read(sd, rep, sizeof rep), "Failed to recieve");

	printf("%s\n", rep);

	CHECK(write(sd, msg, strlen(msg)+1), "Failed to send");
	
	printf("Dialogue terminé.\n");
}

void derouteFinFils(int sig) {
	int status = 0;
	printf("Fin traitement d'un client\n");
	wait(&status);
}