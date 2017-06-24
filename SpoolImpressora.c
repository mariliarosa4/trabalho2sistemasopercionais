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
#include <stdbool.h>
pthread_t impressoraThread;
pthread_t threadInterface;
void *threadInterface_result;
void *impressora_result;
int statusImpressoraAtiva = 0;
int tempoAtiva = -1;
int threadImpressao = -1;
int pausarImpressora = 1;
char impressaoAleatoria[MAXIMO_TEXTO_IMPRESSO];
char bufferThreadsUsuarios[NUMEROTHREADS_USUARIOS][CONTEUDO_IMPRESSAO];
sem_t semaforoThreadUsuario[NUMEROTHREADS_USUARIOS];
sem_t sessaoCritica;
sem_t statusImpressora;


void inserirNoBuffer(int idThreadUsuario);
void gerarFraseAleatoria();
void gerarInterface();
void* impressora();

typedef struct dataNode {
    int id;
    int statusImpressao;
    char texto[100];
} DataNode;
DataNode data;

typedef struct node {
    DataNode data;
    struct node* next;
} Node;

Node* nodoEmImpressao;

typedef struct list {
    int size;
    Node* head;
} List;
List *l;

Node* atPos(List* list, int index);

List* createList() {
    List* list = (List*) malloc(sizeof (List));
    list->size = 0;
    list->head = NULL;
    return list;
}

void push(List *list, DataNode data) {
    Node* node = (Node*) malloc(sizeof (Node));
    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size++;
}

void printList(List* list) {
    Node* pointer = list->head;
    if (pointer == NULL) {
        printf("Lista vazia");
    }
    while (pointer != NULL) {
        printf("\n       %d        |      %d     | %s \n", pointer->data.id, pointer->data.statusImpressao, pointer->data.texto);


        pointer = pointer->next;
    }
}

int indexOf(List* list, Node* node) {
    if (node != NULL) {
        Node* pointer = list->head;
        int index = 0;

        while (pointer != node && pointer != NULL) {
            pointer = pointer->next;
            index++;
        }
        if (pointer != NULL) {
            return index;
        }
    }
    return -1;
}

void erase(List* list, int index) {
    if (index == 0) {
        pop(list);
    } else {
        Node* current = atPos(list, index);
        if (current != NULL) {
            Node* previous = atPos(list, index - 1);
            previous->next = current->next;

            free(current);
            list->size--;
        }


    }
}

Node* atPos(List* list, int index) {
    if (index >= 0 && index < list->size) {
        Node* node = list->head;
        int i;
        for (i = 0; i < index; i++) {
            node = node->next;
        }
        return node;
    }
}

bool isEmpty(List* list) {
    return (list->size == 0);
}

void pop(List* list) {
    if (!isEmpty(list)) {

        Node* pointer = list->head;

        list->head = pointer->next;
        free(pointer);
        list->size--;
    }
}

void xchgNodes(List* list, Node* nodeA, Node* nodeB) {

    if (nodeA == nodeB) {
        return;
    }
    int indexA = indexOf(list, nodeA);
    int indexB = indexOf(list, nodeB);

    if (indexA == -1 || indexB == -1) {
        return;
    }
    if (indexA > indexB) {
        nodeA = nodeB;
        nodeB = atPos(list, indexA);
        indexA = indexB;
        indexB = indexOf(list, nodeB);
    }
    Node* pb = atPos(list, indexB - 1);
    if (nodeA == list->head) {
        list->head = nodeB;

    } else {
        Node* pa = atPos(list, indexA - 1);
        pa->next = nodeB;
    }

    pb->next = nodeA;
    Node* pointer = nodeA->next;
    nodeA->next = nodeB->next;
    nodeB->next = pointer;

}

Node* max(List* list, int index) {
    Node* pointer = atPos(list, index);
    if (pointer != NULL) {
        Node* maxNode = pointer;
        while (pointer != NULL) {
            if (pointer->data.id > maxNode->data.id) {
                maxNode = pointer;
            }
            pointer = pointer->next;
        }
        return maxNode;
    }
    return NULL;
}

void decSort(List* list) {
    int i;
    for (i = 0; i < list->size - 1; i++) {
        xchgNodes(list, atPos(list, i), max(list, i));
    }
}

void* usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while (TRUE) {


        inserirNoBuffer(idThreadUsuario);

    }
    pthread_exit(0);
}
	Node* findNodeByIdThread(List *list, int idThread){
			
			Node* node=list->head;
			int i;
			for(i=0;i<list->size-1;i++){
				if(node->data.id==5){
					return node;
				}
				node=node->next;
			}
			return node;
		}
void* interface() {

    char opcao;
    int idImpressao;
    while (!kbhit()) { //funcao kbhit da biblioteca conio.h 


        gerarInterface();

    }
    system("color fc");
    fflush(stdin);
    fflush(stdin);

    opcao = getche();
    printf("\n Informe o id da impressao desejada: ");
    scanf("%d", &idImpressao);
    if ((opcao == 'S' || opcao == 's') && idImpressao == threadImpressao) {
        pausarImpressora = 0;
        threadImpressao = -1;
        finalizarImpressao();
        printf("\n Impressao suspensa");
        sleep(1);
         pausarImpressora = 1;
    pthread_create(&threadInterface, NULL, interface, NULL);
    pthread_create(&impressoraThread, NULL, impressora, NULL);

    pthread_join(threadInterface, &threadInterface_result);
    pthread_join(impressoraThread, &impressora_result);
    }
    if ((opcao == 'E' || opcao == 'e')) {
       erase(l, indexOf(l, findNodeByIdThread(l,idImpressao)));
        printf("\n ........ Impressao excluida do buffer");
        sleep(2);
    }
    pthread_create(&threadInterface, NULL, interface, NULL);
 pthread_join(threadInterface, &threadInterface_result);
}

void gerarInterface() {
    system("cls"); //unix system("clear");
    printf("\n ----------- interface -----------------\n");
    printf("\n Pressione as opcoes abaixo para interagir com a impressora \n");
    printf("E ----- excluir impressao \n");
    printf("S ----- suspender impressao \n");
    printf("\n imprimindo thread %d  \n", threadImpressao);
    printf(" ID IMPRESSAO   |   STATUS   |  TEXTO IMPRESSAO \n ");

    if (isEmpty(l)) {
        printf("\n nada para imprimir");

    } else {
        printList(l);
        sleep(3);
    }


}

void* impressora() {
    while (pausarImpressora) {

        sleep(1);
        int contadorSleep = 0;


        if (!isEmpty(l)) {
            nodoEmImpressao = l->head;
            threadImpressao = l->head->data.id;

            if (threadImpressao > 0) {
                while (pausarImpressora && contadorSleep < 20) {
                    sleep(1);
                    contadorSleep++;
                }
                if (pausarImpressora) {
                    finalizarImpressao();
                }
            }
        }
    }
}

void finalizarImpressao() {
    printf("\n\n ...... finalizando impressao");
    sleep(2);
    sem_post(&semaforoThreadUsuario[threadImpressao]);
    erase(l, indexOf(l, nodoEmImpressao));
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
    system("color 02");
    data.id = idThreadUsuario;
    data.statusImpressao = 1;
    strcpy(data.texto, impressaoAleatoria);
    push(l, data);
    decSort(l);
    sleep(4);
    sem_post(&sessaoCritica);
}

main() {
    l = createList();
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

