#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

#include "ArvoreHuffman.h"

/*
 * ============================================================================
 * TAD Opaco: FilaPrioridade (Min-Heap)
 * ----------------------------------------------------------------------------
 * Fila de prioridade minima utilizada para construir a Arvore de Huffman.
 * Mantem sempre na raiz do heap (indice 0) o no com menor frequencia,
 * permitindo inserir e remover o menor elemento em tempo O(log n).
 * ============================================================================
 */

/* Tipo opaco: os campos internos existem apenas em FilaPrioridade.c */
typedef struct FilaPrioridade FilaPrioridade;

/* Cria uma fila de prioridade vazia com a capacidade maxima informada. */
FilaPrioridade *criar_fila_prioridade(int capacidade);

/*
 * Libera a memoria da fila de prioridade.
 * Caso ainda existam nos nao removidos na fila (por exemplo, em caso de erro),
 * esses nos tambem sao liberados para evitar vazamento de memoria.
 */
void liberar_fila_prioridade(FilaPrioridade *fila);

/* Retorna 1 se a fila estiver vazia, ou 0 caso contrario. */
int fila_prioridade_esta_vazia(const FilaPrioridade *fila);

/* Retorna a quantidade atual de nos armazenados na fila. */
int tamanho_fila_prioridade(const FilaPrioridade *fila);

/*
 * Insere um no da Arvore de Huffman na fila mantendo a propriedade de min-heap.
 * Retorna 1 em caso de sucesso, ou 0 se a fila for invalida ou estiver cheia.
 */
int inserir_fila_prioridade(FilaPrioridade *fila, NoHuffman *no);

/*
 * Remove e retorna o no com menor frequencia da fila.
 * Retorna NULL se a fila for invalida ou estiver vazia.
 */
NoHuffman *remover_menor_fila_prioridade(FilaPrioridade *fila);

#endif /* FILA_PRIORIDADE_H */
