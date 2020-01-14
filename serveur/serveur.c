#include "../shared/includes.h"

sem_t* mutex;
sem_t* file_request;

void dialogueClt (int, struct sockaddr_in);

void acquitterLesFils(int signal_number)
{
	int status;
    printf("\nSIGCHLD reçu, fin du fils %d\n",signal_number);
    wait(&status);
}

int ajouterRoom(char* ip,int port){
	FILE* pf;
	pf = fopen("memoire.dat", "r+");
	char* test_ip=(char *) malloc(20);
	int test_port;
	int test=0;
	int id;
	while(!feof(pf)){
		fscanf(pf,"%d : %s : %d",&id,test_ip,&test_port);
		if (strcmp(test_ip,ip) == 0 && test_port == port)
		{
			test = 1;
		}
	}
	printf("%d\n",test );
	if (test == 0)
	{
		FILE* pfile;
		pfile = fopen("id.dat", "r");
		fscanf(pfile,"%d",&id);
		fclose(pfile);
		pfile = fopen("id.dat", "w");
		id++;
		fprintf(pfile,"%d",id);
		fclose(pfile);
		fprintf(pf, "%d : %s : %d\n",id,ip,port);
	}
	fclose(pf);
	free(test_ip);
	return id;
}

int rejoindreRoom(int id){
	FILE* pf;
	int test=0;
	char* test_ip=(char *) malloc(20);
	int test_port;
	int test_id;
	sem_wait(mutex);	
	pf = fopen("memoire.dat","r");
	printf("%d\n",id);
	while(!feof(pf)){
		fscanf(pf,"%d : %s : %d",&test_id,test_ip,&test_port);
		if (test_id == id)
		{
			test = 1;
		}
	}
	sem_post(mutex);
	int mdp;
	sem_wait(file_request);
	if (test == 1)
	{
		pf = fopen("request.dat","r");
		while(!feof(pf)){
			test_id = -1;
			fscanf(pf,"%d : %d",&test_id,&mdp);
			if (test_id == id)
			{
				sem_post(file_request);
				printf("already : %d\n",mdp );
				return mdp;
			}
		}
		fclose(pf);
		pf = fopen("request.dat","a");
		printf("mdp\n");
		mdp = rand()%1000;
		fprintf(pf, "%d : %d\n",id,mdp);
		fclose(pf);
		sem_post(file_request);
		printf("%d\n",mdp);
		return mdp;
	}
	return -1;
}

int verifrequest(int id, int mdp){
	FILE* pf;
	FILE* pfile;
	int test = 0;
	int test_mdp;
	int test_id;
	printf("Start id: %d mdp: %d\n",id,mdp);
	pf = fopen("request.dat","r");
	pfile = fopen("request2.dat","w");
	while(!feof(pf)){
		test_id = -1;
		fscanf(pf,"%d : %d",&test_id,&test_mdp);
		printf("id: %d %d mdp: %d %d\n",id,test_id,mdp,test_mdp);
		if (test_id == id && test_mdp == mdp)
		{
			test = 1;
		}else{
			if (test_id != -1)
			{
				fprintf(pfile, "%d : %d\n",test_id,test_mdp);
			}
		}
	}
	fclose(pf);
	printf("Middle\n");
	pf = fopen("request.dat","w");
	pfile = fopen("request2.dat","r");
	while(!feof(pfile)){
		test_id = -1;
		fscanf(pfile,"%d : %d",&test_id,&test_mdp);
		if (test_id != -1)
		{
			fprintf(pf, "%d : %d\n",test_id,test_mdp);
		}
	}
	printf("End\n");
	return test;
}

int supprimerRoom(char* ip,int port){
	FILE* pf;
	FILE* pfile;
	pf = fopen("memoire.dat", "r+");
	pfile = fopen("save.dat","w");
	char* test_ip=(char *) malloc(20);
	int test_port;
	int test=0;
	int test_id;
	int id = -1;
	while(!feof(pf)){
		test_id = -1;
		fscanf(pf,"%d : %s : %d",&test_id,test_ip,&test_port);
		if ((strcmp(test_ip,ip) != 0 || test_port != port ) && test_id != -1)
		{
			fprintf(pfile, "%d : %s : %d\n",test_id,test_ip,test_port);
		}else{
			if (test_id != -1)
			{
				id = test_id;
			}
		}
	}
	fclose(pf);
	fclose(pfile);
	pf = fopen("memoire.dat", "w");
	pfile = fopen("save.dat","r");
	while(!feof(pfile)){
		test_id = -1;
		fscanf(pfile,"%d : %s : %d",&test_id,test_ip,&test_port);
		if (test_id != -1)
		{
			fprintf(pf, "%d : %s : %d\n",test_id,test_ip,test_port);
		}
	}
	fclose(pf);
	fclose(pfile);
	return id;
}

char* LireRoom(){
	FILE* pf;
	int MAX_LIGNE = 50;
	char* ligne=(char *) malloc(MAX_LIGNE);
	char* all=(char *) malloc(MAX_BUFF);
	char* id_char = (char *) malloc( sizeof(int));
	char* port_char = (char *) malloc( sizeof(int));	
	int id;
	int port;
	pf = fopen("memoire.dat", "r");
	if (pf != NULL)
	{
		while(!feof(pf)){
			id = -1;
			fscanf(pf,"%d : %s : %d",&id,ligne,&port);
			if (id != -1)
			{
				sprintf(id_char,"%d",id);
				sprintf(port_char,"%d",port);
	    		strcat(all,id_char);
				strcat(all," : ");
				strcat(all,ligne);
				strcat(all," : ");
				strcat(all,port_char);
				strcat(all,"\n");
			}
		}
		int size = strlen(all);
		all[size-1] = '\0';
	}
	else{
		printf("pf null\n");
	}
	fclose(pf);
	free(ligne);
	free(id_char);
	free(port_char);
	return all;
}


int main() {
	int se, sd;
	struct sockaddr_in svc, clt;
	socklen_t cltLen;

	struct sigaction newact;
	// Création de la socket de réception d’écoute des appels
	CHECK(se=socket(PF_INET, SOCK_STREAM, 0), "Can't create");
	// Préparation de l’adressage du service (d’appel)
	svc.sin_family = PF_INET;
	svc.sin_port = htons (PORT_SRV);
	svc.sin_addr.s_addr = INADDR_ANY;
	memset(&svc.sin_zero, 0, 8);
	// Association de l’adressage préparé avec la socket d’écoute
	printf("Création\n");
	CHECK(bind(se, (struct sockaddr *) &svc, sizeof svc) , "Can't bind");
	// Mise en écoute de la socket
 	printf("Ecoute\n");
 	CHECK(listen(se, 5) , "Can't calibrate");
	// Boucle permanente de service

	newact.sa_handler = acquitterLesFils;  
	CHECK(sigemptyset(&newact.sa_mask),"problème sigemptyset"); 
	newact.sa_flags = SA_RESTART;           	 
	CHECK(sigaction(SIGCHLD, &newact, NULL),"problème sigaction");

	mutex = sem_open("/mutex", O_CREAT|O_EXCL, 0644, 1);
	sem_unlink("/mutex");
	file_request = sem_open("/file_request", O_CREAT|O_EXCL, 0644, 1);
	sem_unlink("/file_request");

	while (1) {
		// Attente d’un appel
		cltLen = sizeof(clt);
		CHECK(sd=accept(se, (struct sockaddr *)&clt, &cltLen) , "Can't connect");
		printf("Client trouvé\n");
		// Dialogue avec le client

		int pid = fork();
		if (pid == 0)
		{
			dialogueClt (sd , clt);
			shutdown(sd,SHUT_RD);
			printf("fin lecture\n");
			exit(1);
		}
		//close(sd);
	}
	close(se);
	return 0;
}  

void dialogueClt (int sd, struct sockaddr_in clt) {
	char requete[MAX_BUFF];
	char adresse[MAX_BUFF];
	char port[MAX_BUFF];
	do {
		read(sd, buffer, sizeof(buffer));
		printf("Message reçu :\n");
		sscanf (buffer, "%s : %s : %s",requete,adresse,port); 
		switch (atoi(requete)) {
			case 0 : 
				printf("Au revoir\n");
			break;
			case 1 : 
				printf("Creation Room\n");
				printf("%s\n",buffer);
				printf("%s\n",adresse);
				printf("%s\n",port);
				sem_wait(mutex);
				int id = ajouterRoom(adresse,(int)*port);
				sem_post(mutex);
				int fullSize = strlen(adresse)+strlen(port)+3+sizeof(int);
				char* MSG = (char *) malloc( fullSize );
    			char* id_char = (char *) malloc( sizeof(int));
    			sprintf(id_char,"%d",id);
    			strcpy(MSG,id_char);
    			strcat(MSG," : ");	
    			strcat(MSG,adresse);
    			strcat(MSG," : ");
    			strcat(MSG,port);
				printf("%s\n",MSG );
				CHECK(write(sd, MSG, strlen(MSG)), "Can't send");
				free(id_char);
				free(MSG);
			break;
			case 2 :
				printf("Demande List\n");
				sem_wait(mutex);
				char* liste = LireRoom();
				printf("%s\n",liste);
				sem_post(mutex);
				CHECK(write(sd,liste , strlen(liste)+1), "Can't send");
			break;
			case 3:
				printf("Join Room\n");
				printf("%s\n",adresse);
				int mdp = rejoindreRoom(atoi(adresse));
				printf("join mdp :%d\n",mdp );
				//TODO MSG
				MSG = (char *) malloc( fullSize );
				char* char_mdp = (char *) malloc( sizeof(int));
				if (mdp != -1)
				{
					strcpy(MSG,"1");
				}else{
					strcpy(MSG,"0");
				}
    			strcat(MSG," : ");
    			sprintf(char_mdp,"%d",mdp);
    			strcat(MSG,char_mdp);
				CHECK(write(sd, MSG, strlen(MSG)), "Can't send");
			break;
			case 4 :
				printf("Lancement partie\n");
				sem_wait(mutex);
				id = supprimerRoom(adresse,(int)*port);
				sem_post(mutex);
				fullSize = strlen(adresse)+strlen(port)+3+sizeof(int);
				MSG = (char *) malloc( fullSize );
				id_char = (char *) malloc( sizeof(int));
				sprintf(id_char,"%d",id);
    			strcpy(MSG,id_char);
    			strcat(MSG," : ");
    			strcat(MSG,adresse);
    			strcat(MSG," : ");
    			strcat(MSG,port);
				CHECK(write(sd, MSG, strlen(MSG)), "Can't send");
				free(id_char);
				free(MSG);
			break;
			case 5 :
				printf("Verif mdp\n");
				sem_wait(file_request);
				int test = verifrequest(atoi(adresse),atoi(port));
				sem_post(file_request);
				MSG = (char *) malloc( fullSize );
				if (test == 1)
				{
					strcpy(MSG,"1");
				}else{
					strcpy(MSG,"0");
				}
				CHECK(write(sd, MSG, strlen(MSG)), "Can't send");
			break;
			default :
			printf("NOK : message recu %s\n", buffer); 
			break;
		}
	} while ( atoi(requete) != 0);
}