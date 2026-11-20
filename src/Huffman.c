#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Huffman.h"
#include "ArvoreHuffman.h"
#include "FilaPrioridade.h"

/* Constantes internas do algoritmo */
#define TOTAL_SIMBOLOS 256
#define TAMANHO_MAX_CODIGO 257
#define ASSINATURA_HUFFMAN "HUF1"
#define TAMANHO_ASSINATURA 4

/*
 * Definicao interna do TAD opaco Huffman.
 * Nenhum outro arquivo pode acessar diretamente os campos abaixo.
 */
struct Huffman {
    unsigned long frequencias[TOTAL_SIMBOLOS];
    ArvoreHuffman *arvore;
    char *tabela_codigos[TOTAL_SIMBOLOS];

    /* Estatisticas da ultima operacao realizada */
    unsigned long tamanho_original;
    unsigned long bytes_cabecalho;
    unsigned long bytes_dados_codificados;
    unsigned long tamanho_compactado;
    int simbolos_distintos;
};

/* ============================================================================
 * Funcoes auxiliares internas (privadas ao modulo Huffman.c)
 * ============================================================================
 */

/* Libera arvore e tabela de codigos anteriores e zera o estado interno */
static void limpar_estado_huffman(Huffman *huffman) {
    int i;

    if (huffman->arvore != NULL) {
        liberar_arvore_huffman(huffman->arvore);
        huffman->arvore = NULL;
    }

    for (i = 0; i < TOTAL_SIMBOLOS; i++) {
        huffman->frequencias[i] = 0;
        if (huffman->tabela_codigos[i] != NULL) {
            free(huffman->tabela_codigos[i]);
            huffman->tabela_codigos[i] = NULL;
        }
    }

    huffman->tamanho_original = 0;
    huffman->bytes_cabecalho = 0;
    huffman->bytes_dados_codificados = 0;
    huffman->tamanho_compactado = 0;
    huffman->simbolos_distintos = 0;
}

/*
 * Etapa 1 do algoritmo: percorre o arquivo de entrada contando a frequencia
 * de ocorrencia de cada caractere (byte 0..255).
 *
 * Abrimos em modo binario ("rb") para ler cada byte do arquivo de texto
 * exatamente como esta armazenado, preservando quebras de linha e acentos
 * UTF-8 em qualquer sistema operacional.
 */
static int contar_frequencias_arquivo(Huffman *huffman, const char *caminho_entrada) {
    FILE *entrada = fopen(caminho_entrada, "rb");
    if (entrada == NULL) {
        return 0;
    }

    int caractere;
    while ((caractere = fgetc(entrada)) != EOF) {
        unsigned char simbolo = (unsigned char) caractere;
        if (huffman->frequencias[simbolo] == 0) {
            huffman->simbolos_distintos++;
        }
        huffman->frequencias[simbolo]++;
        huffman->tamanho_original++;
    }

    int erro_leitura = ferror(entrada);
    fclose(entrada);

    return !erro_leitura;
}

/*
 * Etapa 2 do algoritmo: constroi a Arvore de Huffman usando a Fila de Prioridade.
 *
 * Passos:
 *   1. Cria um no folha para cada simbolo com frequencia > 0 e insere na fila;
 *   2. Enquanto houver mais de um no na fila, remove os dois de menor frequencia,
 *      combina-os em um novo no interno e reinsere o no interno na fila;
 *   3. O unico no restante e a raiz da Arvore de Huffman.
 *
 * Observacao sobre determinismo: como os simbolos sao sempre inseridos em ordem
 * crescente (0..255), a reconstrucao da arvore na descompactacao produz
 * exatamente a mesma arvore gerada na compactacao.
 */
static int construir_arvore_huffman(Huffman *huffman) {
    if (huffman->simbolos_distintos == 0) {
        huffman->arvore = criar_arvore_huffman(NULL);
        return (huffman->arvore != NULL);
    }

    FilaPrioridade *fila = criar_fila_prioridade(TOTAL_SIMBOLOS);
    if (fila == NULL) {
        return 0;
    }

    int i;
    for (i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0) {
            NoHuffman *folha = criar_no_folha((unsigned char) i, huffman->frequencias[i]);
            if (folha == NULL || !inserir_fila_prioridade(fila, folha)) {
                liberar_no_huffman(folha);
                liberar_fila_prioridade(fila);
                return 0;
            }
        }
    }

    /* Combina iterativamente os dois nos de menor frequencia */
    while (tamanho_fila_prioridade(fila) > 1) {
        NoHuffman *menor1 = remover_menor_fila_prioridade(fila);
        NoHuffman *menor2 = remover_menor_fila_prioridade(fila);

        NoHuffman *pai = criar_no_interno(menor1, menor2);
        if (pai == NULL) {
            liberar_no_huffman(menor1);
            liberar_no_huffman(menor2);
            liberar_fila_prioridade(fila);
            return 0;
        }

        if (!inserir_fila_prioridade(fila, pai)) {
            liberar_no_huffman(pai);
            liberar_fila_prioridade(fila);
            return 0;
        }
    }

    NoHuffman *raiz = remover_menor_fila_prioridade(fila);
    liberar_fila_prioridade(fila);

    huffman->arvore = criar_arvore_huffman(raiz);
    if (huffman->arvore == NULL) {
        liberar_no_huffman(raiz);
        return 0;
    }

    return 1;
}

/* Percorre a arvore recursivamente montando o codigo de cada folha ('0' esq, '1' dir) */
static int gerar_codigos_recursivo(Huffman *huffman,
                                   const NoHuffman *no,
                                   char *buffer,
                                   int profundidade) {
    if (no == NULL) {
        return 1;
    }

    if (no_eh_folha(no)) {
        buffer[profundidade] = '\0';

        unsigned char simbolo = obter_simbolo_no(no);
        char *codigo = (char *) malloc((size_t) (profundidade + 1));
        if (codigo == NULL) {
            return 0;
        }

        strcpy(codigo, buffer);
        huffman->tabela_codigos[simbolo] = codigo;
        return 1;
    }

    /* Ramo esquerdo acrescenta o bit '0' */
    buffer[profundidade] = '0';
    if (!gerar_codigos_recursivo(huffman, obter_esquerda_no(no), buffer, profundidade + 1)) {
        return 0;
    }

    /* Ramo direito acrescenta o bit '1' */
    buffer[profundidade] = '1';
    if (!gerar_codigos_recursivo(huffman, obter_direita_no(no), buffer, profundidade + 1)) {
        return 0;
    }

    return 1;
}

/*
 * Etapa 3 do algoritmo: gera a tabela de codigos binarios a partir da arvore.
 *
 * Caso especial tratado: se o arquivo possui apenas 1 simbolo distinto
 * (ex.: "aaaaa"), a raiz ja e uma folha. Nesse caso, atribuimos o codigo "0"
 * para que cada ocorrencia seja representada por 1 bit.
 */
static int gerar_tabela_codigos(Huffman *huffman) {
    if (arvore_huffman_esta_vazia(huffman->arvore)) {
        return 1;
    }

    const NoHuffman *raiz = obter_raiz_arvore_huffman(huffman->arvore);

    /* Tratamento do caso especial de arquivo com um unico simbolo distinto */
    if (no_eh_folha(raiz)) {
        unsigned char simbolo = obter_simbolo_no(raiz);
        char *codigo = (char *) malloc(2);
        if (codigo == NULL) {
            return 0;
        }
        codigo[0] = '0';
        codigo[1] = '\0';
        huffman->tabela_codigos[simbolo] = codigo;
        return 1;
    }

    char buffer[TAMANHO_MAX_CODIGO];
    return gerar_codigos_recursivo(huffman, raiz, buffer, 0);
}

/*
 * Grava o cabecalho do arquivo compactado (.huff):
 *   - Assinatura magica "HUF1"            (4 bytes)
 *   - Total de bytes do texto original    (8 bytes - uint64_t)
 *   - Quantidade de simbolos distintos    (2 bytes - uint16_t)
 *   - Para cada simbolo presente (0..255):
 *       * simbolo                         (1 byte  - uint8_t)
 *       * frequencia                      (4 bytes - uint32_t)
 */
static int escrever_cabecalho(const Huffman *huffman,
                              FILE *saida,
                              unsigned long *bytes_escritos) {
    uint64_t total_original = (uint64_t) huffman->tamanho_original;
    uint16_t qtd_simbolos = (uint16_t) huffman->simbolos_distintos;
    unsigned long contador = 0;

    if (fwrite(ASSINATURA_HUFFMAN, 1, TAMANHO_ASSINATURA, saida) != TAMANHO_ASSINATURA) {
        return 0;
    }
    contador += TAMANHO_ASSINATURA;

    if (fwrite(&total_original, sizeof(uint64_t), 1, saida) != 1) {
        return 0;
    }
    contador += sizeof(uint64_t);

    if (fwrite(&qtd_simbolos, sizeof(uint16_t), 1, saida) != 1) {
        return 0;
    }
    contador += sizeof(uint16_t);

    int i;
    for (i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0) {
            if (huffman->frequencias[i] > UINT32_MAX) {
                return 0;
            }
            uint8_t simbolo = (uint8_t) i;
            uint32_t freq = (uint32_t) huffman->frequencias[i];

            if (fwrite(&simbolo, sizeof(uint8_t), 1, saida) != 1 ||
                fwrite(&freq, sizeof(uint32_t), 1, saida) != 1) {
                return 0;
            }
            contador += sizeof(uint8_t) + sizeof(uint32_t);
        }
    }

    *bytes_escritos = contador;
    return 1;
}

/* Le e valida o cabecalho de um arquivo compactado (.huff) */
static int ler_cabecalho(Huffman *huffman, FILE *entrada, unsigned long *bytes_lidos) {
    char assinatura[TAMANHO_ASSINATURA];
    uint64_t total_original = 0;
    uint16_t qtd_simbolos = 0;
    unsigned long contador = 0;

    if (fread(assinatura, 1, TAMANHO_ASSINATURA, entrada) != TAMANHO_ASSINATURA) {
        return 0;
    }
    if (memcmp(assinatura, ASSINATURA_HUFFMAN, TAMANHO_ASSINATURA) != 0) {
        return 0; /* Arquivo nao possui a assinatura esperada */
    }
    contador += TAMANHO_ASSINATURA;

    if (fread(&total_original, sizeof(uint64_t), 1, entrada) != 1) {
        return 0;
    }
    contador += sizeof(uint64_t);

    if (fread(&qtd_simbolos, sizeof(uint16_t), 1, entrada) != 1) {
        return 0;
    }
    contador += sizeof(uint16_t);

    if (qtd_simbolos > TOTAL_SIMBOLOS) {
        return 0;
    }

    unsigned long soma_frequencias = 0;
    unsigned int i;
    for (i = 0; i < qtd_simbolos; i++) {
        uint8_t simbolo = 0;
        uint32_t freq = 0;

        if (fread(&simbolo, sizeof(uint8_t), 1, entrada) != 1 ||
            fread(&freq, sizeof(uint32_t), 1, entrada) != 1) {
            return 0;
        }
        if (freq == 0 || huffman->frequencias[simbolo] != 0) {
            return 0; /* Frequencia invalida ou simbolo duplicado no cabecalho */
        }

        huffman->frequencias[simbolo] = (unsigned long) freq;
        soma_frequencias += (unsigned long) freq;
        contador += sizeof(uint8_t) + sizeof(uint32_t);
    }

    if (soma_frequencias != (unsigned long) total_original) {
        return 0; /* Inconsistencia entre a soma das frequencias e o total original */
    }

    huffman->tamanho_original = (unsigned long) total_original;
    huffman->simbolos_distintos = (int) qtd_simbolos;
    *bytes_lidos = contador;

    return 1;
}

/*
 * Etapa 4 do algoritmo: reler o arquivo original e gravar os codigos de Huffman
 * empacotados bit a bit em bytes no arquivo de saida.
 */
static int gravar_dados_compactados(const Huffman *huffman,
                                    FILE *entrada,
                                    FILE *saida,
                                    unsigned long *bytes_escritos) {
    unsigned char byte_atual = 0;
    int bits_no_byte = 0;
    unsigned long contador = 0;

    int caractere;
    while ((caractere = fgetc(entrada)) != EOF) {
        const char *codigo = huffman->tabela_codigos[(unsigned char) caractere];
        if (codigo == NULL) {
            return 0;
        }

        int i;
        for (i = 0; codigo[i] != '\0'; i++) {
            /* Desloca os bits ja acumulados para a esquerda e insere o proximo bit */
            byte_atual = (unsigned char) ((byte_atual << 1) | (codigo[i] == '1' ? 1 : 0));
            bits_no_byte++;

            /* Quando completamos 8 bits, gravamos o byte completo no arquivo */
            if (bits_no_byte == 8) {
                if (fputc(byte_atual, saida) == EOF) {
                    return 0;
                }
                contador++;
                byte_atual = 0;
                bits_no_byte = 0;
            }
        }
    }

    if (ferror(entrada)) {
        return 0;
    }

    /*
     * Se sobraram bits pendentes no ultimo byte (< 8 bits), deslocamos para a
     * esquerda completando os bits finais com zero (padding). Na descompactacao,
     * esses bits extras serao ignorados pois sabemos o total exato de caracteres.
     */
    if (bits_no_byte > 0) {
        byte_atual = (unsigned char) (byte_atual << (8 - bits_no_byte));
        if (fputc(byte_atual, saida) == EOF) {
            return 0;
        }
        contador++;
    }

    *bytes_escritos = contador;
    return 1;
}

/*
 * Etapa 5 do algoritmo: le os bits do arquivo compactado e caminha na Arvore
 * de Huffman (0 -> esquerda, 1 -> direita) ate recuperar exatamente todos os
 * caracteres originais.
 */
static int decodificar_dados(const Huffman *huffman,
                             FILE *entrada,
                             FILE *saida,
                             unsigned long *bytes_lidos) {
    *bytes_lidos = 0;

    /* Arquivo original vazio: nada a decodificar */
    if (huffman->tamanho_original == 0) {
        return 1;
    }

    const NoHuffman *raiz = obter_raiz_arvore_huffman(huffman->arvore);
    if (raiz == NULL) {
        return 0;
    }

    const NoHuffman *atual = raiz;
    unsigned long decodificados = 0;
    int byte_lido;

    while (decodificados < huffman->tamanho_original && (byte_lido = fgetc(entrada)) != EOF) {
        (*bytes_lidos)++;

        /* Le os 8 bits do byte do mais significativo (7) ao menos significativo (0) */
        int pos_bit;
        for (pos_bit = 7; pos_bit >= 0 && decodificados < huffman->tamanho_original; pos_bit--) {
            int bit = (byte_lido >> pos_bit) & 1;

            /* Caso especial: arvore com apenas 1 simbolo distinto (raiz e folha) */
            if (no_eh_folha(raiz)) {
                if (fputc(obter_simbolo_no(raiz), saida) == EOF) {
                    return 0;
                }
                decodificados++;
                continue;
            }

            /* Caminha na arvore conforme o bit lido */
            if (bit == 0) {
                atual = obter_esquerda_no(atual);
            } else {
                atual = obter_direita_no(atual);
            }

            if (atual == NULL) {
                return 0; /* Caminho invalido (arquivo corrompido) */
            }

            /* Ao atingir uma folha, recuperamos um simbolo e voltamos para a raiz */
            if (no_eh_folha(atual)) {
                if (fputc(obter_simbolo_no(atual), saida) == EOF) {
                    return 0;
                }
                decodificados++;
                atual = raiz;
            }
        }
    }

    if (ferror(entrada)) {
        return 0;
    }

    /* Garante que todos os caracteres originais foram recuperados */
    return (decodificados == huffman->tamanho_original);
}

/* ============================================================================
 * Funcoes publicas do TAD Huffman
 * ============================================================================
 */

Huffman *criar_huffman(void) {
    Huffman *huffman = (Huffman *) calloc(1, sizeof(Huffman));
    return huffman;
}

void liberar_huffman(Huffman *huffman) {
    if (huffman == NULL) {
        return;
    }

    limpar_estado_huffman(huffman);
    free(huffman);
}

int compactar_arquivo(Huffman *huffman,
                      const char *caminho_entrada,
                      const char *caminho_saida) {
    if (huffman == NULL || caminho_entrada == NULL || caminho_saida == NULL) {
        return 0;
    }

    limpar_estado_huffman(huffman);

    /* Passo 1: contar a frequencia de cada simbolo no arquivo de entrada */
    if (!contar_frequencias_arquivo(huffman, caminho_entrada)) {
        return 0;
    }

    /* Passo 2: construir a Arvore de Huffman a partir das frequencias */
    if (!construir_arvore_huffman(huffman)) {
        return 0;
    }

    /* Passo 3: gerar a tabela de codigos binarios de cada simbolo */
    if (!gerar_tabela_codigos(huffman)) {
        return 0;
    }

    /* Passo 4: gravar o cabecalho e os bits codificados no arquivo de saida */
    FILE *entrada = fopen(caminho_entrada, "rb");
    if (entrada == NULL) {
        return 0;
    }

    FILE *saida = fopen(caminho_saida, "wb");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }

    unsigned long bytes_cab = 0;
    unsigned long bytes_dados = 0;
    int sucesso = 1;

    if (!escrever_cabecalho(huffman, saida, &bytes_cab)) {
        sucesso = 0;
    }

    if (sucesso && huffman->tamanho_original > 0) {
        if (!gravar_dados_compactados(huffman, entrada, saida, &bytes_dados)) {
            sucesso = 0;
        }
    }

    fclose(entrada);
    if (fclose(saida) != 0) {
        sucesso = 0;
    }

    if (!sucesso) {
        return 0;
    }

    huffman->bytes_cabecalho = bytes_cab;
    huffman->bytes_dados_codificados = bytes_dados;
    huffman->tamanho_compactado = bytes_cab + bytes_dados;

    return 1;
}

int descompactar_arquivo(Huffman *huffman,
                         const char *caminho_entrada,
                         const char *caminho_saida) {
    if (huffman == NULL || caminho_entrada == NULL || caminho_saida == NULL) {
        return 0;
    }

    limpar_estado_huffman(huffman);

    FILE *entrada = fopen(caminho_entrada, "rb");
    if (entrada == NULL) {
        return 0;
    }

    /* Passo 1: ler e validar o cabecalho com a tabela de frequencias */
    unsigned long bytes_cab = 0;
    if (!ler_cabecalho(huffman, entrada, &bytes_cab)) {
        fclose(entrada);
        return 0;
    }

    /* Passo 2: reconstruir exatamente a mesma Arvore de Huffman */
    if (!construir_arvore_huffman(huffman)) {
        fclose(entrada);
        return 0;
    }

    /* Passo 3: gerar a tabela de codigos (permite consulta/impressao apos descompactar) */
    if (!gerar_tabela_codigos(huffman)) {
        fclose(entrada);
        return 0;
    }

    /* Passo 4: decodificar os bits e reconstruir o arquivo de texto original */
    FILE *saida = fopen(caminho_saida, "wb");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }

    unsigned long bytes_dados = 0;
    int sucesso = decodificar_dados(huffman, entrada, saida, &bytes_dados);

    fclose(entrada);
    if (fclose(saida) != 0) {
        sucesso = 0;
    }

    if (!sucesso) {
        return 0;
    }

    huffman->bytes_cabecalho = bytes_cab;
    huffman->bytes_dados_codificados = bytes_dados;
    huffman->tamanho_compactado = bytes_cab + bytes_dados;

    return 1;
}

int comparar_arquivos(const char *caminho_a, const char *caminho_b) {
    if (caminho_a == NULL || caminho_b == NULL) {
        return 0;
    }

    FILE *arq_a = fopen(caminho_a, "rb");
    FILE *arq_b = fopen(caminho_b, "rb");
    if (arq_a == NULL || arq_b == NULL) {
        if (arq_a != NULL) fclose(arq_a);
        if (arq_b != NULL) fclose(arq_b);
        return 0;
    }

    int iguais = 1;
    while (1) {
        int ca = fgetc(arq_a);
        int cb = fgetc(arq_b);

        if (ca != cb) {
            iguais = 0;
            break;
        }
        if (ca == EOF) {
            break;
        }
    }

    fclose(arq_a);
    fclose(arq_b);
    return iguais;
}

/* --- Consultas de estatisticas --- */

unsigned long obter_tamanho_original(const Huffman *huffman) {
    return (huffman != NULL) ? huffman->tamanho_original : 0;
}

unsigned long obter_bytes_cabecalho(const Huffman *huffman) {
    return (huffman != NULL) ? huffman->bytes_cabecalho : 0;
}

unsigned long obter_bytes_dados_codificados(const Huffman *huffman) {
    return (huffman != NULL) ? huffman->bytes_dados_codificados : 0;
}

unsigned long obter_tamanho_compactado(const Huffman *huffman) {
    return (huffman != NULL) ? huffman->tamanho_compactado : 0;
}

int obter_simbolos_distintos(const Huffman *huffman) {
    return (huffman != NULL) ? huffman->simbolos_distintos : 0;
}

int obter_altura_arvore(const Huffman *huffman) {
    if (huffman == NULL || huffman->arvore == NULL) {
        return 0;
    }
    return altura_arvore_huffman(huffman->arvore);
}

int obter_total_nos_arvore(const Huffman *huffman) {
    if (huffman == NULL || huffman->arvore == NULL) {
        return 0;
    }
    return total_nos_arvore_huffman(huffman->arvore);
}

double obter_taxa_reducao(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) {
        return 0.0;
    }
    return (1.0 - ((double) huffman->tamanho_compactado / (double) huffman->tamanho_original)) * 100.0;
}

double obter_taxa_reducao_dados(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) {
        return 0.0;
    }
    return (1.0 - ((double) huffman->bytes_dados_codificados / (double) huffman->tamanho_original)) * 100.0;
}

double obter_media_bits_por_simbolo(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) {
        return 0.0;
    }

    unsigned long total_bits = 0;
    int i;
    for (i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0 && huffman->tabela_codigos[i] != NULL) {
            total_bits += huffman->frequencias[i] * (unsigned long) strlen(huffman->tabela_codigos[i]);
        }
    }

    return (double) total_bits / (double) huffman->tamanho_original;
}

/* --- Funcoes de exibicao --- */

void imprimir_tabela_codigos(const Huffman *huffman) {
    if (huffman == NULL || huffman->simbolos_distintos == 0) {
        printf("(Tabela de codigos vazia)\n");
        return;
    }

    printf("\n+------------+------------+--------+--------------------------+\n");
    printf("| Simbolo    | Frequencia | Bits   | Codigo de Huffman        |\n");
    printf("+------------+------------+--------+--------------------------+\n");

    int i;
    for (i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0 && huffman->tabela_codigos[i] != NULL) {
            char rotulo[16];
            if (i == '\n') {
                strcpy(rotulo, "'\\n'");
            } else if (i == '\r') {
                strcpy(rotulo, "'\\r'");
            } else if (i == '\t') {
                strcpy(rotulo, "'\\t'");
            } else if (i == ' ') {
                strcpy(rotulo, "' ' (esp)");
            } else if (i >= 33 && i <= 126) {
                sprintf(rotulo, "'%c'", i);
            } else {
                sprintf(rotulo, "0x%02X", i);
            }

            printf("| %-10s | %10lu | %6lu | %-24s |\n",
                   rotulo,
                   huffman->frequencias[i],
                   (unsigned long) strlen(huffman->tabela_codigos[i]),
                   huffman->tabela_codigos[i]);
        }
    }

    printf("+------------+------------+--------+--------------------------+\n");
}

void imprimir_resumo_huffman(const Huffman *huffman) {
    if (huffman == NULL) {
        return;
    }

    printf("\n=== Resumo da Compressao de Huffman ===\n");
    printf("Tamanho original           : %lu bytes (%.2f KB)\n",
           huffman->tamanho_original,
           (double) huffman->tamanho_original / 1024.0);
    printf("Simbolos distintos         : %d\n", huffman->simbolos_distintos);
    printf("Total de nos na arvore     : %d\n", obter_total_nos_arvore(huffman));
    printf("Altura da arvore           : %d\n", obter_altura_arvore(huffman));
    printf("Bytes do cabecalho         : %lu bytes\n", huffman->bytes_cabecalho);
    printf("Bytes de dados codificados : %lu bytes (%.2f KB)\n",
           huffman->bytes_dados_codificados,
           (double) huffman->bytes_dados_codificados / 1024.0);
    printf("Tamanho total compactado   : %lu bytes (%.2f KB)\n",
           huffman->tamanho_compactado,
           (double) huffman->tamanho_compactado / 1024.0);
    printf("Media de bits/simbolo      : %.2f bits (original: 8.00 bits)\n",
           obter_media_bits_por_simbolo(huffman));
    printf("Reducao real (c/ cabecalho): %.2f%%\n", obter_taxa_reducao(huffman));
    printf("Reducao dados (s/ cabec.)  : %.2f%%\n", obter_taxa_reducao_dados(huffman));
    printf("=======================================\n");
}

void imprimir_arvore_do_huffman(const Huffman *huffman) {
    if (huffman == NULL || huffman->arvore == NULL) {
        printf("(Arvore de Huffman vazia)\n");
        return;
    }
    imprimir_arvore_huffman(huffman->arvore);
}
