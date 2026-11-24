#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Huffman.h"
#include "ArvoreHuffman.h"
#include "FilaPrioridade.h"

#define TOTAL_SIMBOLOS 256
#define ASSINATURA_HUFFMAN "HUF1"
#define TAMANHO_ASSINATURA 4
#define TAMANHO_CABECALHO_FIXO 14

struct Huffman {
    unsigned long frequencias[TOTAL_SIMBOLOS];
    ArvoreHuffman *arvore;
    char *tabela_codigos[TOTAL_SIMBOLOS];

    unsigned long tamanho_original;
    unsigned long bytes_cabecalho;
    unsigned long bytes_dados_codificados;
    unsigned long tamanho_compactado;
    int simbolos_distintos;
};

// Calcula log2(x) por quadrados sucessivos (sem depender de -lm)
static double calcular_log2(double x) {
    if (x <= 0.0) return 0.0;

    double resultado = 0.0;
    while (x >= 2.0) {
        x /= 2.0;
        resultado += 1.0;
    }
    while (x < 1.0) {
        x *= 2.0;
        resultado -= 1.0;
    }

    double peso = 0.5;
    for (int i = 0; i < 40; i++) {
        x = x * x;
        if (x >= 2.0) {
            x /= 2.0;
            resultado += peso;
        }
        peso *= 0.5;
    }
    return resultado;
}

// Libera arvore e codigos anteriores e zera os contadores do TAD
static void limpar_estado_huffman(Huffman *huffman) {
    if (huffman->arvore != NULL) {
        liberar_arvore_huffman(huffman->arvore);
        huffman->arvore = NULL;
    }

    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        huffman->frequencias[i] = 0;
        free(huffman->tabela_codigos[i]);
        huffman->tabela_codigos[i] = NULL;
    }

    huffman->tamanho_original = 0;
    huffman->bytes_cabecalho = 0;
    huffman->bytes_dados_codificados = 0;
    huffman->tamanho_compactado = 0;
    huffman->simbolos_distintos = 0;
}

// Conta a frequencia de cada caractere abrindo o arquivo em modo texto ("r")
static int contar_frequencias_arquivo(Huffman *huffman, const char *caminho_entrada) {
    FILE *entrada = fopen(caminho_entrada, "r");
    if (entrada == NULL) return 0;

    int c;
    while ((c = fgetc(entrada)) != EOF) {
        unsigned char simbolo = (unsigned char) c;
        if (huffman->frequencias[simbolo] == 0)
            huffman->simbolos_distintos++;
        huffman->frequencias[simbolo]++;
        huffman->tamanho_original++;
    }

    int erro = ferror(entrada);
    fclose(entrada);
    return !erro;
}

// Monta a Arvore de Huffman unindo sempre os dois nos de menor frequencia
static int construir_arvore_huffman(Huffman *huffman) {
    if (huffman->simbolos_distintos == 0) {
        huffman->arvore = criar_arvore_huffman(NULL);
        return (huffman->arvore != NULL);
    }

    FilaPrioridade *fila = criar_fila_prioridade(TOTAL_SIMBOLOS);
    if (fila == NULL) return 0;

    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0) {
            NoHuffman *folha = criar_no_folha((unsigned char) i, huffman->frequencias[i]);
            if (folha == NULL || !inserir_fila_prioridade(fila, folha)) {
                liberar_no_huffman(folha);
                liberar_fila_prioridade(fila);
                return 0;
            }
        }
    }

    while (tamanho_fila_prioridade(fila) > 1) {
        NoHuffman *menor1 = remover_menor_fila_prioridade(fila);
        NoHuffman *menor2 = remover_menor_fila_prioridade(fila);

        NoHuffman *pai = criar_no_interno(menor1, menor2);
        if (pai == NULL || !inserir_fila_prioridade(fila, pai)) {
            if (pai == NULL) {
                liberar_no_huffman(menor1);
                liberar_no_huffman(menor2);
            } else {
                liberar_no_huffman(pai);
            }
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

// Cabecalho (.huff): "HUF1" (4B) + total_original (uint64_t) + k (uint16_t) + k*(simbolo + freq)
static int escrever_cabecalho(const Huffman *huffman, FILE *saida, unsigned long *bytes_escritos) {
    uint64_t total_original = (uint64_t) huffman->tamanho_original;
    uint16_t qtd_simbolos = (uint16_t) huffman->simbolos_distintos;

    if (fwrite(ASSINATURA_HUFFMAN, 1, TAMANHO_ASSINATURA, saida) != TAMANHO_ASSINATURA ||
        fwrite(&total_original, sizeof(uint64_t), 1, saida) != 1 ||
        fwrite(&qtd_simbolos, sizeof(uint16_t), 1, saida) != 1) {
        return 0;
    }

    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0) {
            if (huffman->frequencias[i] > UINT32_MAX) return 0;

            uint8_t simbolo = (uint8_t) i;
            uint32_t freq = (uint32_t) huffman->frequencias[i];
            if (fwrite(&simbolo, sizeof(uint8_t), 1, saida) != 1 ||
                fwrite(&freq, sizeof(uint32_t), 1, saida) != 1) {
                return 0;
            }
        }
    }

    *bytes_escritos = TAMANHO_CABECALHO_FIXO + (unsigned long) qtd_simbolos * 5UL;
    return 1;
}

// Le e valida assinatura, quantidade de simbolos e consistencia das frequencias
static int ler_cabecalho(Huffman *huffman, FILE *entrada, unsigned long *bytes_lidos) {
    char assinatura[TAMANHO_ASSINATURA];
    uint64_t total_original = 0;
    uint16_t qtd_simbolos = 0;

    if (fread(assinatura, 1, TAMANHO_ASSINATURA, entrada) != TAMANHO_ASSINATURA ||
        memcmp(assinatura, ASSINATURA_HUFFMAN, TAMANHO_ASSINATURA) != 0 ||
        fread(&total_original, sizeof(uint64_t), 1, entrada) != 1 ||
        fread(&qtd_simbolos, sizeof(uint16_t), 1, entrada) != 1 ||
        qtd_simbolos > TOTAL_SIMBOLOS) {
        return 0;
    }

    unsigned long soma_frequencias = 0;
    for (unsigned int i = 0; i < qtd_simbolos; i++) {
        uint8_t simbolo = 0;
        uint32_t freq = 0;

        if (fread(&simbolo, sizeof(uint8_t), 1, entrada) != 1 ||
            fread(&freq, sizeof(uint32_t), 1, entrada) != 1 ||
            freq == 0 || huffman->frequencias[simbolo] != 0) {
            return 0;
        }

        huffman->frequencias[simbolo] = (unsigned long) freq;
        soma_frequencias += (unsigned long) freq;
    }

    if (soma_frequencias != (unsigned long) total_original) return 0;

    huffman->tamanho_original = (unsigned long) total_original;
    huffman->simbolos_distintos = (int) qtd_simbolos;
    *bytes_lidos = TAMANHO_CABECALHO_FIXO + (unsigned long) qtd_simbolos * 5UL;
    return 1;
}

// Rele o arquivo original e empacota os codigos bit a bit em bytes
static int gravar_dados_compactados(const Huffman *huffman,
                                    FILE *entrada,
                                    FILE *saida,
                                    unsigned long *bytes_escritos) {
    unsigned char byte_atual = 0;
    int bits_no_byte = 0;
    unsigned long contador = 0;

    int c;
    while ((c = fgetc(entrada)) != EOF) {
        const char *codigo = huffman->tabela_codigos[(unsigned char) c];
        if (codigo == NULL) return 0;

        for (int i = 0; codigo[i] != '\0'; i++) {
            byte_atual = (unsigned char) ((byte_atual << 1) | (codigo[i] == '1'));
            bits_no_byte++;

            if (bits_no_byte == 8) {
                if (fputc(byte_atual, saida) == EOF) return 0;
                contador++;
                byte_atual = 0;
                bits_no_byte = 0;
            }
        }
    }

    if (ferror(entrada)) return 0;

    // Completa o ultimo byte com zeros a direita (padding) se sobrarem bits
    if (bits_no_byte > 0) {
        byte_atual = (unsigned char) (byte_atual << (8 - bits_no_byte));
        if (fputc(byte_atual, saida) == EOF) return 0;
        contador++;
    }

    *bytes_escritos = contador;
    return 1;
}

// Le os bits do arquivo .huff caminhando da raiz as folhas e valida padding/EOF
static int decodificar_dados(const Huffman *huffman,
                             FILE *entrada,
                             FILE *saida,
                             unsigned long *bytes_lidos) {
    *bytes_lidos = 0;

    if (huffman->tamanho_original == 0)
        return (fgetc(entrada) == EOF && !ferror(entrada));

    const NoHuffman *raiz = obter_raiz_arvore_huffman(huffman->arvore);
    if (raiz == NULL) return 0;

    const NoHuffman *atual = raiz;
    unsigned long decodificados = 0;
    int byte_lido;

    while (decodificados < huffman->tamanho_original && (byte_lido = fgetc(entrada)) != EOF) {
        (*bytes_lidos)++;

        int pos_bit;
        for (pos_bit = 7; pos_bit >= 0 && decodificados < huffman->tamanho_original; pos_bit--) {
            int bit = (byte_lido >> pos_bit) & 1;

            // Caso especial: arvore com 1 unico simbolo (codigo esperado = 0)
            if (no_eh_folha(raiz)) {
                if (bit != 0 || fputc(obter_simbolo_no(raiz), saida) == EOF) return 0;
                decodificados++;
                continue;
            }

            atual = (bit == 0) ? obter_esquerda_no(atual) : obter_direita_no(atual);
            if (atual == NULL) return 0;

            if (no_eh_folha(atual)) {
                if (fputc(obter_simbolo_no(atual), saida) == EOF) return 0;
                decodificados++;
                atual = raiz;
            }
        }

        // Garante que os bits residuais de padding sao todos zero
        if (decodificados == huffman->tamanho_original) {
            for (; pos_bit >= 0; pos_bit--) {
                if (((byte_lido >> pos_bit) & 1) != 0) return 0;
            }
        }
    }

    if (ferror(entrada) || decodificados != huffman->tamanho_original) return 0;
    return (fgetc(entrada) == EOF && !ferror(entrada));
}

Huffman *criar_huffman(void) {
    return (Huffman *) calloc(1, sizeof(Huffman));
}

void liberar_huffman(Huffman *huffman) {
    if (huffman == NULL) return;
    limpar_estado_huffman(huffman);
    free(huffman);
}

// Compacta caminho_entrada (modo texto) gravando cabecalho e bits em caminho_saida
int compactar_arquivo(Huffman *huffman, const char *caminho_entrada, const char *caminho_saida) {
    if (huffman == NULL || caminho_entrada == NULL || caminho_saida == NULL) return 0;

    limpar_estado_huffman(huffman);

    if (!contar_frequencias_arquivo(huffman, caminho_entrada) ||
        !construir_arvore_huffman(huffman) ||
        !gerar_codigos_arvore_huffman(huffman->arvore, huffman->tabela_codigos)) {
        return 0;
    }

    FILE *entrada = fopen(caminho_entrada, "r");
    if (entrada == NULL) return 0;

    FILE *saida = fopen(caminho_saida, "wb");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }

    unsigned long bytes_cab = 0;
    unsigned long bytes_dados = 0;
    int sucesso = escrever_cabecalho(huffman, saida, &bytes_cab);

    if (sucesso && huffman->tamanho_original > 0)
        sucesso = gravar_dados_compactados(huffman, entrada, saida, &bytes_dados);

    fclose(entrada);
    if (fclose(saida) != 0) sucesso = 0;
    if (!sucesso) return 0;

    huffman->bytes_cabecalho = bytes_cab;
    huffman->bytes_dados_codificados = bytes_dados;
    huffman->tamanho_compactado = bytes_cab + bytes_dados;
    return 1;
}

// Descompacta caminho_entrada (.huff) restaurando o texto original em caminho_saida
int descompactar_arquivo(Huffman *huffman, const char *caminho_entrada, const char *caminho_saida) {
    if (huffman == NULL || caminho_entrada == NULL || caminho_saida == NULL) return 0;

    limpar_estado_huffman(huffman);

    FILE *entrada = fopen(caminho_entrada, "rb");
    if (entrada == NULL) return 0;

    unsigned long bytes_cab = 0;
    if (!ler_cabecalho(huffman, entrada, &bytes_cab) ||
        !construir_arvore_huffman(huffman) ||
        !gerar_codigos_arvore_huffman(huffman->arvore, huffman->tabela_codigos)) {
        fclose(entrada);
        return 0;
    }

    FILE *saida = fopen(caminho_saida, "w");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }

    unsigned long bytes_dados = 0;
    int sucesso = decodificar_dados(huffman, entrada, saida, &bytes_dados);

    fclose(entrada);
    if (fclose(saida) != 0) sucesso = 0;
    if (!sucesso) return 0;

    huffman->bytes_cabecalho = bytes_cab;
    huffman->bytes_dados_codificados = bytes_dados;
    huffman->tamanho_compactado = bytes_cab + bytes_dados;
    return 1;
}

// Compara dois arquivos em modo texto ("r") para manter consistencia entre OS
int comparar_arquivos(const char *caminho_a, const char *caminho_b) {
    if (caminho_a == NULL || caminho_b == NULL) return 0;

    FILE *arq_a = fopen(caminho_a, "r");
    FILE *arq_b = fopen(caminho_b, "r");
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
        if (ca == EOF) break;
    }

    fclose(arq_a);
    fclose(arq_b);
    return iguais;
}

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
    return (huffman != NULL && huffman->arvore != NULL) ? altura_arvore_huffman(huffman->arvore) : 0;
}

int obter_total_nos_arvore(const Huffman *huffman) {
    return (huffman != NULL && huffman->arvore != NULL) ? total_nos_arvore_huffman(huffman->arvore) : 0;
}

double obter_taxa_reducao(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) return 0.0;
    return (1.0 - ((double) huffman->tamanho_compactado / (double) huffman->tamanho_original)) * 100.0;
}

double obter_taxa_reducao_dados(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) return 0.0;
    return (1.0 - ((double) huffman->bytes_dados_codificados / (double) huffman->tamanho_original)) * 100.0;
}

// Calcula a media ponderada de bits por simbolo (original sem compressao = 8 bits)
double obter_media_bits_por_simbolo(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) return 0.0;

    unsigned long total_bits = 0;
    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0 && huffman->tabela_codigos[i] != NULL)
            total_bits += huffman->frequencias[i] * (unsigned long) strlen(huffman->tabela_codigos[i]);
    }
    return (double) total_bits / (double) huffman->tamanho_original;
}

// Entropia de Shannon: H(X) = -soma(p[i] * log2(p[i]))
double obter_entropia_shannon(const Huffman *huffman) {
    if (huffman == NULL || huffman->tamanho_original == 0) return 0.0;

    double entropia = 0.0;
    double total = (double) huffman->tamanho_original;
    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0) {
            double p = (double) huffman->frequencias[i] / total;
            entropia -= p * calcular_log2(p);
        }
    }
    return entropia;
}

// Exibe na tela os simbolos presentes, frequencias e codigos binarios gerados
void imprimir_tabela_codigos(const Huffman *huffman) {
    if (huffman == NULL || huffman->simbolos_distintos == 0) {
        printf("(Tabela de codigos vazia)\n");
        return;
    }

    printf("\n%-10s | %-10s | %-5s | %s\n", "Simbolo", "Frequencia", "Bits", "Codigo");
    printf("-----------+------------+-------+------------------------\n");

    for (int i = 0; i < TOTAL_SIMBOLOS; i++) {
        if (huffman->frequencias[i] > 0 && huffman->tabela_codigos[i] != NULL) {
            char rotulo[16];
            if (i == '\n') strcpy(rotulo, "'\\n'");
            else if (i == '\r') strcpy(rotulo, "'\\r'");
            else if (i == '\t') strcpy(rotulo, "'\\t'");
            else if (i == ' ') strcpy(rotulo, "' ' (esp)");
            else if (i >= 33 && i <= 126) sprintf(rotulo, "'%c'", i);
            else sprintf(rotulo, "0x%02X", i);

            printf("%-10s | %10lu | %5lu | %s\n",
                   rotulo,
                   huffman->frequencias[i],
                   (unsigned long) strlen(huffman->tabela_codigos[i]),
                   huffman->tabela_codigos[i]);
        }
    }
}

// Exibe um resumo com tamanhos, entropia, media de bits e taxas de reducao
void imprimir_resumo_huffman(const Huffman *huffman) {
    if (huffman == NULL) return;

    printf("\nResumo da Compressao de Huffman:\n");
    printf("  Tamanho original           : %lu bytes (%.2f KB)\n",
           huffman->tamanho_original, (double) huffman->tamanho_original / 1024.0);
    printf("  Simbolos distintos         : %d\n", huffman->simbolos_distintos);
    printf("  Nos / altura da arvore     : %d nos / altura %d\n",
           obter_total_nos_arvore(huffman), obter_altura_arvore(huffman));
    printf("  Cabecalho / dados codif.   : %lu bytes / %lu bytes\n",
           huffman->bytes_cabecalho, huffman->bytes_dados_codificados);
    printf("  Tamanho total compactado   : %lu bytes (%.2f KB)\n",
           huffman->tamanho_compactado, (double) huffman->tamanho_compactado / 1024.0);
    printf("  Entropia H / media L       : %.4f / %.4f bits/simbolo\n",
           obter_entropia_shannon(huffman), obter_media_bits_por_simbolo(huffman));
    printf("  Reducao real / apenas dados: %.2f%% / %.2f%%\n",
           obter_taxa_reducao(huffman), obter_taxa_reducao_dados(huffman));
}

void imprimir_arvore_do_huffman(const Huffman *huffman) {
    if (huffman == NULL || huffman->arvore == NULL) {
        printf("(Arvore vazia)\n");
        return;
    }
    imprimir_arvore_huffman(huffman->arvore);
}
