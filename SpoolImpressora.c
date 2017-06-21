#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <conio.h>

#define NUMEROTHREADS_USUARIOS 50
#define CONTEUDO_IMPRESSAO 0
#define MAXIMO_TEXTO_IMPRESSO 100
#define TRUE 1
#define TAMANHOFILA 50
    pthread_t impressoraThread;
pthread_t threadInterface;
    void *threadInterface_result;
    void *impressora_result;
int statusImpressoraAtiva = 0;
int tempoAtiva = -1;
int threadImpressao = -1;
int pausarImpressora = 1;
struct Fila fila;
char impressaoAleatoria[MAXIMO_TEXTO_IMPRESSO];
char bufferThreadsUsuarios[NUMEROTHREADS_USUARIOS][CONTEUDO_IMPRESSAO];
sem_t semaforoThreadUsuario[NUMEROTHREADS_USUARIOS];
sem_t sessaoCritica;
sem_t statusImpressora;

void inserirNoBuffer(int idThreadUsuario);
void gerarFraseAleatoria();
void gerarInterface();
void* impressora();
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
    f->nItens = 0;
}

void inserir(struct Fila *f, int idThread, int statusImpressao, char *texto) {
    if (f->ultimo == f->capacidade - 1)
        f->ultimo = -1;
    f->ultimo++;
    f->buffer[f->ultimo].idThread = idThread;
    f->buffer[f->ultimo].statusImpressao = statusImpressao;
    strcpy(f->buffer[f->ultimo].texto, texto);

    f->nItens++;

}

struct BufferThreads remover(struct Fila *f) {
    struct BufferThreads temp = f->buffer[f->primeiro++];
    if (f->primeiro == f->capacidade)
        f->primeiro = 0;
    f->nItens--;
    return temp;
}

struct BufferThreads primeiroFila(struct Fila *f) {
    return f->buffer[f->primeiro];
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

void* usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while (TRUE) {


        inserirNoBuffer(idThreadUsuario);

    }
    pthread_exit(0);
}

void* interface() {

    char opcao;
    int idImpressao;
    while (!kbhit()) { //funcao kbhit da biblioteca conio.h 


        gerarInterface();

    }
    fflush(stdin);
    printf("E ----- excluir impressao \n");
    printf("S ----- suspender impressao \n");
    opcao = getchar();
    printf("\n Informe o id da impressao desejada");
    scanf("%d", &idImpressao);
if (opcao=='E' && idImpressao==threadImpressao){
	pausarImpressora=0;
	threadImpressao=-1;
	 finalizarImpressao();
	printf("pausar impressora");
	sleep(1);
}
    printf("tchau");
    pausarImpressora=1;
    pthread_create(&threadInterface, NULL, interface, NULL);
    pthread_join(threadInterface, &threadInterface_result);
    sleep(3);
    pthread_create(&impressoraThread, NULL, impressora, NULL);
     pthread_join(impressoraThread, &impressora_result);

}

void gerarInterface() {
    system("cls"); //unix system("clear");
    printf("\n ----------- interface -----------------\n");
    printf("\n Pressione qualquer tecla para interagir com a impressora\n ");
    printf("\n imprimindo thread %d  \n", threadImpressao);
    printf(" ID IMPRESSAO   |   STATUS   |  TEXTO IMPRESSAO \n ");


    if (fila.nItens == 0) {
        printf("\n nada para imprimir");

    } else {
        mostrarFila(&fila);
        sleep(3);
    }


}

void* impressora() {
	  printf("to na impressora");
    while (pausarImpressora) {
        printf("to na impressora");
        sleep(1);
int contadorSleep = 0;

        struct BufferThreads bufferParaImpressao = primeiroFila(&fila);
        threadImpressao = bufferParaImpressao.idThread;

        if (threadImpressao > 0) {
        	while (pausarImpressora && contadorSleep<10){
        		 sleep(1);
        		 contadorSleep++;
			}
           
             finalizarImpressao();
             
			         }




    }
    printf("impressora pausada");
}
void finalizarImpressao(){
	sem_post(&semaforoThreadUsuario[threadImpressao]);
            remover(&fila);
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
sem_wait(&semaforoThreadUsuario[idThreadUsuario]);
    sem_wait(&sessaoCritica);


    gerarFraseAleatoria();
    sleep(2);
    inserir(&fila, idThreadUsuario, 1, impressaoAleatoria);
sleep(2);
     
    sem_post(&sessaoCritica);
}

main() {
    criarFila(&fila);
    void *thread_result;

    pthread_t thread[NUMEROTHREADS_USUARIOS];


    int idUsuario;
    sem_init(&sessaoCritica, 0, 1);
    sem_init(&statusImpressora, 0, 1);


    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {
        sem_init(&semaforoThreadUsuario[idUsuario], 0, 1);


        pthread_create(&thread[idUsuario], NULL, usuario, &idUsuario);
    }
    pthread_create(&threadInterface, NULL, interface, NULL);
    pthread_create(&impressoraThread, NULL, impressora, NULL);
    for (idUsuario = 0; idUsuario < NUMEROTHREADS_USUARIOS; idUsuario++) {


        pthread_join(thread[idUsuario], &thread_result);
    }
    pthread_join(threadInterface, &threadInterface_result);

    pthread_join(impressoraThread, &impressora_result);

}
