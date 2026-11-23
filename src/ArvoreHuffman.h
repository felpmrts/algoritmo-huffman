#ifndef ARVORE_HUFFMAN_H
#define ARVORE_HUFFMAN_H

/*
 * TAD Opaco: ArvoreHuffman
 * Estrutura de arvore binaria para o algoritmo de Huffman, adaptada a partir
 * da implementacao de referencia de ArvoreBinaria da disciplina.
 */

typedef struct NoHuffman NoHuffman;
typedef struct ArvoreHuffman ArvoreHuffman;

/* Operacoes sobre nos */
NoHuffman *criar_no_folha(unsigned char simbolo, unsigned long frequencia);
NoHuffman *criar_no_interno(NoHuffman *esquerda, NoHuffman *direita);
void liberar_no_huffman(NoHuffman *no);

int no_eh_folha(const NoHuffman *no);
unsigned char obter_simbolo_no(const NoHuffman *no);
unsigned long obter_frequencia_no(const NoHuffman *no);
NoHuffman *obter_esquerda_no(const NoHuffman *no);
NoHuffman *obter_direita_no(const NoHuffman *no);
int comparar_nos_huffman(const NoHuffman *a, const NoHuffman *b);

/* Operacoes sobre a arvore */
ArvoreHuffman *criar_arvore_huffman(NoHuffman *raiz);
void liberar_arvore_huffman(ArvoreHuffman *arvore);
int arvore_huffman_esta_vazia(const ArvoreHuffman *arvore);
NoHuffman *obter_raiz_arvore_huffman(const ArvoreHuffman *arvore);
int altura_arvore_huffman(const ArvoreHuffman *arvore);
int total_nos_arvore_huffman(const ArvoreHuffman *arvore);
int gerar_codigos_arvore_huffman(const ArvoreHuffman *arvore, char *tabela_codigos[256]);
void imprimir_arvore_huffman(const ArvoreHuffman *arvore);

#endif
