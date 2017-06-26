#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <conio.h>
#include <stdbool.h>

#define NUMEROTHREADS_USUARIOS 50
#define CONTEUDO_IMPRESSAO 0
#define MAXIMO_TEXTO_IMPRESSO 100
#define TRUE 1
#define MAXIMO_PAGINAS 40

pthread_t impressoraThread;
pthread_t threadInterface;
void *threadInterface_result;
void *impressora_result;
int tempoAtiva = -1;
int threadImpressao = -1;
int pausarImpressora = 1;
int tempoImpressao = 0;
int contadorImpressoes = 1;
char impressaoAleatoria[MAXIMO_TEXTO_IMPRESSO];
char bufferThreadsUsuarios[NUMEROTHREADS_USUARIOS][CONTEUDO_IMPRESSAO];
sem_t semaforoThreadUsuario[NUMEROTHREADS_USUARIOS];
sem_t sessaoCritica;
int contadorSleep = 0;

typedef struct dadosBuffer {
    int numeroPaginas;
    int id;
    int idUsuario;
    int statusImpressao;
    char texto[200];
} DadosBuffer;
DadosBuffer data;
DadosBuffer dataSuspenso;

typedef struct node {
    DadosBuffer dadosNodo;
    struct node* next;
} Node;

Node* nodoEmImpressao;

typedef struct lista {
    int size;
    Node* head;
} List;
List *l;
List *suspensos;
Node* posicao(List* list, int index);

void inserirNoBuffer(int idThreadUsuario);
void gerarFraseAleatoria();
void gerarInterface();
void* impressora();
void* usuario();
void* interface();
List* iniciarLista();
void inserir(List *list, DadosBuffer data);
void mostrarLista(List* list);
int indexOf(List* list, Node* node);
void apagarNodoPorPosicao(List* list, int index);
Node* posicao(List* list, int index);
bool isEmpty(List* list);
void apagarPrimeiroNodo(List* list);
void trocarNodosDeLugar(List* list, Node* nodeA, Node* nodeB);
Node* menorNodo(List* list, int index);
void ordenarListaMenorMaior(List* list);
Node* findNodeByIdThread(List *list, int idThread);

List* iniciarLista() {
    List* list = (List*) malloc(sizeof (List));
    list->size = 0;
    list->head = NULL;
    return list;
}

void inserir(List *list, DadosBuffer data) {
    Node* node = (Node*) malloc(sizeof (Node));
    node->dadosNodo = data;
    node->next = list->head;
    list->head = node;
    list->size++;
}

void mostrarLista(List* list) {
    Node* pointer = list->head;
    if (pointer == NULL) {
        printf("Lista vazia");
    }
    while (pointer != NULL) {
        printf("\n       %d       |      %d      |      %d      |      %d     | %s \n", pointer->dadosNodo.id, pointer->dadosNodo.idUsuario, pointer->dadosNodo.numeroPaginas, pointer->dadosNodo.statusImpressao, pointer->dadosNodo.texto);


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

void apagarNodoPorPosicao(List* list, int index) {
    if (index == 0) {
        apagarPrimeiroNodo(list);
    } else {
        Node* current = posicao(list, index);
        if (current != NULL) {
            Node* previous = posicao(list, index - 1);
            previous->next = current->next;

            free(current);
            list->size--;
        }


    }
}

Node* posicao(List* list, int index) {
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

void apagarPrimeiroNodo(List* list) {
    if (!isEmpty(list)) {

        Node* pointer = list->head;

        list->head = pointer->next;
        free(pointer);
        list->size--;
    }
}

void trocarNodosDeLugar(List* list, Node* nodeA, Node* nodeB) {

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
        nodeB = posicao(list, indexA);
        indexA = indexB;
        indexB = indexOf(list, nodeB);
    }
    Node* pb = posicao(list, indexB - 1);
    if (nodeA == list->head) {
        list->head = nodeB;

    } else {
        Node* pa = posicao(list, indexA - 1);
        pa->next = nodeB;
    }

    pb->next = nodeA;
    Node* pointer = nodeA->next;
    nodeA->next = nodeB->next;
    nodeB->next = pointer;

}

Node* menorNodo(List* list, int index) {
    Node* pointer = posicao(list, index);
    if (pointer != NULL) {
        Node* minNode = pointer;
        while (pointer != NULL) {
            if (pointer->dadosNodo.numeroPaginas < minNode->dadosNodo.numeroPaginas) {
                minNode = pointer;
            }
            pointer = pointer->next;
        }
        return minNode;
    }
    return NULL;
}

void ordenarListaMenorMaior(List* list) {
    int i;
    for (i = 0; i < list->size - 1; i++) {
        trocarNodosDeLugar(list, posicao(list, i), menorNodo(list, i));
    }
}

void* usuario(void *j) {
    int idThreadUsuario = *(int *) j;
    while (TRUE) {


        inserirNoBuffer(idThreadUsuario);

    }
    pthread_exit(0);
}

Node* findNodeByIdThread(List *list, int idThread) {

    Node* node = list->head;
    int i;
    for (i = 0; i < list->size - 1; i++) {
        if (node->dadosNodo.id == idThread) {
            return node;
        }
        node = node->next;
    }
    return node;
}

void* interface() {

    char opcao;
    int idImpressao;
    while (!kbhit()) { //funcao kbhit da biblioteca conio.h 
        system("color 0F");

        gerarInterface();

    }
    system("color fc");
    fflush(stdin);
    fflush(stdin);

    opcao = getche();
    printf("\n Informe o id da impressao desejada: ");
    scanf("%d", &idImpressao);
    if ((opcao == 'S' || opcao == 's')) {
        if (idImpressao == threadImpressao) {
            printf("Voce nao pode suspender a impressao atual - Processo nao-preemptivo");
            sleep(1);
        } else {
            Node* retorno = (findNodeByIdThread(l, idImpressao));
            if (retorno) {
                dataSuspenso.id = retorno->dadosNodo.id;
                dataSuspenso.statusImpressao = 3;
                dataSuspenso.idUsuario = retorno->dadosNodo.idUsuario;
                dataSuspenso.numeroPaginas = retorno->dadosNodo.numeroPaginas;

                strcpy(dataSuspenso.texto, retorno->dadosNodo.texto);
                inserir(suspensos, dataSuspenso);

                apagarNodoPorPosicao(l, indexOf(l, retorno));

                fflush(stdin);
                sleep(1);
            } else {
                printf("Impressao nao encontrada!");
                sleep(1);
            }
        }

    }else{
    	
	
    if ((opcao == 'R' || opcao == 'r')) {
        if (!isEmpty(suspensos)) {
            if (idImpressao == threadImpressao) {
                printf("Voce nao pode suspender a impressao atual - Processo nao-preemptivo");
                sleep(1);
            } else {
                Node* retorno = (findNodeByIdThread(suspensos, idImpressao));
                if (retorno) {
                	DadosBuffer dataAuxiliar;
                    dataAuxiliar.id = retorno->dadosNodo.id;
                    dataAuxiliar.statusImpressao = 1;
                    dataAuxiliar.numeroPaginas = retorno->dadosNodo.numeroPaginas;
                    dataAuxiliar.idUsuario = retorno->dadosNodo.idUsuario;

                    strcpy(dataAuxiliar.texto, retorno->dadosNodo.texto);
                    inserir(l, dataAuxiliar);

                    apagarNodoPorPosicao(suspensos, indexOf(suspensos, retorno));

                    fflush(stdin);
                    sleep(2);
                } else {
                    printf("Impressao nao encontrada!");
                    sleep(1);
                }
            }
        } else {
            printf("Nao ha impressão para retomar!");
            sleep(1);
        }
    }else{
    	
    if ((opcao == 'E' || opcao == 'e')) {
        if (idImpressao == threadImpressao) {
            pausarImpressora = 0;
            threadImpressao = -1;
            finalizarImpressao();
            printf("\n Impressao atual excluida! ");
            sleep(1);
            pausarImpressora = 1;
            pthread_create(&threadInterface, NULL, interface, NULL);
            pthread_create(&impressoraThread, NULL, impressora, NULL);

            pthread_join(threadInterface, &threadInterface_result);
            pthread_join(impressoraThread, &impressora_result);
        } else {

            apagarNodoPorPosicao(l, indexOf(l, findNodeByIdThread(l, idImpressao)));
            printf("\n ........ Impressao excluida do buffer");
            sleep(2);
        }
    }else{
    printf("\n    >>>>>  Opção indisponivel!")	;
    sleep(1);
	}
}
}
    pthread_create(&threadInterface, NULL, interface, NULL);
    pthread_join(threadInterface, &threadInterface_result);
}

void gerarInterface() {
    system("color 00");
    system("cls"); //unix system("clear");
    printf("\n ------------------------------ GERENCIAR IMPRESSORA  ----------------------------\n");
    printf("\n >> Pressione as opcoes abaixo para interagir com a impressora \n");
    printf("    E ----- excluir impressao \n");
    printf("    S ----- suspender impressao \n");
    printf("    R ----- retomar impressao \n");
    if (threadImpressao>-1) {
        printf("\n\n  >>>>> IMPRIMINDO IMPRESSAO %d , PAGINA %d DE %d  \n\n", threadImpressao, contadorSleep, tempoImpressao);
    } else {
        printf("\n\n  <<<<<< AGUARDANDO IMPRESSAO  \n\n");
    }

    printf(" ID IMPRESSAO   |  ID USUARIO  |   PAGINAS   |   STATUS   |  TEXTO IMPRESSAO \n ");

    if (isEmpty(l)) {
        printf("\n >>>> Nao ha impressoes!");

    } else {
        mostrarLista(l);
        if (!isEmpty(suspensos)) {
            mostrarLista(suspensos);
        }

        sleep(3);
    }


}

void* impressora() {
    while (pausarImpressora) {

        sleep(1);
        contadorSleep = 0;


        if (!isEmpty(l)) {
            nodoEmImpressao = l->head;
            threadImpressao = l->head->dadosNodo.id;
            tempoImpressao = l->head->dadosNodo.numeroPaginas;
            l->head->dadosNodo.statusImpressao = 2;

            if (threadImpressao > 0) {
                while (pausarImpressora && contadorSleep < tempoImpressao) {
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

    apagarNodoPorPosicao(l, indexOf(l, nodoEmImpressao));
}

void gerarFraseAleatoria() {
    {
        int posicao = 0, auxiliarAscii = 0;
        srand(time(NULL));
        fflush(stdin);
        while (posicao < MAXIMO_TEXTO_IMPRESSO) {
            auxiliarAscii = (65 + rand() % (91 - 65));
            impressaoAleatoria[posicao] = (char) auxiliarAscii;
            posicao++;
        }
    }
}

void inserirNoBuffer(int idThreadUsuario) {
    srand(time(NULL));

    sem_wait(&sessaoCritica);
    gerarFraseAleatoria();
    sleep(2);

    data.id = contadorImpressoes;
    data.idUsuario = idThreadUsuario;
    data.statusImpressao = 1;
    data.numeroPaginas = (rand()) % MAXIMO_PAGINAS + 1;
    strcpy(data.texto, impressaoAleatoria);
    fflush(stdin);
    inserir(l, data);
    ordenarListaMenorMaior(l);
    sleep(4);
    contadorImpressoes++;
    sem_post(&sessaoCritica);
}

main() {
    l = iniciarLista();
    suspensos = iniciarLista();
    void *thread_result;

    pthread_t thread[NUMEROTHREADS_USUARIOS];


    int idUsuario;
    sem_init(&sessaoCritica, 0, 1);



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

