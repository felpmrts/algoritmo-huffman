#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

#include "ArvoreHuffman.h"

/*
 * TAD Opaco: FilaPrioridade
 * Fila de prioridade minima (Min-Heap binario) para selecao dos nos
 * de menor frequencia durante a construcao da Arvore de Huffman.
 */

typedef struct FilaPrioridade FilaPrioridade;

FilaPrioridade *criar_fila_prioridade(int capacidade);
void liberar_fila_prioridade(FilaPrioridade *fila);
int fila_prioridade_esta_vazia(const FilaPrioridade *fila);
int tamanho_fila_prioridade(const FilaPrioridade *fila);
int inserir_fila_prioridade(FilaPrioridade *fila, NoHuffman *no);
NoHuffman *remover_menor_fila_prioridade(FilaPrioridade *fila);

#endif
