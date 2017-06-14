#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUMEROTHREADS_USUARIOS 20
#define CONTEUDO_IMPRESSAO 0
#define TRUE 1

char bufferThreadsUsuarios[NUMEROTHREADS_USUARIOS][CONTEUDO_IMPRESSAO];
sem_t semaforoThreadUsuario[NUMEROTHREADS_USUARIOS];
sem_t sessaoCritica;
sem_t statusImpressora;

struct BufferThreads {
    int idThread;
    char texto[100];
} BUFFER[NUMEROTHREADS_USUARIOS];

void *usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while (TRUE) {
        inserirNoBuffer(idThreadUsuario);
        retirarDoBuffer(idThreadUsuario);
    }
}

void *interface() {


    while (TRUE) {
        gerarInterface();
        impressora();
    }
}

void gerarInterface() {
    printf("\ninterface");
    sleep(2);
    int numeroUsuarios;
    system("cls"); //unix system("clear");

    for (numeroUsuarios = 0; numeroUsuarios < NUMEROTHREADS_USUARIOS; numeroUsuarios++) {

        if (BUFFER[numeroUsuarios].texto != NULL)
            printf("\nsou interface e acesso outras coisas %c", BUFFER[numeroUsuarios].texto);
    }
}

void impressora() {
    sem_wait(&statusImpressora);
    printf("imprimindo ....... ");
    sleep(10);
    sem_wait(&statusImpressora);
}

void inserirNoBuffer(int idThreadUsuario) {
    sleep(4);
    sem_wait(&sessaoCritica);
    sem_wait(&semaforoThreadUsuario[idThreadUsuario]);
    printf("Enviando para buffer de impressão...");
    BUFFER[idThreadUsuario].texto[0] = "k";
    BUFFER[idThreadUsuario].idThread = idThreadUsuario;
    printf("           %c \n\n", BUFFER[idThreadUsuario].texto[0]);
    sleep(4);
    sem_post(&sessaoCritica);
}

void retirarDoBuffer(int idThreadUsuario) {
    sleep(3);
    sem_post(&semaforoThreadUsuario[idThreadUsuario]);
}

main() {
    void *thread_result;
    void *threadInterface_result;
    pthread_t thread[NUMEROTHREADS_USUARIOS];
    pthread_t threadInterface;
    int idUsuario;
    sem_init(&sessaoCritica, 0, 1);
    sem_init(&statusImpressora, 0, 1);

    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {
        sem_init(&semaforoThreadUsuario[idUsuario], 0, 1);
        if (idUsuario == 0)
            pthread_create(&threadInterface, NULL, interface, NULL);
        pthread_create(&thread[idUsuario], NULL, usuario, &idUsuario);
    }
    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {
        if (idUsuario == 0)
            pthread_join(threadInterface, &threadInterface_result);
        pthread_join(thread[idUsuario], &thread_result);
    }


}
