#ifndef ARVORE_HUFFMAN_H
#define ARVORE_HUFFMAN_H

/*
 * ============================================================================
 * TAD Opaco: ArvoreHuffman
 * ----------------------------------------------------------------------------
 * Define a estrutura de arvore binaria utilizada pelo algoritmo de Huffman.
 * Os campos internos dos nos e da arvore ficam ocultos em ArvoreHuffman.c,
 * permitindo acesso apenas por meio das funcoes declaradas neste cabecalho.
 * ============================================================================
 */

/* Tipos opacos: a definicao completa das structs existe apenas no arquivo .c */
typedef struct NoHuffman NoHuffman;
typedef struct ArvoreHuffman ArvoreHuffman;

/* --- Operacoes sobre nos da arvore --- */

/* Cria um no folha contendo um simbolo (byte) e sua frequencia no texto. */
NoHuffman *criar_no_folha(unsigned char simbolo, unsigned long frequencia);

/* Cria um no interno unindo dois nos filhos; sua frequencia e a soma deles. */
NoHuffman *criar_no_interno(NoHuffman *esquerda, NoHuffman *direita);

/* Libera recursivamente um no e toda a sua subarvore (percurso pos-ordem). */
void liberar_no_huffman(NoHuffman *no);

/* Retorna 1 se o no for folha (sem filhos), ou 0 caso contrario. */
int no_eh_folha(const NoHuffman *no);

/* Retorna o simbolo armazenado em um no folha. */
unsigned char obter_simbolo_no(const NoHuffman *no);

/* Retorna a frequencia associada ao no. */
unsigned long obter_frequencia_no(const NoHuffman *no);

/* Retorna o ponteiro para o filho a esquerda do no (ou NULL). */
NoHuffman *obter_esquerda_no(const NoHuffman *no);

/* Retorna o ponteiro para o filho a direita do no (ou NULL). */
NoHuffman *obter_direita_no(const NoHuffman *no);

/* --- Operacoes sobre a arvore de Huffman --- */

/* Cria a arvore de Huffman a partir de um no raiz (pode ser NULL se vazia). */
ArvoreHuffman *criar_arvore_huffman(NoHuffman *raiz);

/* Libera toda a memoria ocupada pela arvore e por todos os seus nos. */
void liberar_arvore_huffman(ArvoreHuffman *arvore);

/* Retorna 1 se a arvore estiver vazia, ou 0 caso contrario. */
int arvore_huffman_esta_vazia(const ArvoreHuffman *arvore);

/* Retorna o ponteiro para o no raiz da arvore (ou NULL se vazia). */
NoHuffman *obter_raiz_arvore_huffman(const ArvoreHuffman *arvore);

/* Retorna a altura da arvore de Huffman (0 para arvore vazia). */
int altura_arvore_huffman(const ArvoreHuffman *arvore);

/* Retorna a quantidade total de nos (folhas + internos) da arvore. */
int total_nos_arvore_huffman(const ArvoreHuffman *arvore);

/* Imprime a estrutura da arvore em pre-ordem para fins didaticos/depuracao. */
void imprimir_arvore_huffman(const ArvoreHuffman *arvore);

#endif /* ARVORE_HUFFMAN_H */
