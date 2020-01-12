#include "../shared/includes.h"

int op = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex;

void* th_exec(void* unused){
    do {

        printf("thread créé, en attente\n");
        pthread_cond_wait(&cond, &mutex);
        printf("Opération reçu: %i\n", op);

        if (op == 5){
            op = 2;
            printf("thread créé signal thread principal\n");
            pthread_cond_signal(&cond);
        }

    }while(op != 0);

    printf("Adios!\n");

    pthread_exit(EXIT_SUCCESS);
}

int main(){
    void* ret = NULL;
    pthread_t th;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&th, NULL, th_exec, NULL);

    sleep(3);
    op = 3;
    printf("thread principal signal le thread en attente\n");
    pthread_cond_signal(&cond);

    sleep(3);
    op = 5;
    printf("thread principal signal le thread en attente\n");
    pthread_cond_signal(&cond);

    printf("thread principal en attente\n");
    pthread_cond_wait(&cond, &mutex);
    printf("Opértion reçu: %i\n", op);
    pthread_mutex_unlock(&mutex); // à faire pour ne pas bloquer les deux threads

    sleep(3);
    op = 1;
    printf("thread principal signal le thread en attente\n");
    pthread_cond_signal(&cond);

    sleep(3);
    op = 0;
    printf("thread principal signal le thread en attente\n");
    pthread_cond_signal(&cond);

    pthread_join(th, &ret);

    return EXIT_SUCCESS;
}