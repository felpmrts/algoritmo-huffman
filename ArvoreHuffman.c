#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ArvoreHuffman.h"

#define TOTAL_SIMBOLOS_ARVORE 256
#define TAMANHO_MAX_CODIGO 257

// menor_simbolo guarda o menor byte da subarvore para desempate deterministico no heap
struct NoHuffman {
    unsigned char simbolo;
    unsigned char menor_simbolo;
    unsigned long frequencia;
    struct NoHuffman *esquerda;
    struct NoHuffman *direita;
};

struct ArvoreHuffman {
    NoHuffman *raiz;
};

NoHuffman *criar_no_folha(unsigned char simbolo, unsigned long frequencia) {
    NoHuffman *novo = (NoHuffman *) malloc(sizeof(NoHuffman));
    if (novo == NULL) return NULL;

    novo->simbolo = simbolo;
    novo->menor_simbolo = simbolo;
    novo->frequencia = frequencia;
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

// Cria um no interno unindo dois filhos (frequencia = soma dos filhos)
NoHuffman *criar_no_interno(NoHuffman *esquerda, NoHuffman *direita) {
    if (esquerda == NULL || direita == NULL) return NULL;

    NoHuffman *novo = (NoHuffman *) malloc(sizeof(NoHuffman));
    if (novo == NULL) return NULL;

    novo->simbolo = 0;
    novo->menor_simbolo = (esquerda->menor_simbolo < direita->menor_simbolo)
                              ? esquerda->menor_simbolo
                              : direita->menor_simbolo;
    novo->frequencia = esquerda->frequencia + direita->frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}

// Libera a subarvore em pos-ordem (filhos antes do pai)
void liberar_no_huffman(NoHuffman *no) {
    if (no == NULL) return;
    liberar_no_huffman(no->esquerda);
    liberar_no_huffman(no->direita);
    free(no);
}

int no_eh_folha(const NoHuffman *no) {
    return (no != NULL && no->esquerda == NULL && no->direita == NULL);
}

unsigned char obter_simbolo_no(const NoHuffman *no) {
    return (no != NULL) ? no->simbolo : 0;
}

unsigned long obter_frequencia_no(const NoHuffman *no) {
    return (no != NULL) ? no->frequencia : 0;
}

NoHuffman *obter_esquerda_no(const NoHuffman *no) {
    return (no != NULL) ? no->esquerda : NULL;
}

NoHuffman *obter_direita_no(const NoHuffman *no) {
    return (no != NULL) ? no->direita : NULL;
}

// Criterio do min-heap: 1) menor frequencia; 2) folha antes de interno; 3) menor_simbolo
int comparar_nos_huffman(const NoHuffman *a, const NoHuffman *b) {
    if (a == b) return 0;
    if (a == NULL) return 1;
    if (b == NULL) return -1;

    if (a->frequencia != b->frequencia)
        return (a->frequencia < b->frequencia) ? -1 : 1;

    int folha_a = no_eh_folha(a);
    int folha_b = no_eh_folha(b);
    if (folha_a != folha_b)
        return folha_a ? -1 : 1;

    if (a->menor_simbolo != b->menor_simbolo)
        return (a->menor_simbolo < b->menor_simbolo) ? -1 : 1;

    return 0;
}

ArvoreHuffman *criar_arvore_huffman(NoHuffman *raiz) {
    ArvoreHuffman *arvore = (ArvoreHuffman *) malloc(sizeof(ArvoreHuffman));
    if (arvore == NULL) return NULL;
    arvore->raiz = raiz;
    return arvore;
}

void liberar_arvore_huffman(ArvoreHuffman *arvore) {
    if (arvore == NULL) return;
    liberar_no_huffman(arvore->raiz);
    free(arvore);
}

int arvore_huffman_esta_vazia(const ArvoreHuffman *arvore) {
    return (arvore == NULL || arvore->raiz == NULL);
}

NoHuffman *obter_raiz_arvore_huffman(const ArvoreHuffman *arvore) {
    return (arvore != NULL) ? arvore->raiz : NULL;
}

static int altura_subarvore(const NoHuffman *no) {
    if (no == NULL) return 0;
    int esq = altura_subarvore(no->esquerda);
    int dir = altura_subarvore(no->direita);
    return (esq > dir) ? (esq + 1) : (dir + 1);
}

int altura_arvore_huffman(const ArvoreHuffman *arvore) {
    return (arvore != NULL) ? altura_subarvore(arvore->raiz) : 0;
}

static int contar_nos_subarvore(const NoHuffman *no) {
    if (no == NULL) return 0;
    return contar_nos_subarvore(no->esquerda) + contar_nos_subarvore(no->direita) + 1;
}

int total_nos_arvore_huffman(const ArvoreHuffman *arvore) {
    return (arvore != NULL) ? contar_nos_subarvore(arvore->raiz) : 0;
}

// Percurso recursivo: esquerda = '0', direita = '1'; salva o codigo ao achar uma folha
static int gerar_codigos_subarvore(const NoHuffman *no,
                                   char *tabela_codigos[TOTAL_SIMBOLOS_ARVORE],
                                   char *buffer,
                                   int profundidade) {
    if (no == NULL) return 1;

    if (no_eh_folha(no)) {
        buffer[profundidade] = '\0';
        char *codigo = (char *) malloc((size_t) (profundidade + 1));
        if (codigo == NULL) return 0;
        strcpy(codigo, buffer);
        tabela_codigos[no->simbolo] = codigo;
        return 1;
    }

    if (profundidade >= TAMANHO_MAX_CODIGO - 1) return 0;

    buffer[profundidade] = '0';
    if (!gerar_codigos_subarvore(no->esquerda, tabela_codigos, buffer, profundidade + 1))
        return 0;

    buffer[profundidade] = '1';
    if (!gerar_codigos_subarvore(no->direita, tabela_codigos, buffer, profundidade + 1))
        return 0;

    return 1;
}

// Gera os codigos binarios a partir da raiz (trata arquivo com 1 unico simbolo)
int gerar_codigos_arvore_huffman(const ArvoreHuffman *arvore,
                                 char *tabela_codigos[TOTAL_SIMBOLOS_ARVORE]) {
    if (tabela_codigos == NULL) return 0;
    if (arvore_huffman_esta_vazia(arvore)) return 1;

    const NoHuffman *raiz = arvore->raiz;

    // Caso especial: arquivo com apenas um caractere distinto recebe codigo "0"
    if (no_eh_folha(raiz)) {
        char *codigo = (char *) malloc(2);
        if (codigo == NULL) return 0;
        strcpy(codigo, "0");
        tabela_codigos[raiz->simbolo] = codigo;
        return 1;
    }

    char buffer[TAMANHO_MAX_CODIGO];
    if (!gerar_codigos_subarvore(raiz, tabela_codigos, buffer, 0)) {
        for (int i = 0; i < TOTAL_SIMBOLOS_ARVORE; i++) {
            free(tabela_codigos[i]);
            tabela_codigos[i] = NULL;
        }
        return 0;
    }

    return 1;
}

static void imprimir_subarvore_pre_ordem(const NoHuffman *no, int nivel) {
    if (no == NULL) return;

    for (int i = 0; i < nivel; i++) printf("  ");

    if (no_eh_folha(no)) {
        if (no->simbolo >= 32 && no->simbolo <= 126)
            printf("[Folha '%c' | freq=%lu]\n", no->simbolo, no->frequencia);
        else
            printf("[Folha 0x%02X | freq=%lu]\n", (unsigned int) no->simbolo, no->frequencia);
    } else {
        printf("[Interno | freq=%lu]\n", no->frequencia);
    }

    imprimir_subarvore_pre_ordem(no->esquerda, nivel + 1);
    imprimir_subarvore_pre_ordem(no->direita, nivel + 1);
}

void imprimir_arvore_huffman(const ArvoreHuffman *arvore) {
    if (arvore_huffman_esta_vazia(arvore)) {
        printf("(Arvore vazia)\n");
        return;
    }
    imprimir_subarvore_pre_ordem(arvore->raiz, 0);
}
