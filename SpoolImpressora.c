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
sem_t sessaoCritica ;
void *usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while(TRUE){
    	inserirNoBuffer(idThreadUsuario);
    	retirarDoBuffer(idThreadUsuario );
	}

    
}

void inserirNoBuffer(int idThreadUsuario){
	sem_wait(&sessaoCritica);
	sem_wait(&semaforoThreadUsuario[idThreadUsuario]);
	printf("Enviando para buffer de impressão...");
	bufferThreadsUsuarios[idThreadUsuario][CONTEUDO_IMPRESSAO]="b";
	printf("           %d \n\n", idThreadUsuario);

	sem_post(&sessaoCritica);
 

	
}
void retirarDoBuffer(int idThreadUsuario){
	sleep(3);
		sem_post(&semaforoThreadUsuario[idThreadUsuario]);
}
main() {
	 void *thread_result;
    pthread_t thread[NUMEROTHREADS_USUARIOS];
    int idUsuario;
      sem_init(&sessaoCritica, 0, 1);
    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {
       sem_init(&semaforoThreadUsuario[idUsuario], 0, 1);

        pthread_create(&thread[idUsuario], NULL, usuario, &idUsuario);
    }
    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {
        pthread_join(thread[idUsuario], &thread_result);
    }
}
