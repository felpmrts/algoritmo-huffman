#include <stdlib.h>
#include "FilaPrioridade.h"

// Min-heap em vetor: pai = (i-1)/2, esq = 2*i+1, dir = 2*i+2
struct FilaPrioridade {
    NoHuffman **itens;
    int tamanho;
    int capacidade;
};

static void trocar_nos(NoHuffman **a, NoHuffman **b) {
    NoHuffman *temp = *a;
    *a = *b;
    *b = temp;
}

// Sobe o elemento no indice informado ate restaurar o min-heap
static void subir_heap(FilaPrioridade *fila, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (comparar_nos_huffman(fila->itens[indice], fila->itens[pai]) >= 0)
            break;
        trocar_nos(&fila->itens[indice], &fila->itens[pai]);
        indice = pai;
    }
}

// Desce o elemento trocando-o sempre pelo menor filho ate restaurar o min-heap
static void descer_heap(FilaPrioridade *fila, int indice) {
    while (1) {
        int esq = 2 * indice + 1;
        int dir = 2 * indice + 2;
        int menor = indice;

        if (esq < fila->tamanho &&
            comparar_nos_huffman(fila->itens[esq], fila->itens[menor]) < 0)
            menor = esq;

        if (dir < fila->tamanho &&
            comparar_nos_huffman(fila->itens[dir], fila->itens[menor]) < 0)
            menor = dir;

        if (menor == indice) break;

        trocar_nos(&fila->itens[indice], &fila->itens[menor]);
        indice = menor;
    }
}

// Aloca a fila de prioridade e o vetor de ponteiros do heap
FilaPrioridade *criar_fila_prioridade(int capacidade) {
    if (capacidade <= 0) return NULL;

    FilaPrioridade *fila = (FilaPrioridade *) malloc(sizeof(FilaPrioridade));
    if (fila == NULL) return NULL;

    fila->itens = (NoHuffman **) malloc((size_t) capacidade * sizeof(NoHuffman *));
    if (fila->itens == NULL) {
        free(fila);
        return NULL;
    }

    fila->tamanho = 0;
    fila->capacidade = capacidade;
    return fila;
}

// Libera eventuais nos restantes, o vetor do heap e a struct da fila
void liberar_fila_prioridade(FilaPrioridade *fila) {
    if (fila == NULL) return;

    for (int i = 0; i < fila->tamanho; i++)
        liberar_no_huffman(fila->itens[i]);

    free(fila->itens);
    free(fila);
}

int fila_prioridade_esta_vazia(const FilaPrioridade *fila) {
    return (fila == NULL || fila->tamanho == 0);
}

int tamanho_fila_prioridade(const FilaPrioridade *fila) {
    return (fila != NULL) ? fila->tamanho : 0;
}

// Insere um no ao final do vetor e sobe ate restaurar a propriedade do min-heap
int inserir_fila_prioridade(FilaPrioridade *fila, NoHuffman *no) {
    if (fila == NULL || no == NULL || fila->tamanho >= fila->capacidade)
        return 0;

    fila->itens[fila->tamanho] = no;
    subir_heap(fila, fila->tamanho);
    fila->tamanho++;
    return 1;
}

// Remove o menor no (raiz), move o ultimo item para o topo e reorganiza o heap
NoHuffman *remover_menor_fila_prioridade(FilaPrioridade *fila) {
    if (fila == NULL || fila->tamanho == 0) return NULL;

    NoHuffman *menor = fila->itens[0];
    fila->tamanho--;

    if (fila->tamanho > 0) {
        fila->itens[0] = fila->itens[fila->tamanho];
        descer_heap(fila, 0);
    }

    return menor;
}
