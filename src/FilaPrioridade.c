#include <stdlib.h>
#include "FilaPrioridade.h"

/*
 * Definicao interna da Fila de Prioridade (Min-Heap binario em vetor).
 * Para um elemento no indice i:
 *   - Pai:             (i - 1) / 2
 *   - Filho esquerda:  2 * i + 1
 *   - Filho direita:   2 * i + 2
 */
struct FilaPrioridade {
    NoHuffman **itens;
    int tamanho;
    int capacidade;
};

/* Troca dois ponteiros de no dentro do vetor do heap */
static void trocar_nos(NoHuffman **a, NoHuffman **b) {
    NoHuffman *temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * Restaura a propriedade de min-heap subindo o elemento da posicao 'indice'
 * enquanto sua frequencia for menor que a frequencia do seu pai.
 */
static void subir_heap(FilaPrioridade *fila, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;

        if (obter_frequencia_no(fila->itens[indice]) < obter_frequencia_no(fila->itens[pai])) {
            trocar_nos(&fila->itens[indice], &fila->itens[pai]);
            indice = pai;
        } else {
            break;
        }
    }
}

/*
 * Restaura a propriedade de min-heap descendo o elemento da posicao 'indice'
 * trocando-o sempre com o filho de menor frequencia.
 */
static void descer_heap(FilaPrioridade *fila, int indice) {
    while (1) {
        int esq = 2 * indice + 1;
        int dir = 2 * indice + 2;
        int menor = indice;

        if (esq < fila->tamanho &&
            obter_frequencia_no(fila->itens[esq]) < obter_frequencia_no(fila->itens[menor])) {
            menor = esq;
        }

        if (dir < fila->tamanho &&
            obter_frequencia_no(fila->itens[dir]) < obter_frequencia_no(fila->itens[menor])) {
            menor = dir;
        }

        if (menor != indice) {
            trocar_nos(&fila->itens[indice], &fila->itens[menor]);
            indice = menor;
        } else {
            break;
        }
    }
}

FilaPrioridade *criar_fila_prioridade(int capacidade) {
    if (capacidade <= 0) {
        return NULL;
    }

    FilaPrioridade *fila = (FilaPrioridade *) malloc(sizeof(FilaPrioridade));
    if (fila == NULL) {
        return NULL;
    }

    fila->itens = (NoHuffman **) malloc((size_t) capacidade * sizeof(NoHuffman *));
    if (fila->itens == NULL) {
        free(fila);
        return NULL;
    }

    fila->tamanho = 0;
    fila->capacidade = capacidade;
    return fila;
}

void liberar_fila_prioridade(FilaPrioridade *fila) {
    if (fila == NULL) {
        return;
    }

    /* Se ainda restou algum no na fila, libera sua subarvore por seguranca */
    int i;
    for (i = 0; i < fila->tamanho; i++) {
        liberar_no_huffman(fila->itens[i]);
    }

    free(fila->itens);
    free(fila);
}

int fila_prioridade_esta_vazia(const FilaPrioridade *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        return 1;
    }
    return 0;
}

int tamanho_fila_prioridade(const FilaPrioridade *fila) {
    if (fila == NULL) {
        return 0;
    }
    return fila->tamanho;
}

int inserir_fila_prioridade(FilaPrioridade *fila, NoHuffman *no) {
    if (fila == NULL || no == NULL) {
        return 0;
    }

    if (fila->tamanho >= fila->capacidade) {
        return 0;
    }

    /* Insere no final do vetor e sobe ate a posicao correta */
    fila->itens[fila->tamanho] = no;
    subir_heap(fila, fila->tamanho);
    fila->tamanho++;

    return 1;
}

NoHuffman *remover_menor_fila_prioridade(FilaPrioridade *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        return NULL;
    }

    /* O menor elemento esta sempre na raiz do heap (indice 0) */
    NoHuffman *menor = fila->itens[0];
    fila->tamanho--;

    /* Move o ultimo elemento para a raiz e desce ate restaurar o min-heap */
    if (fila->tamanho > 0) {
        fila->itens[0] = fila->itens[fila->tamanho];
        descer_heap(fila, 0);
    }

    return menor;
}
