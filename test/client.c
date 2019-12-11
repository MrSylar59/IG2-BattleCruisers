#include "includes.h"

void dialogueSrv(int);
void* dialogueClt(void*);

int main(int argc, char** argv){

    int s, se, speer;
	pthread_t th;
	struct sockaddr_in srv, clt;

    if (argc < 3) {
		fprintf(stderr, "usage: ./clt addr port\n");
		exit(666);
	}
    // Création du socket pour envoyer la requête
	CHECK(s=socket(PF_INET, SOCK_STREAM, 0), "Failed to create socket");
	
	// Préparation de l'addressage du serviceà contacter
	srv.sin_family = PF_INET;
	srv.sin_port = htons(atoi(argv[2]));
	srv.sin_addr.s_addr = inet_addr(argv[1]);
	memset(&srv.sin_zero, 0, 8);
	
	// Demande d'une connexion au service
	CHECK(connect(s, (struct sockaddr*) &srv, sizeof srv), "Failed to connect");
	printf("Connexion établie.\n");

	// Dialogue avec le serveur
	dialogueSrv(s);

	getchar();
	printf("Passage en peer-to-peer\n");

	if (argc == 4){
		// Mode serveur pour le client
		CHECK(se=socket(PF_INET, SOCK_STREAM, 0), "Failed to create socket");
		printf("Création de la socket %i\n", se);

		// Préparation de l'adressage
		clt.sin_family = PF_INET;
		clt.sin_port = htons(1337);
		clt.sin_addr.s_addr = INADDR_ANY;
		memset(&clt.sin_zero, 0, 8);

		// Association de l'adressage avec la socket d'écoute
		CHECK(bind(se, (struct sockaddr*) &clt, (socklen_t) sizeof clt), "Failed to bind");

		printf("Création du thread\n");
		CHECK(pthread_create(&th, NULL, dialogueClt, (void*)&se), "Failed to create server thread");

		void* ret = 0;
		pthread_join(th, ret);

	}
	else {
		// Dialogue avec le peer
		CHECK(speer=socket(PF_INET, SOCK_STREAM, 0), "Failed to create peer socket");

		// Préparation de l'addressage du serviceà contacter
		srv.sin_family = PF_INET;
		srv.sin_port = htons(1337);
		srv.sin_addr.s_addr = inet_addr(argv[1]);
		memset(&srv.sin_zero, 0, 8);

		// Demande d'une connexion au service
		CHECK(connect(speer, (struct sockaddr*) &srv, sizeof srv), "Failed to connect");
		printf("Connexion au peer établie.\n");

		// Dialogue avec le peer
		dialogueSrv(speer);

		shutdown(speer, SHUT_WR);
	}
	
	shutdown(s, SHUT_WR);

	return 0;
}

void dialogueSrv(int sa) {
	char msg[] = "Bonjour";
    char rep[1024];
	
	// Envoi du message au serveur
	CHECK(write(sa, msg, strlen(msg)+1), "Failed to send");
	CHECK(read(sa, rep, sizeof rep), "Failed to recieve");

	printf("%s\n", rep);
}

void* dialogueClt(void* s) {
	char msg[] = "Bonjour";
    char rep[1024];

	struct sockaddr_in peer;
	int se = *(int*)s;
	int sd;
	socklen_t peerLen = 0;
	
	// Mise en écoute de la socket
	CHECK(listen(se, 5), "Failed to listen");
	printf("Serveur lancé\n");

	//while(1) {
		peerLen = sizeof peer;
		CHECK(sd=accept(se, (struct sockaddr*) &peer, &peerLen), "Failed to connect");
		printf("Arrivé d'un client.\n");
		// Envoi du message au client
		CHECK(read(sd, rep, sizeof rep), "Failed to recieve");
		printf("%s\n", rep);
		CHECK(write(sd, msg, strlen(msg)+1), "Failed to send");

		shutdown(sd, SHUT_RD);
	//}
	close(se);

	pthread_exit(EXIT_SUCCESS);
}