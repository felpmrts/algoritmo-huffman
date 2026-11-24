#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Huffman.h"

#define TAM_CAMINHO 256

// Exibe o prompt, le uma linha da entrada padrao e remove '\n' ou '\r' do final
static int ler_linha(const char *prompt, char *dest, int tam) {
    if (prompt != NULL) printf("%s", prompt);
    if (fgets(dest, tam, stdin) == NULL) return 0;

    size_t len = strlen(dest);
    while (len > 0 && (dest[len - 1] == '\n' || dest[len - 1] == '\r'))
        dest[--len] = '\0';

    return 1;
}

// Localiza o arquivo em "dados/" ou no diretorio atual; retorna NULL se ausente
static const char *resolver_caminho_arquivo(const char *caminho_padrao, const char *nome_curto) {
    FILE *f = fopen(caminho_padrao, "r");
    if (f != NULL) {
        fclose(f);
        return caminho_padrao;
    }
    f = fopen(nome_curto, "r");
    if (f != NULL) {
        fclose(f);
        return nome_curto;
    }
    return NULL;
}

// Compacta, descompacta, mede tempos e confere integridade em modo texto
static int executar_teste_arquivo(Huffman *huffman,
                                  const char *caminho_entrada,
                                  int exibir_detalhes) {
    const char *arq_huff = "temp_teste.huff";
    const char *arq_rest = "temp_restaurado.txt";

    printf("\nArquivo de teste: %s\n", caminho_entrada);

    clock_t ini_comp = clock();
    if (!compactar_arquivo(huffman, caminho_entrada, arq_huff)) {
        printf("ERRO: nao foi possivel compactar '%s'.\n", caminho_entrada);
        return 0;
    }
    clock_t fim_comp = clock();

    if (exibir_detalhes) {
        printf("\nArvore de Huffman (Pre-Ordem):\n");
        imprimir_arvore_do_huffman(huffman);
        imprimir_tabela_codigos(huffman);
    }
    imprimir_resumo_huffman(huffman);

    clock_t ini_desc = clock();
    if (!descompactar_arquivo(huffman, arq_huff, arq_rest)) {
        printf("ERRO: nao foi possivel descompactar '%s'.\n", arq_huff);
        remove(arq_huff);
        return 0;
    }
    clock_t fim_desc = clock();

    int integro = comparar_arquivos(caminho_entrada, arq_rest);
    printf("  Tempo comp. / descomp.     : %.2f ms / %.2f ms\n",
           ((double) (fim_comp - ini_comp) / CLOCKS_PER_SEC) * 1000.0,
           ((double) (fim_desc - ini_desc) / CLOCKS_PER_SEC) * 1000.0);
    printf("  Verificacao de integridade : %s\n", integro ? "OK (identico ao original)" : "FALHA");

    remove(arq_huff);
    remove(arq_rest);
    return integro;
}

// Executa a bateria comparativa nos 5 arquivos de texto de tamanhos crescentes
static int executar_bateria_testes(Huffman *huffman) {
    const char *caminhos[] = {
        "dados/texto_pequeno_2kb.txt",
        "dados/texto_medio_15kb.txt",
        "dados/texto_medio_50kb.txt",
        "dados/texto_grande_200kb.txt",
        "dados/texto_grande_500kb.txt"
    };
    const char *nomes[] = {
        "texto_pequeno_2kb.txt",
        "texto_medio_15kb.txt",
        "texto_medio_50kb.txt",
        "texto_grande_200kb.txt",
        "texto_grande_500kb.txt"
    };
    const int total = (int) (sizeof(caminhos) / sizeof(caminhos[0]));
    const char *arq_huff = "temp_bateria.huff";
    const char *arq_rest = "temp_bateria.txt";

    printf("\nBateria de testes - Compressao de Huffman por tamanho:\n");
    printf("%-25s | %9s | %4s | %4s | %7s | %9s | %8s | %8s | %8s | %8s | %s\n",
           "Arquivo", "Original", "Simb", "Nos", "Cabec.", "Compact.",
           "Entropia", "Bits/S", "Red.Real", "Red.Dado", "Integ");
    printf("--------------------------+-----------+------+------+---------+-----------+----------+----------+----------+----------+------\n");

    int todos_ok = 1;
    int executados = 0;
    for (int i = 0; i < total; i++) {
        const char *caminho = resolver_caminho_arquivo(caminhos[i], nomes[i]);
        if (caminho == NULL) continue;

        executados++;
        if (!compactar_arquivo(huffman, caminho, arq_huff)) {
            printf("%-25s | ERRO ao abrir ou compactar\n", caminho);
            todos_ok = 0;
            continue;
        }

        int desc_ok = descompactar_arquivo(huffman, arq_huff, arq_rest);
        int integro = desc_ok && comparar_arquivos(caminho, arq_rest);
        if (!integro) todos_ok = 0;

        printf("%-25s | %7lu B | %4d | %4d | %5lu B | %7lu B | %8.4f | %8.4f | %7.2f%% | %7.2f%% | %s\n",
               nomes[i],
               obter_tamanho_original(huffman),
               obter_simbolos_distintos(huffman),
               obter_total_nos_arvore(huffman),
               obter_bytes_cabecalho(huffman),
               obter_tamanho_compactado(huffman),
               obter_entropia_shannon(huffman),
               obter_media_bits_por_simbolo(huffman),
               obter_taxa_reducao(huffman),
               obter_taxa_reducao_dados(huffman),
               integro ? "OK" : "ERRO");
    }

    if (executados == 0) {
        printf("Nenhum arquivo de teste encontrado no diretorio atual ou em dados/.\n");
        todos_ok = 0;
    } else if (executados < total) {
        printf("(%d de %d textos do corpus presentes no diretorio e validados)\n",
               executados, total);
    }

    remove(arq_huff);
    remove(arq_rest);
    return todos_ok;
}

// Grava texto em arquivo temporario, compacta, descompacta e valida integridade
static int testar_texto_temporario(Huffman *huffman, const char *texto) {
    FILE *f = fopen("temp_especial_in.txt", "w");
    if (f == NULL) return 0;
    fputs(texto, f);
    fclose(f);

    return compactar_arquivo(huffman, "temp_especial_in.txt", "temp_especial.huff") &&
           descompactar_arquivo(huffman, "temp_especial.huff", "temp_especial_out.txt") &&
           comparar_arquivos("temp_especial_in.txt", "temp_especial_out.txt");
}

// Valida casos de fronteira: vazio, 1 simbolo, UTF-8, inexistente e .huff corrompido
static int executar_testes_casos_especiais(Huffman *huffman) {
    const char *arq_in = "temp_especial_in.txt";
    const char *arq_huff = "temp_especial.huff";
    const char *arq_out = "temp_especial_out.txt";
    int todos_ok = 1;

    printf("\nTestes de casos de fronteira:\n");

    int ok1 = testar_texto_temporario(huffman, "");
    printf("  [Caso 1] Arquivo vazio (0 bytes)                  : %s\n", ok1 ? "OK" : "FALHA");

    char repetido[101];
    memset(repetido, 'A', 100);
    repetido[100] = '\0';
    int ok2 = testar_texto_temporario(huffman, repetido);
    printf("  [Caso 2] Unico simbolo repetido ('A' x 100)       : %s\n", ok2 ? "OK" : "FALHA");

    int ok3 = testar_texto_temporario(
        huffman,
        "Estrutura de Dados II - Compressão com Huffman!\n"
        "Acentuação: ação, coração, árvore, frequência, código.\n");
    printf("  [Caso 3] Texto curto com acentuacao UTF-8         : %s\n", ok3 ? "OK" : "FALHA");

    int ok4 = !compactar_arquivo(huffman, "arquivo_que_nao_existe.txt", arq_huff);
    printf("  [Caso 4] Tratamento de arquivo inexistente        : %s\n", ok4 ? "OK" : "FALHA");

    // Caso 5: assinatura invalida e arquivo .huff valido com byte espurio ao final
    FILE *f = fopen(arq_huff, "wb");
    if (f != NULL) {
        fputs("ARQUIVO_INVALIDO_SEM_CABECALHO", f);
        fclose(f);
    }
    int rejeitou_assinatura = !descompactar_arquivo(huffman, arq_huff, arq_out);
    int rejeitou_extra = 0;
    if (compactar_arquivo(huffman, arq_in, arq_huff)) {
        f = fopen(arq_huff, "ab");
        if (f != NULL) {
            fputc(0xFF, f);
            fclose(f);
        }
        rejeitou_extra = !descompactar_arquivo(huffman, arq_huff, arq_out);
    }
    int ok5 = rejeitou_assinatura && rejeitou_extra;
    printf("  [Caso 5] Rejeicao de .huff invalido/corrompido    : %s\n", ok5 ? "OK" : "FALHA");

    todos_ok = ok1 && ok2 && ok3 && ok4 && ok5;

    remove(arq_in);
    remove(arq_huff);
    remove(arq_out);
    return todos_ok;
}

// Executa o menu interativo no terminal para operar o compressor
static void executar_menu_interativo(Huffman *huffman) {
    char linha[TAM_CAMINHO], entrada[TAM_CAMINHO], saida[TAM_CAMINHO];
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== Compressao de Textos - Algoritmo de Huffman ===\n");
        printf("1. Compactar arquivo de texto\n");
        printf("2. Descompactar arquivo (.huff)\n");
        printf("3. Testar arquivo (compactar + descompactar + verificar)\n");
        printf("4. Executar bateria de testes (textos disponiveis)\n");
        printf("5. Executar testes de casos de fronteira\n");
        printf("6. Exibir tabela de codigos e arvore da ultima operacao\n");
        printf("0. Sair\n");

        if (!ler_linha("Escolha uma opcao: ", linha, sizeof(linha))) break;
        opcao = atoi(linha);

        switch (opcao) {
            case 1:
                if (!ler_linha("Entrada (.txt): ", entrada, sizeof(entrada))) break;
                if (!ler_linha("Saida (.huff) : ", saida, sizeof(saida))) break;
                if (compactar_arquivo(huffman, entrada, saida)) {
                    printf("Compactacao concluida!\n");
                    imprimir_resumo_huffman(huffman);
                } else {
                    printf("ERRO ao compactar o arquivo.\n");
                }
                break;

            case 2:
                if (!ler_linha("Entrada (.huff): ", entrada, sizeof(entrada))) break;
                if (!ler_linha("Saida (.txt)   : ", saida, sizeof(saida))) break;
                if (descompactar_arquivo(huffman, entrada, saida))
                    printf("Descompactacao concluida (%lu bytes restaurados).\n",
                           obter_tamanho_original(huffman));
                else
                    printf("ERRO ao descompactar (arquivo invalido ou inexistente).\n");
                break;

            case 3:
                if (!ler_linha("Arquivo de texto: ", entrada, sizeof(entrada))) break;
                if (!ler_linha("Exibir tabela e arvore? (s/n): ", linha, sizeof(linha))) break;
                executar_teste_arquivo(huffman, entrada, (linha[0] == 's' || linha[0] == 'S'));
                break;

            case 4:
                executar_bateria_testes(huffman);
                break;

            case 5:
                executar_testes_casos_especiais(huffman);
                break;

            case 6:
                imprimir_tabela_codigos(huffman);
                printf("\nArvore de Huffman (Pre-Ordem):\n");
                imprimir_arvore_do_huffman(huffman);
                break;

            case 0:
                printf("Encerrando programa.\n");
                break;

            default:
                printf("Opcao invalida.\n");
                break;
        }
    }
}

// Mostra os parametros aceitos via linha de comando
static void exibir_ajuda_uso(const char *nome) {
    printf("Uso:\n");
    printf("  %s                      -> Menu interativo\n", nome);
    printf("  %s -c <in> <out>        -> Compactar arquivo de texto\n", nome);
    printf("  %s -d <in> <out>        -> Descompactar arquivo .huff\n", nome);
    printf("  %s -t <entrada>         -> Testar compactacao e descompactacao\n", nome);
    printf("  %s -b                   -> Bateria de testes nos textos disponiveis\n", nome);
    printf("  %s -e                   -> Testes de casos de fronteira\n", nome);
}

// Ponto de entrada: cria o TAD Huffman, executa o modo escolhido e libera a memoria
int main(int argc, char *argv[]) {
    Huffman *huffman = criar_huffman();
    if (huffman == NULL) {
        printf("ERRO: falha ao alocar memoria para o TAD Huffman.\n");
        return 1;
    }

    int erro = 0;

    if (argc == 1) {
        executar_menu_interativo(huffman);
    } else if (strcmp(argv[1], "-c") == 0 && argc == 4) {
        if (compactar_arquivo(huffman, argv[2], argv[3])) {
            printf("Arquivo '%s' compactado em '%s'.\n", argv[2], argv[3]);
            imprimir_resumo_huffman(huffman);
        } else {
            printf("ERRO ao compactar '%s'.\n", argv[2]);
            erro = 1;
        }
    } else if (strcmp(argv[1], "-d") == 0 && argc == 4) {
        if (descompactar_arquivo(huffman, argv[2], argv[3]))
            printf("Arquivo '%s' descompactado em '%s' (%lu bytes).\n",
                   argv[2], argv[3], obter_tamanho_original(huffman));
        else {
            printf("ERRO ao descompactar '%s'.\n", argv[2]);
            erro = 1;
        }
    } else if (strcmp(argv[1], "-t") == 0 && argc == 3) {
        erro = !executar_teste_arquivo(huffman, argv[2], 0);
    } else if (strcmp(argv[1], "-b") == 0 && argc == 2) {
        erro = !executar_bateria_testes(huffman);
    } else if (strcmp(argv[1], "-e") == 0 && argc == 2) {
        erro = !executar_testes_casos_especiais(huffman);
    } else {
        exibir_ajuda_uso(argv[0]);
        erro = 1;
    }

    liberar_huffman(huffman);
    return erro;
}
