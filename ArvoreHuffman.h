#ifndef ARVORE_HUFFMAN_H
#define ARVORE_HUFFMAN_H

// TAD Opaco: ArvoreHuffman (structs definidas apenas em ArvoreHuffman.c)
typedef struct NoHuffman NoHuffman;
typedef struct ArvoreHuffman ArvoreHuffman;

// Criacao, liberacao e comparacao de nos
NoHuffman *criar_no_folha(unsigned char simbolo, unsigned long frequencia);
NoHuffman *criar_no_interno(NoHuffman *esquerda, NoHuffman *direita);
void liberar_no_huffman(NoHuffman *no);
int comparar_nos_huffman(const NoHuffman *a, const NoHuffman *b);

// Consultas aos campos do no sem expor a struct
int no_eh_folha(const NoHuffman *no);
unsigned char obter_simbolo_no(const NoHuffman *no);
unsigned long obter_frequencia_no(const NoHuffman *no);
NoHuffman *obter_esquerda_no(const NoHuffman *no);
NoHuffman *obter_direita_no(const NoHuffman *no);

// Operacoes da arvore de Huffman
ArvoreHuffman *criar_arvore_huffman(NoHuffman *raiz);
void liberar_arvore_huffman(ArvoreHuffman *arvore);
int arvore_huffman_esta_vazia(const ArvoreHuffman *arvore);
NoHuffman *obter_raiz_arvore_huffman(const ArvoreHuffman *arvore);
int altura_arvore_huffman(const ArvoreHuffman *arvore);
int total_nos_arvore_huffman(const ArvoreHuffman *arvore);

// Percorre a arvore montando os codigos binarios ('0' esq, '1' dir) em tabela_codigos
int gerar_codigos_arvore_huffman(const ArvoreHuffman *arvore, char *tabela_codigos[256]);
void imprimir_arvore_huffman(const ArvoreHuffman *arvore);

#endif
