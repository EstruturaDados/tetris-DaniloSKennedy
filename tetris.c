#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3
#define TAM_HIST 20

// =======================
// Estrutura da Peça
// =======================

typedef struct {
    char nome;
    int id;
} Peca;

// =======================
// Fila Circular
// =======================

typedef struct {
    Peca dados[TAM_FILA];
    int frente;
    int tras;
    int tamanho;
} Fila;

// =======================
// Pilha
// =======================

typedef struct {
    Peca dados[TAM_PILHA];
    int topo;
} Pilha;

// =======================
// Histórico (para desfazer)
// =======================

typedef struct {
    Fila fila;
    Pilha pilha;
} Estado;

Estado historico[TAM_HIST];
int topoHist = -1;

// =======================
// Protótipos
// =======================

Peca gerarPeca();
void inicializarFila(Fila *f);
void enfileirar(Fila *f, Peca p);
Peca desenfileirar(Fila *f);
void mostrarFila(Fila *f);

void inicializarPilha(Pilha *p);
void push(Pilha *p, Peca peca);
Peca pop(Pilha *p);
void mostrarPilha(Pilha *p);

void salvarEstado(Fila *f, Pilha *p);
void desfazer(Fila *f, Pilha *p);

void trocarTopoComFrente(Fila *f, Pilha *p);
void inverterFilaComPilha(Fila *f, Pilha *p);

void mostrarTudo(Fila *f, Pilha *p);

// =======================
// Implementação
// =======================

Peca gerarPeca() {
    char tipos[] = {'I','O','T','L','J','S','Z'};
    Peca p;
    p.nome = tipos[rand() % 7];
    p.id = rand() % 1000;
    return p;
}

void inicializarFila(Fila *f) {
    f->frente = 0;
    f->tras = 0;
    f->tamanho = 0;
    for(int i=0;i<TAM_FILA;i++)
        enfileirar(f, gerarPeca());
}

void enfileirar(Fila *f, Peca p) {
    if (f->tamanho == TAM_FILA) return;
    f->dados[f->tras] = p;
    f->tras = (f->tras + 1) % TAM_FILA;
    f->tamanho++;
}

Peca desenfileirar(Fila *f) {
    Peca vazio = {'-', -1};
    if (f->tamanho == 0) return vazio;

    Peca p = f->dados[f->frente];
    f->frente = (f->frente + 1) % TAM_FILA;
    f->tamanho--;
    return p;
}

void mostrarFila(Fila *f) {
    printf("Fila: ");
    for(int i=0;i<f->tamanho;i++) {
        int idx = (f->frente + i) % TAM_FILA;
        printf("[%c-%d] ", f->dados[idx].nome, f->dados[idx].id);
    }
    printf("\n");
}

void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

void push(Pilha *p, Peca peca) {
    if (p->topo == TAM_PILHA-1) {
        printf("Pilha cheia!\n");
        return;
    }
    p->dados[++p->topo] = peca;
}

Peca pop(Pilha *p) {
    Peca vazio = {'-', -1};
    if (p->topo == -1) {
        printf("Pilha vazia!\n");
        return vazio;
    }
    return p->dados[p->topo--];
}

void mostrarPilha(Pilha *p) {
    printf("Pilha: ");
    for(int i=0;i<=p->topo;i++)
        printf("[%c-%d] ", p->dados[i].nome, p->dados[i].id);
    printf("\n");
}

void salvarEstado(Fila *f, Pilha *p) {
    if (topoHist == TAM_HIST-1) return;
    historico[++topoHist].fila = *f;
    historico[topoHist].pilha = *p;
}

void desfazer(Fila *f, Pilha *p) {
    if (topoHist < 0) {
        printf("Nada para desfazer!\n");
        return;
    }
    *f = historico[topoHist].fila;
    *p = historico[topoHist].pilha;
    topoHist--;
}

void trocarTopoComFrente(Fila *f, Pilha *p) {
    if (p->topo == -1 || f->tamanho == 0) {
        printf("Operação inválida!\n");
        return;
    }
    Peca temp = f->dados[f->frente];
    f->dados[f->frente] = p->dados[p->topo];
    p->dados[p->topo] = temp;
}

void inverterFilaComPilha(Fila *f, Pilha *p) {
    if (p->topo == -1) {
        printf("Pilha vazia!\n");
        return;
    }

    int count = p->topo + 1;
    if (count != f->tamanho) {
        printf("Para inverter, pilha e fila devem ter mesmo tamanho!\n");
        return;
    }

    for(int i=0;i<count;i++) {
        int idx = (f->frente + i) % TAM_FILA;
        Peca temp = f->dados[idx];
        f->dados[idx] = p->dados[count-1-i];
        p->dados[count-1-i] = temp;
    }
}

void mostrarTudo(Fila *f, Pilha *p) {
    printf("\n-------------------------\n");
    mostrarFila(f);
    mostrarPilha(p);
    printf("-------------------------\n");
}

// =======================
// MAIN (NÍVEL MESTRE)
// =======================

int main() {
    srand(time(NULL));

    Fila fila;
    Pilha pilha;

    inicializarFila(&fila);
    inicializarPilha(&pilha);

    int opcao;

    do {
        mostrarTudo(&fila, &pilha);

        printf("\n1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("4 - Trocar topo da pilha com frente da fila\n");
        printf("5 - Desfazer última jogada\n");
        printf("6 - Inverter fila com pilha\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                salvarEstado(&fila, &pilha);
                desenfileirar(&fila);
                enfileirar(&fila, gerarPeca());
                break;

            case 2:
                salvarEstado(&fila, &pilha);
                push(&pilha, desenfileirar(&fila));
                enfileirar(&fila, gerarPeca());
                break;

            case 3:
                salvarEstado(&fila, &pilha);
                enfileirar(&fila, pop(&pilha));
                break;

            case 4:
                salvarEstado(&fila, &pilha);
                trocarTopoComFrente(&fila, &pilha);
                break;

            case 5:
                desfazer(&fila, &pilha);
                break;

            case 6:
                salvarEstado(&fila, &pilha);
                inverterFilaComPilha(&fila, &pilha);
                break;

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opção inválida!\n");
        }

    } while(opcao != 0);

    return 0;
}
