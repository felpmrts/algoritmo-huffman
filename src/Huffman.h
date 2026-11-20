#ifndef HUFFMAN_H
#define HUFFMAN_H

/*
 * ============================================================================
 * TAD Opaco: Huffman
 * ----------------------------------------------------------------------------
 * Encapsula todo o funcionamento do algoritmo de compressao de Huffman:
 *   1. Contagem de frequencias dos simbolos do arquivo de texto;
 *   2. Construcao da Arvore de Huffman usando Fila de Prioridade (Min-Heap);
 *   3. Geracao da tabela de codigos binarios livres de prefixo;
 *   4. Compactacao em nivel de bits com cabecalho autossuficiente;
 *   5. Descompactacao fiel (sem perdas) e estatisticas de compressao.
 *
 * Todos os dados internos sao protegidos pelo tipo opaco 'Huffman'.
 * ============================================================================
 */

/* Tipo opaco: a definicao completa da struct existe apenas em Huffman.c */
typedef struct Huffman Huffman;

/* --- Ciclo de vida do TAD --- */

/* Cria e inicializa uma nova instancia do TAD Huffman. */
Huffman *criar_huffman(void);

/* Libera toda a memoria associada ao TAD Huffman (arvore, tabela e struct). */
void liberar_huffman(Huffman *huffman);

/* --- Operacoes principais de compactacao e descompactacao --- */

/*
 * Compacta o arquivo de texto 'caminho_entrada' gerando o arquivo binario
 * 'caminho_saida' contendo o cabecalho (tabela de frequencias) e os bits
 * codificados pelo algoritmo de Huffman.
 * Retorna 1 em caso de sucesso, ou 0 em caso de erro.
 */
int compactar_arquivo(Huffman *huffman,
                      const char *caminho_entrada,
                      const char *caminho_saida);

/*
 * Descompacta o arquivo 'caminho_entrada' (gerado por compactar_arquivo)
 * restaurando o arquivo de texto original em 'caminho_saida'.
 * Retorna 1 em caso de sucesso, ou 0 em caso de erro.
 */
int descompactar_arquivo(Huffman *huffman,
                         const char *caminho_entrada,
                         const char *caminho_saida);

/*
 * Compara dois arquivos byte a byte para validar que a descompactacao
 * recuperou exatamente o conteudo original (compressao sem perdas).
 * Retorna 1 se os arquivos forem identicos, ou 0 se diferirem (ou em erro).
 */
int comparar_arquivos(const char *caminho_a, const char *caminho_b);

/* --- Funcoes de consulta das estatisticas da ultima operacao --- */

/* Tamanho do arquivo de texto original em bytes. */
unsigned long obter_tamanho_original(const Huffman *huffman);

/* Quantidade de bytes usados pelo cabecalho no arquivo compactado. */
unsigned long obter_bytes_cabecalho(const Huffman *huffman);

/* Quantidade de bytes ocupados apenas pelos dados codificados (sem cabecalho). */
unsigned long obter_bytes_dados_codificados(const Huffman *huffman);

/* Tamanho total do arquivo compactado em bytes (cabecalho + dados). */
unsigned long obter_tamanho_compactado(const Huffman *huffman);

/* Quantidade de simbolos (bytes) distintos encontrados no arquivo. */
int obter_simbolos_distintos(const Huffman *huffman);

/* Altura da Arvore de Huffman construida. */
int obter_altura_arvore(const Huffman *huffman);

/* Quantidade total de nos da Arvore de Huffman construida. */
int obter_total_nos_arvore(const Huffman *huffman);

/*
 * Percentual de reducao real do arquivo (considerando o cabecalho):
 *   (1 - tamanho_compactado / tamanho_original) * 100
 * Pode ser negativo em arquivos minusculos onde o cabecalho supera a economia.
 */
double obter_taxa_reducao(const Huffman *huffman);

/*
 * Percentual de reducao considerando apenas os dados codificados (sem cabecalho):
 *   (1 - bytes_dados_codificados / tamanho_original) * 100
 */
double obter_taxa_reducao_dados(const Huffman *huffman);

/* Media de bits utilizados por caractere codificado (original = 8.00 bits). */
double obter_media_bits_por_simbolo(const Huffman *huffman);

/* --- Funcoes didaticas de exibicao --- */

/* Imprime a tabela de frequencias e os codigos binarios gerados para cada simbolo. */
void imprimir_tabela_codigos(const Huffman *huffman);

/* Imprime um resumo formatado com todas as metricas da ultima compactacao. */
void imprimir_resumo_huffman(const Huffman *huffman);

/* Imprime a Arvore de Huffman atual em pre-ordem. */
void imprimir_arvore_do_huffman(const Huffman *huffman);

#endif /* HUFFMAN_H */
