#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

#define NUMEROTHREADS_USUARIOS 3
#define CONTEUDO_IMPRESSAO 0
#define MAXIMO_TEXTO_IMPRESSO 100
#define TRUE 1
#define TAMANHOFILA 3
struct Fila fila;
char impressaoAleatoria[MAXIMO_TEXTO_IMPRESSO];
char bufferThreadsUsuarios[NUMEROTHREADS_USUARIOS][CONTEUDO_IMPRESSAO];
sem_t semaforoThreadUsuario[NUMEROTHREADS_USUARIOS];
sem_t sessaoCritica;
sem_t statusImpressora;
void inserirNoBuffer(int idThreadUsuario);
void gerarFraseAleatoria();
void gerarInterface();
void impressora();

struct BufferThreads {
    int idThread;
    int statusImpressao;
    char texto[MAXIMO_TEXTO_IMPRESSO];
};


struct Fila {
    int capacidade;
    struct BufferThreads buffer[NUMEROTHREADS_USUARIOS];
    int primeiro;
    int ultimo;
    int nItens;
};

void criarFila(struct Fila *f) {
    f->capacidade = TAMANHOFILA;
    f->primeiro = 0;
    f->ultimo = -1;
    f->nItens = -1;
}

void inserir(struct Fila *f, int idThread, int statusImpressao, char *texto) {
    if (f->ultimo == f->capacidade - 1)
        f->ultimo = -1;
    f->ultimo++;
    f->buffer[f->ultimo].idThread = idThread; 
    f->buffer[f->ultimo].statusImpressao = statusImpressao;
    strcpy(f->buffer[f->ultimo].texto , texto);
  
    f->nItens++;

}
struct  BufferThreads remover(struct Fila *f) { // pega o primeiro da fila
    struct BufferThreads temp = f->buffer[f->primeiro++];
    if (f->primeiro == f->capacidade)
        f->primeiro = 0;
    f->nItens--;
    return temp;
}
int estaVazia(struct Fila *f) { // retorna verdadeiro se a fila está vazia
    return (f->nItens == 0);
}
int estaCheia(struct Fila *f) { // retorna verdadeiro se a fila está cheia
    return (f->nItens == f->capacidade);
}
void mostrarFila(struct Fila *f) {
	
    int cont, i;
    i = f->primeiro;
    while (i <= f->ultimo) {
            printf("\n       %d        |      %d     | %s \n", f->buffer[i].idThread, f->buffer[i].statusImpressao, f->buffer[i].texto);
        i++;
    }


    printf("\n\n");

}

void *usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while (TRUE) {


        inserirNoBuffer(idThreadUsuario);
        // retirarDoBuffer(idThreadUsuario);
    }
}

void *interface() {


    while (TRUE) {


        gerarInterface();
     
    }
}

void gerarInterface() {
    system("cls"); //unix system("clear");
    printf("\n ----------- interface -----------------\n");
    printf("E idImpressao ----- excluir impressao \n");
    printf("S idImpressao ----- suspender impressao \n");
    int numeroUsuarios;

    printf(" ID IMPRESSAO   |   STATUS   |  TEXTO IMPRESSAO  ");
     mostrarFila(&fila);
    if (fila.nItens>-1){
    	 mostrarFila(&fila);
    impressora();
	}else{
		printf ("\n nada para imprimir");
	}
   
    sleep(7);
}

void impressora() {
    sem_wait(&statusImpressora);
    int time;

    printf("imprimindo ....... ");
    struct BufferThreads bufferParaImpressao = remover(&fila);
    printf ("imprimindo thread %d", bufferParaImpressao.idThread);
    sleep(10);
	sem_post(&semaforoThreadUsuario[bufferParaImpressao.idThread]); 
 
    sem_post(&statusImpressora);
}

void gerarFraseAleatoria() {
    {
        int posicao = 0, auxiliarAscii;

        srand(time(NULL));
        while (posicao < MAXIMO_TEXTO_IMPRESSO) {

            auxiliarAscii = (65 + rand() % (91 - 65));
            impressaoAleatoria[posicao] = (char) auxiliarAscii;
            posicao++;
        }
    }
}

void inserirNoBuffer(int idThreadUsuario) {

    sem_wait(&sessaoCritica);
    sem_wait(&semaforoThreadUsuario[idThreadUsuario]);
    sleep(4);
    gerarFraseAleatoria();
    inserir(&fila, idThreadUsuario, 1, impressaoAleatoria);   
      printf("Enviando para buffer de impress�o thread id %d \n\n", idThreadUsuario);
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
