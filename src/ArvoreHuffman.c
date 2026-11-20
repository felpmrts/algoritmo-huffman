#include <stdio.h>
#include <stdlib.h>
#include "ArvoreHuffman.h"

/*
 * Definicao interna do no da Arvore de Huffman.
 * Apenas este arquivo (.c) tem acesso direto aos campos da struct.
 */
struct NoHuffman {
    unsigned char simbolo;
    unsigned long frequencia;
    struct NoHuffman *esquerda;
    struct NoHuffman *direita;
};

/*
 * Definicao interna do TAD ArvoreHuffman.
 * Guarda a referencia para o no raiz da arvore.
 */
struct ArvoreHuffman {
    NoHuffman *raiz;
};

NoHuffman *criar_no_folha(unsigned char simbolo, unsigned long frequencia) {
    NoHuffman *novo = (NoHuffman *) malloc(sizeof(NoHuffman));
    if (novo == NULL) {
        return NULL;
    }

    novo->simbolo = simbolo;
    novo->frequencia = frequencia;
    novo->esquerda = NULL;
    novo->direita = NULL;

    return novo;
}

NoHuffman *criar_no_interno(NoHuffman *esquerda, NoHuffman *direita) {
    if (esquerda == NULL || direita == NULL) {
        return NULL;
    }

    NoHuffman *novo = (NoHuffman *) malloc(sizeof(NoHuffman));
    if (novo == NULL) {
        return NULL;
    }

    /* Nos internos nao representam um caractere especifico, apenas combinam frequencias */
    novo->simbolo = 0;
    novo->frequencia = esquerda->frequencia + direita->frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;

    return novo;
}

void liberar_no_huffman(NoHuffman *no) {
    if (no == NULL) {
        return;
    }

    /* Libera primeiro os filhos e depois o proprio no (pos-ordem) */
    liberar_no_huffman(no->esquerda);
    liberar_no_huffman(no->direita);
    free(no);
}

int no_eh_folha(const NoHuffman *no) {
    if (no == NULL) {
        return 0;
    }
    return (no->esquerda == NULL && no->direita == NULL);
}

unsigned char obter_simbolo_no(const NoHuffman *no) {
    if (no == NULL) {
        return 0;
    }
    return no->simbolo;
}

unsigned long obter_frequencia_no(const NoHuffman *no) {
    if (no == NULL) {
        return 0;
    }
    return no->frequencia;
}

NoHuffman *obter_esquerda_no(const NoHuffman *no) {
    if (no == NULL) {
        return NULL;
    }
    return no->esquerda;
}

NoHuffman *obter_direita_no(const NoHuffman *no) {
    if (no == NULL) {
        return NULL;
    }
    return no->direita;
}

ArvoreHuffman *criar_arvore_huffman(NoHuffman *raiz) {
    ArvoreHuffman *arvore = (ArvoreHuffman *) malloc(sizeof(ArvoreHuffman));
    if (arvore == NULL) {
        return NULL;
    }

    arvore->raiz = raiz;
    return arvore;
}

void liberar_arvore_huffman(ArvoreHuffman *arvore) {
    if (arvore == NULL) {
        return;
    }

    liberar_no_huffman(arvore->raiz);
    free(arvore);
}

int arvore_huffman_esta_vazia(const ArvoreHuffman *arvore) {
    if (arvore == NULL || arvore->raiz == NULL) {
        return 1;
    }
    return 0;
}

NoHuffman *obter_raiz_arvore_huffman(const ArvoreHuffman *arvore) {
    if (arvore == NULL) {
        return NULL;
    }
    return arvore->raiz;
}

/* Funcao auxiliar recursiva para calcular a altura de uma subarvore */
static int altura_subarvore(const NoHuffman *no) {
    if (no == NULL) {
        return 0;
    }

    int alt_esq = altura_subarvore(no->esquerda);
    int alt_dir = altura_subarvore(no->direita);

    if (alt_esq > alt_dir) {
        return alt_esq + 1;
    }
    return alt_dir + 1;
}

int altura_arvore_huffman(const ArvoreHuffman *arvore) {
    if (arvore == NULL) {
        return 0;
    }
    return altura_subarvore(arvore->raiz);
}

/* Funcao auxiliar recursiva para contar o total de nos de uma subarvore */
static int contar_nos_subarvore(const NoHuffman *no) {
    if (no == NULL) {
        return 0;
    }

    int nos_esq = contar_nos_subarvore(no->esquerda);
    int nos_dir = contar_nos_subarvore(no->direita);

    return nos_esq + nos_dir + 1;
}

int total_nos_arvore_huffman(const ArvoreHuffman *arvore) {
    if (arvore == NULL) {
        return 0;
    }
    return contar_nos_subarvore(arvore->raiz);
}

/* Funcao auxiliar recursiva para imprimir a arvore em pre-ordem com recuo por nivel */
static void imprimir_subarvore_pre_ordem(const NoHuffman *no, int nivel) {
    if (no == NULL) {
        return;
    }

    int i;
    for (i = 0; i < nivel; i++) {
        printf("  ");
    }

    if (no_eh_folha(no)) {
        if (no->simbolo >= 32 && no->simbolo <= 126) {
            printf("[Folha '%c' (ASCII %u) | freq = %lu]\n",
                   no->simbolo, (unsigned int) no->simbolo, no->frequencia);
        } else {
            printf("[Folha 0x%02X | freq = %lu]\n",
                   (unsigned int) no->simbolo, no->frequencia);
        }
    } else {
        printf("[Interno | freq = %lu]\n", no->frequencia);
    }

    imprimir_subarvore_pre_ordem(no->esquerda, nivel + 1);
    imprimir_subarvore_pre_ordem(no->direita, nivel + 1);
}

void imprimir_arvore_huffman(const ArvoreHuffman *arvore) {
    if (arvore_huffman_esta_vazia(arvore)) {
        printf("(Arvore de Huffman vazia)\n");
        return;
    }

    imprimir_subarvore_pre_ordem(arvore->raiz, 0);
}
