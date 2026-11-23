#ifndef HUFFMAN_H
#define HUFFMAN_H

/*
 * TAD Opaco: Huffman
 * Implementa as etapas de contagem de frequencias, construcao da arvore,
 * geracao da tabela de codigos livres de prefixo, compactacao bit a bit
 * e descompactacao de arquivos de texto.
 */

typedef struct Huffman Huffman;

/* Criacao e liberacao */
Huffman *criar_huffman(void);
void liberar_huffman(Huffman *huffman);

/* Compactacao, descompactacao e verificacao de integridade */
int compactar_arquivo(Huffman *huffman,
                      const char *caminho_entrada,
                      const char *caminho_saida);

int descompactar_arquivo(Huffman *huffman,
                         const char *caminho_entrada,
                         const char *caminho_saida);

int comparar_arquivos(const char *caminho_a, const char *caminho_b);

/* Consulta de metricas da ultima operacao */
unsigned long obter_tamanho_original(const Huffman *huffman);
unsigned long obter_bytes_cabecalho(const Huffman *huffman);
unsigned long obter_bytes_dados_codificados(const Huffman *huffman);
unsigned long obter_tamanho_compactado(const Huffman *huffman);
int obter_simbolos_distintos(const Huffman *huffman);
int obter_altura_arvore(const Huffman *huffman);
int obter_total_nos_arvore(const Huffman *huffman);
double obter_taxa_reducao(const Huffman *huffman);
double obter_taxa_reducao_dados(const Huffman *huffman);
double obter_media_bits_por_simbolo(const Huffman *huffman);
double obter_entropia_shannon(const Huffman *huffman);

/* Impressao de tabelas e estrutura */
void imprimir_tabela_codigos(const Huffman *huffman);
void imprimir_resumo_huffman(const Huffman *huffman);
void imprimir_arvore_do_huffman(const Huffman *huffman);

#endif
