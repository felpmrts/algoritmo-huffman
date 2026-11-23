# Relatório Técnico — Compressão de Textos com o Algoritmo de Huffman

**Universidade Federal de Uberlândia (UFU)** — Faculdade de Computação (FACOM)  
**Disciplina:** FACOM32402 – Estrutura de Dados II (2026/2)  
**Professor:** Renato Pimentel  

---

## 1. Integrantes do Grupo e Papéis Desempenhados

| Integrante | Papel Desempenhado no Desenvolvimento |
| :--- | :--- |
| **Felipe César Martins** | Arquitetura inicial do repositório, integração entre os TADs opacos e implementação do controle de compactação e descompactação em `Huffman.h/.c`. |
| **Rhyan Videira Barros Guidastre** | Revisão de conformidade com os requisitos, auditoria de memória (Valgrind), curadoria do corpus literário real e redação final do relatório. |
| **Renan Cesar Vieira Mallagoli** | Projeto e implementação do TAD opaco `ArvoreHuffman` (`ArvoreHuffman.h/.c`), adaptado da árvore binária apresentada em aula. |
| **Arthur Souza Moura** | Projeto e implementação do TAD opaco `FilaPrioridade` (`FilaPrioridade.h/.c`) baseado em *min-heap* binário com ordenação canônica determinística. |
| **Hugo Vieira Pereira Rocha** | Especificação do formato binário do cabeçalho (`.huff`), rotinas de empacotamento/leitura bit a bit e validação estrita de integridade. |
| **Sofia Oliveira Ferreira da Silva Cruz** | Desenvolvimento da interface interativa e de linha de comando em `main.c`, execução da bateria experimental e análise de entropia e taxas de compressão. |
| **Kaio Antunes Silveira** | Implementação da geração recursiva da tabela de códigos livres de prefixo e validação dos casos de fronteira (arquivo vazio, símbolo único e corrupção). |

---

## 2. O Algoritmo de Huffman e sua Lógica de Funcionamento

O algoritmo de Huffman, proposto por David A. Huffman em 1952, é um método clássico de **compressão de dados sem perdas** (*lossless*) fundamentado na estratégia **gulosa** (*greedy*). Em arquivos de texto comuns (ASCII ou UTF-8), cada byte ocupa um tamanho fixo de **8 bits**, independentemente de ocorrer milhares de vezes (como espaços e vogais) ou apenas raramente (como letras maiúsculas específicas e sinais gráficos).

A ideia central de Huffman consiste em substituir a representação fixa por **códigos binários de comprimento variável**: símbolos mais frequentes recebem sequências curtas de bits, enquanto símbolos menos frequentes recebem sequências mais longas. Para garantir decodificação sem ambiguidade e sem caracteres delimitadores, o algoritmo constrói **códigos livres de prefixo** (*prefix-free*): nenhum código válido é prefixo inicial de outro. Estruturalmente, essa propriedade é assegurada ao posicionar todos os símbolos exclusivamente nos **nós folha** de uma árvore binária completa.

### Etapas do Algoritmo
1. **Contagem de frequências:** percorre-se o arquivo de entrada contabilizando o número de ocorrências de cada valor de byte possível ($0$ a $255$).
2. **Inicialização da fila de prioridade mínima:** para cada símbolo com frequência maior que zero, cria-se um nó folha contendo `(simbolo, frequencia)` e insere-se esse nó em um *min-heap*.
3. **Construção gulosa da Árvore de Huffman:** enquanto restar mais de um nó na fila, removem-se os dois nós de menor frequência, cria-se um nó interno pai com frequência igual à soma dos filhos e reinsere-se o pai na fila. O último nó restante torna-se a raiz da árvore.
4. **Geração da tabela de códigos:** em um percurso em profundidade a partir da raiz, cada ramo esquerdo acrescenta o bit `0` e cada ramo direito acrescenta o bit `1`; ao atingir uma folha, registra-se o código binário definitivo daquele símbolo.
5. **Compactação e descompactação:** cada byte do texto original é substituído por seu código e empacotado em bytes de 8 bits. Na descompactação, lê-se o fluxo bit a bit caminhando da raiz às folhas (`0` à esquerda, `1` à direita) até restaurar exatamente o total de bytes originais.

Segundo o Teorema de Shannon para codificação de fonte, o comprimento médio $\bar{L}$ em bits por símbolo de um código ótimo livre de prefixo satisfaz $H(X) \le \bar{L} < H(X) + 1$, em que $H(X) = -\sum_i p_i \log_2(p_i)$ é a entropia da fonte.

**Por que não utilizar Árvore AVL?**  
Árvores AVL realizam rotações para manter a altura balanceada entre suas subárvores. No algoritmo de Huffman, o desbalanceamento é intencional e indispensável: símbolos muito frequentes devem permanecer próximos à raiz (gerando códigos curtos de 2 a 4 bits), enquanto símbolos raros ficam em níveis profundos. Aplicar rotações de balanceamento alteraria as profundidades das folhas e destruiria a otimalidade do comprimento médio dos códigos.

---

## 3. Detalhes da Implementação em Linguagem C

O sistema foi desenvolvido em linguagem C padrão (`C99`), compilado sem avisos sob `-Wall -Wextra -pedantic -std=c99` e estruturado em quatro módulos independentes. Tomando como ponto de partida a estrutura de dados de árvore binária vista no curso (`ArvoreBinaria`), todos os dados internos foram protegidos por **Tipos Abstratos de Dados (TADs) opacos**: nenhuma definição de `struct` é exposta nos cabeçalhos (`.h`), restringindo qualquer manipulação às funções públicas previstas.

### 3.1 Arquitetura Modular e Encapsulamento em TADs Opacos
- **`ArvoreHuffman.h/.c`**: declara os tipos opacos `NoHuffman` e `ArvoreHuffman`. Disponibiliza funções para criar nós folha e internos, consultar símbolo, frequência e filhos, comparar nós (`comparar_nos_huffman`) e gerar os códigos diretamente da árvore (`gerar_codigos_arvore_huffman`), além de calcular altura, contar nós ($2k - 1$), imprimir em pré-ordem e liberar memória em pós-ordem.
- **`FilaPrioridade.h/.c`**: encapsula o TAD opaco `FilaPrioridade`, implementado como um *min-heap* binário em vetor com as rotinas internas `subir_heap` e `descer_heap` baseadas em `comparar_nos_huffman`. Para um índice $i$, o pai está em $(i-1)/2$ e os filhos em $2i+1$ e $2i+2$, garantindo inserção e remoção em tempo $O(\log k)$, com $k \le 256$.
- **`Huffman.h/.c`**: encapsula o TAD opaco `Huffman` (frequências, ponteiro para `ArvoreHuffman`, tabela de códigos e métricas). Abre os textos originais e restaurados em **modo texto** (`"r"`/`"w"`) e o arquivo compactado em modo binário (`"rb"`/`"wb"`). Calcula também a entropia de Shannon $H(X)$ via `obter_entropia_shannon` em C99 puro, sem dependência externa da biblioteca `-lm`.
- **`main.c`**: fornece menu interativo e interface de linha de comando (`-c`, `-d`, `-t`, `-b`, `-e`), utilizando exclusivamente as funções públicas de `Huffman.h`. Localiza automaticamente os arquivos de teste tanto em `dados/` quanto no diretório raiz.

### 3.2 Formato do Cabeçalho Binário (`.huff`) e Manipulação de Bits
Para que o arquivo compactado seja autossuficiente na descompactação, grava-se um cabeçalho binário compacto de $14 + 5k$ bytes antes dos dados codificados:

| Campo do Cabeçalho | Tipo / Tamanho | Finalidade na Descompactação |
| :--- | :---: | :--- |
| Assinatura mágica | `"HUF1"` (4 bytes) | Identifica e valida se o arquivo foi gerado pelo programa. |
| Total original | `uint64_t` (8 bytes) | Quantidade exata de bytes do texto original (controla o fim da leitura e o *padding*). |
| Símbolos distintos ($k$) | `uint16_t` (2 bytes) | Quantidade de entradas ($0$ a $256$) gravadas na tabela de frequências. |
| Tabela de frequências | $k \times (1 + 4\text{ bytes})$ | Pares `(uint8_t simbolo, uint32_t frequencia)` em ordem crescente ($0..255$). |

Na escrita dos dados codificados, cada bit é inserido em um acumulador `unsigned char` por deslocamento à esquerda: `byte_atual = (byte_atual << 1) | bit`. Ao completar 8 bits, grava-se o byte via `fputc`. Se restarem bits pendentes ao final ($< 8$ bits), aplica-se deslocamento à esquerda (`byte_atual << (8 - bits_no_byte)`) preenchendo o restante com zeros (*padding*). Na descompactação, os bits são lidos da posição 7 até 0 via `(byte_lido >> pos_bit) & 1`, percorrendo a árvore até que o total decodificado iguale `tamanho_original`.

### 3.3 Casos de Fronteira, Determinismo Canônico e Validação de Memória
- **Arquivo vazio (0 bytes):** grava um cabeçalho de 14 bytes com `total_original = 0` e $k = 0$, validando ausência de dados excedentes e restaurando um arquivo vazio idêntico.
- **Arquivo com 1 único símbolo distinto (ex.: `"AAA..."`):** como a raiz já é folha, atribui-se o código `"0"` (1 bit/ocorrência); na descompactação, qualquer bit igual a `1` é rejeitado como erro de integridade.
- **Determinismo canônico no *min-heap*:** a função `comparar_nos_huffman` desempata frequências iguais priorizando nós folha antes de nós internos e, em seguida, o menor byte (`menor_simbolo`) da subárvore. Assim, a árvore gerada é única e idêntica tanto na compactação quanto na reconstrução.
- **Validação estrita de *padding* e memória:** ao concluir a decodificação, o programa verifica se os bits residuais do último byte são nulos e se o fluxo atingiu `EOF` sem bytes espúrios. A auditoria via **Valgrind** (`--leak-check=full`) atestou **0 erros e 0 bytes vazados** nas 2.725 alocações da bateria de testes.

---

## 4. Testes Experimentais e Discussão dos Resultados

Os experimentos foram realizados utilizando cinco obras completas em língua portuguesa de Machado de Assis (em domínio público, UTF-8), cobrindo escalas de alguns KB (~3,2 KB), dezenas de KB (~16,1 KB e ~49,8 KB) e centenas de KB (~202,1 KB e ~447,8 KB). Em todos os testes, o arquivo compactado (`.huff`) foi descompactado e validado byte a byte contra o original.

| Arquivo de Entrada | Obra Literária | Original | $k$ | Nós | Cabeç. | Compact. | $H$ (bits) | $\bar{L}$ (bits) | Red. Real | Red. Dad. | Int. |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `texto_pequeno_2kb.txt` | Carta à Imprensa | 3.282 B | 71 | 141 | 369 B | 2.255 B | 4,5590 | 4,5969 | **31,29%** | 42,54% | OK |
| `texto_medio_15kb.txt` | Castro Alves | 16.453 B | 75 | 149 | 389 B | 9.730 B | 4,5048 | 4,5418 | **40,86%** | 43,23% | OK |
| `texto_medio_50kb.txt` | As Forcas Caudinas | 50.976 B | 87 | 173 | 449 B | 32.556 B | 5,0140 | 5,0387 | **36,13%** | 37,02% | OK |
| `texto_grande_200kb.txt` | A Mão e a Luva | 207.000 B | 102 | 203 | 524 B | 118.563 B | 4,5305 | 4,5619 | **42,72%** | 42,98% | OK |
| `texto_grande_500kb.txt` | Quincas Borba | 458.506 B | 103 | 205 | 529 B | 266.765 B | 4,6102 | 4,6453 | **41,82%** | 41,93% | OK |

### Discussão dos Resultados Observados
- **Impacto do cabeçalho conforme a escala do arquivo:** no texto pequeno de 3.282 B (*Carta à Imprensa Acadêmica*), os dados codificados reduziram **42,54%**, porém a redução real caiu para **31,29%** porque o cabeçalho (369 B) equivale a **11,24%** do original. Nos romances de 207 KB e 458 KB, o cabeçalho de 524–529 B cai para apenas **0,25%** e **0,12%** do total, fazendo a redução real convergir para a redução pura dos dados (**42,72%** e **41,82%**).
- **Influência do gênero textual na entropia e na compactação:** a amostra de ~50 KB (*As Forcas Caudinas*) apresentou entropia maior ($H = 5{,}0140$ bits) e redução real de **36,13%**, abaixo da prosa corrida ($H \approx 4{,}50\text{--}4{,}61$ e redução de 40,8%–42,7%). Como se trata de uma **peça teatral**, há recorrência de nomes em caixa-alta (`SEABRA`, `MARGARIDA`, `TITO`), travessões e rubricas cênicas, tornando a distribuição de frequências menos concentrada.
- **Proximidade do limite teórico de Shannon e estrutura da árvore:** em todas as cinco obras verifica-se o Teorema de Shannon ($H(X) \le \bar{L} < H(X) + 1$), com o comprimento médio $\bar{L}$ ficando a menos de **0,038 bits/símbolo** acima da entropia $H$ (eficiência de codificação superior a **99,1%**). Além disso, todas as árvores respeitam a identidade exata de árvore binária estritamente completa ($2k - 1$ nós) e restauração 100% fiel.

---

## 5. Links dos Textos Utilizados, Referências e Justificativa da Escolha

### A) Textos de Entrada Utilizados nos Testes (Obras de Machado de Assis — Domínio Público / MEC)
- **Acervo oficial (MEC) e repositório dos arquivos `.txt`:** <http://machado.mec.gov.br/> e <https://github.com/luxedo/dataset-machado-de-assis>.
- **Links diretos dos 5 textos:**
  1. `texto_pequeno_2kb.txt`: <https://raw.githubusercontent.com/luxedo/dataset-machado-de-assis/master/txt/miscel%C3%A2nea/cartaImprensa.txt>
  2. `texto_medio_15kb.txt`: <https://raw.githubusercontent.com/luxedo/dataset-machado-de-assis/master/txt/cr%C3%ADtica/castroAlves.txt>
  3. `texto_medio_50kb.txt`: <https://raw.githubusercontent.com/luxedo/dataset-machado-de-assis/master/txt/teatro/forcas.txt>
  4. `texto_grande_200kb.txt`: <https://raw.githubusercontent.com/luxedo/dataset-machado-de-assis/master/txt/romance/maoLuva.txt>
  5. `texto_grande_500kb.txt`: <https://raw.githubusercontent.com/luxedo/dataset-machado-de-assis/master/txt/romance/quincas.txt>
- **Justificativa da escolha dos textos:** selecionaram-se obras integrais e reais em português cobrindo tamanhos naturais de ~3 KB a ~458 KB para avaliar simultaneamente a diluição do cabeçalho com a escala e o impacto de diferentes gêneros (carta, crítica, teatro e romances) sobre a entropia e o desempenho de Huffman.

### B) Referências Teóricas e Técnicas Consultadas
1. **HUFFMAN, D. A. (1952).** *A Method for the Construction of Minimum-Redundancy Codes*. Proceedings of the IRE, 40(9), 1098–1101. Link: <https://ieeexplore.ieee.org/document/273898>. *Justificativa:* artigo original que formaliza o algoritmo e prova a otimalidade dos códigos livres de prefixo.
2. **PIMENTEL, R. (FACOM/UFU).** *Implementação de Árvore Binária em C (TAD Opaco)*. Link: <https://onlinegdb.com/eqb9wEDY6>. *Justificativa:* código de partida indicado no enunciado da disciplina para estruturar o encapsulamento dos TADs opacos.
3. **FEOFILOFF, P. (IME-USP).** *Análise de Algoritmos: Código de Huffman e Filas de Prioridade*. Link: <https://www.ime.usp.br/~pf/analise_de_algoritmos/>. *Justificativa:* referência acadêmica em português escolhida para fundamentar a construção da árvore de Huffman sobre um *min-heap* binário.
4. **CORMEN, T. H. et al.** *Algoritmos: Teoria e Prática* (Cap. 16.3 — Códigos de Huffman). Link: <https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/>. *Justificativa:* utilizado para validar a propriedade da escolha gulosa e a complexidade $O(k \log k)$ de montagem da árvore.
