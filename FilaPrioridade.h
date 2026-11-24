#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

#include "ArvoreHuffman.h"

// TAD Opaco: FilaPrioridade (Min-Heap em vetor definido em FilaPrioridade.c)
typedef struct FilaPrioridade FilaPrioridade;

// Criacao e liberacao (libera eventuais nos restantes em caso de erro)
FilaPrioridade *criar_fila_prioridade(int capacidade);
void liberar_fila_prioridade(FilaPrioridade *fila);

// Consultas basicas
int fila_prioridade_esta_vazia(const FilaPrioridade *fila);
int tamanho_fila_prioridade(const FilaPrioridade *fila);

// Insere mantendo a propriedade de min-heap e remove o no de menor prioridade
int inserir_fila_prioridade(FilaPrioridade *fila, NoHuffman *no);
NoHuffman *remover_menor_fila_prioridade(FilaPrioridade *fila);

#endif
