#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Huffman.h"

#define TAM_CAMINHO 256

static void remover_quebra_linha(char *texto) {
    size_t tam = strlen(texto);
    if (tam > 0 && texto[tam - 1] == '\n') {
        texto[tam - 1] = '\0';
    }
    tam = strlen(texto);
    if (tam > 0 && texto[tam - 1] == '\r') {
        texto[tam - 1] = '\0';
    }
}

/*
 * Localiza o arquivo de teste tanto na subpasta "dados/" quanto no diretorio
 * atual (util quando todos os arquivos sao entregues juntos sem subpastas).
 */
static const char *resolver_caminho_arquivo(const char *caminho_padrao,
                                            const char *nome_curto) {
    FILE *f = fopen(caminho_padrao, "rb");
    if (f != NULL) {
        fclose(f);
        return caminho_padrao;
    }

    f = fopen(nome_curto, "rb");
    if (f != NULL) {
        fclose(f);
        return nome_curto;
    }

    return caminho_padrao;
}

static int executar_teste_arquivo(Huffman *huffman,
                                  const char *caminho_entrada,
                                  int exibir_tabela,
                                  int exibir_arvore) {
    const char *arquivo_compactado = "temp_teste.huff";
    const char *arquivo_restaurado = "temp_restaurado.txt";

    printf("\n------------------------------------------------------------\n");
    printf("Arquivo de teste: %s\n", caminho_entrada);

    clock_t inicio_comp = clock();
    if (!compactar_arquivo(huffman, caminho_entrada, arquivo_compactado)) {
        printf("ERRO: nao foi possivel compactar '%s'.\n", caminho_entrada);
        return 0;
    }
    clock_t fim_comp = clock();

    if (exibir_arvore) {
        printf("\n=== Estrutura da Arvore de Huffman (Pre-Ordem) ===\n");
        imprimir_arvore_do_huffman(huffman);
    }
    if (exibir_tabela) {
        imprimir_tabela_codigos(huffman);
    }
    imprimir_resumo_huffman(huffman);

    clock_t inicio_desc = clock();
    if (!descompactar_arquivo(huffman, arquivo_compactado, arquivo_restaurado)) {
        printf("ERRO: nao foi possivel descompactar '%s'.\n", arquivo_compactado);
        remove(arquivo_compactado);
        return 0;
    }
    clock_t fim_desc = clock();

    int integro = comparar_arquivos(caminho_entrada, arquivo_restaurado);
    double tempo_comp_ms = ((double) (fim_comp - inicio_comp) / CLOCKS_PER_SEC) * 1000.0;
    double tempo_desc_ms = ((double) (fim_desc - inicio_desc) / CLOCKS_PER_SEC) * 1000.0;

    printf("Tempo de compactacao       : %.2f ms\n", tempo_comp_ms);
    printf("Tempo de descompactacao    : %.2f ms\n", tempo_desc_ms);
    printf("Verificacao byte a byte    : %s\n",
           integro ? "OK (arquivo 100% identico ao original)" : "FALHA (divergencia encontrada)");

    remove(arquivo_compactado);
    remove(arquivo_restaurado);

    return integro;
}

static int executar_bateria_testes(Huffman *huffman) {
    const char *caminhos_dados[] = {
        "dados/texto_pequeno_2kb.txt",
        "dados/texto_medio_15kb.txt",
        "dados/texto_medio_50kb.txt",
        "dados/texto_grande_200kb.txt",
        "dados/texto_grande_500kb.txt"
    };
    const char *nomes_planos[] = {
        "texto_pequeno_2kb.txt",
        "texto_medio_15kb.txt",
        "texto_medio_50kb.txt",
        "texto_grande_200kb.txt",
        "texto_grande_500kb.txt"
    };
    const int total_arquivos = (int) (sizeof(caminhos_dados) / sizeof(caminhos_dados[0]));

    const char *arquivo_compactado = "temp_bateria.huff";
    const char *arquivo_restaurado = "temp_bateria.txt";

    printf("\n===============================================================================================================================\n");
    printf("                                BATERIA DE TESTES - COMPRESSAO DE HUFFMAN POR TAMANHO\n");
    printf("===============================================================================================================================\n");
    printf("%-26s | %9s | %4s | %4s | %7s | %9s | %8s | %8s | %8s | %8s | %5s\n",
           "Arquivo", "Original", "Simb", "Nos", "Cabec.", "Compact.", "Entropia", "Bits/S", "Red.Real", "Red.Dado", "Integ");
    printf("---------------------------+-----------+------+------+---------+-----------+----------+----------+----------+----------+------\n");

    int todos_ok = 1;
    int i;
    for (i = 0; i < total_arquivos; i++) {
        const char *caminho = resolver_caminho_arquivo(caminhos_dados[i], nomes_planos[i]);

        if (!compactar_arquivo(huffman, caminho, arquivo_compactado)) {
            printf("%-26s | ERRO ao abrir ou compactar o arquivo\n", caminho);
            todos_ok = 0;
            continue;
        }

        unsigned long orig = obter_tamanho_original(huffman);
        int simb = obter_simbolos_distintos(huffman);
        int nos = obter_total_nos_arvore(huffman);
        unsigned long cab = obter_bytes_cabecalho(huffman);
        unsigned long comp = obter_tamanho_compactado(huffman);
        double entropia = obter_entropia_shannon(huffman);
        double bits_simb = obter_media_bits_por_simbolo(huffman);
        double red_real = obter_taxa_reducao(huffman);
        double red_dados = obter_taxa_reducao_dados(huffman);

        int desc_ok = descompactar_arquivo(huffman, arquivo_compactado, arquivo_restaurado);
        int integro = desc_ok && comparar_arquivos(caminho, arquivo_restaurado);
        if (!integro) {
            todos_ok = 0;
        }

        printf("%-26s | %7lu B | %4d | %4d | %5lu B | %7lu B | %8.4f | %8.4f | %7.2f%% | %7.2f%% | %-5s\n",
               nomes_planos[i], orig, simb, nos, cab, comp, entropia, bits_simb, red_real, red_dados,
               integro ? "OK" : "ERRO");
    }

    printf("===============================================================================================================================\n");

    remove(arquivo_compactado);
    remove(arquivo_restaurado);

    return todos_ok;
}

/* Testa casos de fronteira: arquivo vazio, simbolo unico, UTF-8 e arquivo inexistente */
static int executar_testes_casos_especiais(Huffman *huffman) {
    const char *arq_in = "temp_especial_in.txt";
    const char *arq_huff = "temp_especial.huff";
    const char *arq_out = "temp_especial_out.txt";
    int todos_ok = 1;

    printf("\n=== Testes de Casos de Fronteira ===\n");

    /* Caso 1: arquivo vazio */
    {
        FILE *f = fopen(arq_in, "wb");
        if (f != NULL) fclose(f);

        int ok = compactar_arquivo(huffman, arq_in, arq_huff) &&
                 descompactar_arquivo(huffman, arq_huff, arq_out) &&
                 comparar_arquivos(arq_in, arq_out);
        printf("[Caso 1] Arquivo vazio (0 bytes)                      : %s\n", ok ? "OK" : "FALHA");
        if (!ok) todos_ok = 0;
    }

    /* Caso 2: arquivo com um unico simbolo repetido */
    {
        FILE *f = fopen(arq_in, "wb");
        if (f != NULL) {
            int i;
            for (i = 0; i < 100; i++) fputc('A', f);
            fclose(f);
        }

        int ok = compactar_arquivo(huffman, arq_in, arq_huff) &&
                 descompactar_arquivo(huffman, arq_huff, arq_out) &&
                 comparar_arquivos(arq_in, arq_out);
        printf("[Caso 2] Unico simbolo repetido ('A' x 100)           : %s\n", ok ? "OK" : "FALHA");
        if (!ok) todos_ok = 0;
    }

    /* Caso 3: texto curto com acentos UTF-8, quebras de linha e pontuacao */
    {
        FILE *f = fopen(arq_in, "w");
        if (f != NULL) {
            fputs("Estrutura de Dados II - Compressao com Huffman!\n", f);
            fputs("Acentuacao: acao, coracao, arvore, frequencia, codigo.\n", f);
            fclose(f);
        }

        int ok = compactar_arquivo(huffman, arq_in, arq_huff) &&
                 descompactar_arquivo(huffman, arq_huff, arq_out) &&
                 comparar_arquivos(arq_in, arq_out);
        printf("[Caso 3] Texto curto com acentuacao e quebras de linha: %s\n", ok ? "OK" : "FALHA");
        if (!ok) todos_ok = 0;
    }

    /* Caso 4: tentativa de abrir arquivo inexistente */
    {
        int falhou_corretamente = !compactar_arquivo(huffman, "arquivo_que_nao_existe.txt", arq_huff);
        printf("[Caso 4] Tratamento de arquivo inexistente            : %s\n",
               falhou_corretamente ? "OK" : "FALHA");
        if (!falhou_corretamente) todos_ok = 0;
    }

    /* Caso 5: rejeicao de arquivo .huff invalido ou corrompido */
    {
        FILE *f = fopen(arq_huff, "wb");
        if (f != NULL) {
            fputs("ARQUIVO_INVALIDO_SEM_CABECALHO", f);
            fclose(f);
        }
        int rejeitou_assinatura = !descompactar_arquivo(huffman, arq_huff, arq_out);

        /* Gera um .huff valido e anexa um byte espúrio ao final */
        int rejeitou_bytes_extras = 0;
        if (compactar_arquivo(huffman, arq_in, arq_huff)) {
            f = fopen(arq_huff, "ab");
            if (f != NULL) {
                fputc(0xFF, f);
                fclose(f);
            }
            rejeitou_bytes_extras = !descompactar_arquivo(huffman, arq_huff, arq_out);
        }

        int ok = rejeitou_assinatura && rejeitou_bytes_extras;
        printf("[Caso 5] Rejeicao de .huff invalido ou corrompido     : %s\n",
               ok ? "OK" : "FALHA");
        if (!ok) todos_ok = 0;
    }

    remove(arq_in);
    remove(arq_huff);
    remove(arq_out);

    return todos_ok;
}

static void executar_menu_interativo(Huffman *huffman) {
    char linha[TAM_CAMINHO];
    char entrada[TAM_CAMINHO];
    char saida[TAM_CAMINHO];
    int opcao = -1;

    while (opcao != 0) {
        printf("\n============================================\n");
        printf("   COMPRESSAO DE TEXTOS - ALGORITMO HUFFMAN \n");
        printf("============================================\n");
        printf("1. Compactar arquivo de texto\n");
        printf("2. Descompactar arquivo (.huff)\n");
        printf("3. Testar arquivo (compactar + descompactar + verificar)\n");
        printf("4. Executar bateria de testes (arquivos de teste)\n");
        printf("5. Executar testes de casos de fronteira\n");
        printf("6. Exibir tabela de codigos e arvore da ultima operacao\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            break;
        }
        opcao = atoi(linha);

        switch (opcao) {
            case 1:
                printf("Caminho do arquivo de entrada (.txt): ");
                if (fgets(entrada, sizeof(entrada), stdin) == NULL) break;
                remover_quebra_linha(entrada);

                printf("Caminho do arquivo compactado (.huff): ");
                if (fgets(saida, sizeof(saida), stdin) == NULL) break;
                remover_quebra_linha(saida);

                if (compactar_arquivo(huffman, entrada, saida)) {
                    printf("Compactacao concluida com sucesso!\n");
                    imprimir_resumo_huffman(huffman);
                } else {
                    printf("ERRO ao compactar o arquivo.\n");
                }
                break;

            case 2:
                printf("Caminho do arquivo compactado (.huff): ");
                if (fgets(entrada, sizeof(entrada), stdin) == NULL) break;
                remover_quebra_linha(entrada);

                printf("Caminho do arquivo restaurado (.txt): ");
                if (fgets(saida, sizeof(saida), stdin) == NULL) break;
                remover_quebra_linha(saida);

                if (descompactar_arquivo(huffman, entrada, saida)) {
                    printf("Descompactacao concluida com sucesso! (%lu bytes restaurados)\n",
                           obter_tamanho_original(huffman));
                } else {
                    printf("ERRO ao descompactar o arquivo (arquivo invalido ou inexistente).\n");
                }
                break;

            case 3:
                printf("Caminho do arquivo de texto para teste: ");
                if (fgets(entrada, sizeof(entrada), stdin) == NULL) break;
                remover_quebra_linha(entrada);

                printf("Deseja exibir a tabela de codigos e a arvore? (s/n): ");
                if (fgets(linha, sizeof(linha), stdin) == NULL) break;
                {
                    int exibir = (linha[0] == 's' || linha[0] == 'S');
                    executar_teste_arquivo(huffman, entrada, exibir, exibir);
                }
                break;

            case 4:
                executar_bateria_testes(huffman);
                break;

            case 5:
                executar_testes_casos_especiais(huffman);
                break;

            case 6:
                imprimir_tabela_codigos(huffman);
                printf("\n=== Estrutura da Arvore de Huffman (Pre-Ordem) ===\n");
                imprimir_arvore_do_huffman(huffman);
                break;

            case 0:
                printf("Encerrando programa.\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

static void exibir_ajuda_uso(const char *nome_programa) {
    printf("Uso:\n");
    printf("  %s                          -> Abre o menu interativo\n", nome_programa);
    printf("  %s -c <entrada> <saida>     -> Compacta arquivo de texto\n", nome_programa);
    printf("  %s -d <entrada> <saida>     -> Descompacta arquivo .huff\n", nome_programa);
    printf("  %s -t <entrada>             -> Compacta, descompacta e valida integridade\n", nome_programa);
    printf("  %s -b                       -> Executa bateria de testes nos 5 textos\n", nome_programa);
    printf("  %s -e                       -> Executa testes de casos de fronteira\n", nome_programa);
}

int main(int argc, char *argv[]) {
    Huffman *huffman = criar_huffman();
    if (huffman == NULL) {
        printf("ERRO: falha ao alocar memoria para o TAD Huffman.\n");
        return 1;
    }

    int codigo_saida = 0;

    if (argc == 1) {
        executar_menu_interativo(huffman);
    } else if (strcmp(argv[1], "-c") == 0 && argc == 4) {
        if (compactar_arquivo(huffman, argv[2], argv[3])) {
            printf("Arquivo '%s' compactado com sucesso em '%s'.\n", argv[2], argv[3]);
            imprimir_resumo_huffman(huffman);
        } else {
            printf("ERRO ao compactar '%s'.\n", argv[2]);
            codigo_saida = 1;
        }
    } else if (strcmp(argv[1], "-d") == 0 && argc == 4) {
        if (descompactar_arquivo(huffman, argv[2], argv[3])) {
            printf("Arquivo '%s' descompactado com sucesso em '%s' (%lu bytes).\n",
                   argv[2], argv[3], obter_tamanho_original(huffman));
        } else {
            printf("ERRO ao descompactar '%s'.\n", argv[2]);
            codigo_saida = 1;
        }
    } else if (strcmp(argv[1], "-t") == 0 && argc == 3) {
        if (!executar_teste_arquivo(huffman, argv[2], 0, 0)) {
            codigo_saida = 1;
        }
    } else if (strcmp(argv[1], "-b") == 0 && argc == 2) {
        if (!executar_bateria_testes(huffman)) {
            codigo_saida = 1;
        }
    } else if (strcmp(argv[1], "-e") == 0 && argc == 2) {
        if (!executar_testes_casos_especiais(huffman)) {
            codigo_saida = 1;
        }
    } else {
        exibir_ajuda_uso(argv[0]);
        codigo_saida = 1;
    }

    liberar_huffman(huffman);
    return codigo_saida;
}
