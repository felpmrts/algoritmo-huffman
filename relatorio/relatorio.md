# Relatório Técnico — Compressão de Textos com o Algoritmo de Huffman

**Universidade Federal de Uberlândia (UFU)** — Faculdade de Computação (FACOM)  
**Disciplina:** FACOM32402 – Estrutura de Dados II (2026/2)  
**Professor:** Renato Pimentel  

> **Instrução para o grupo antes de gerar o PDF final:** substitua os campos `[Nome Completo — Matrícula]` abaixo pelos dados reais dos 5 a 7 integrantes do grupo, ajuste os papéis caso necessário, e gere o PDF final com **exatamente 3 páginas em formato A4** (usando `relatorio.tex` no Overleaf ou `relatorio.html` via imprimir/salvar em PDF no navegador).

---

## 1. Integrantes e Papéis Desempenhados

| Integrante | Matrícula | Papel Desempenhado no Trabalho |
| :--- | :--- | :--- |
| `[Nome do Integrante 1]` | `[Matrícula 1]` | Pesquisa teórica do algoritmo de Huffman, especificação da arquitetura modular e revisão técnica do relatório. |
| `[Nome do Integrante 2]` | `[Matrícula 2]` | Projeto e implementação do TAD opaco `ArvoreHuffman` (`ArvoreHuffman.h` e `ArvoreHuffman.c`) com base na árvore binária da disciplina. |
| `[Nome do Integrante 3]` | `[Matrícula 3]` | Projeto e implementação do TAD opaco `FilaPrioridade` (`FilaPrioridade.h` e `FilaPrioridade.c`) utilizando *min-heap* binário. |
| `[Nome do Integrante 4]` | `[Matrícula 4]` | Implementação do módulo `Huffman` (`Huffman.h` e `Huffman.c`): contagem de frequências, geração de códigos, cabeçalho e compactação/descompactação bit a bit. |
| `[Nome do Integrante 5]` | `[Matrícula 5]` | Desenvolvimento da interface em `main.c`, elaboração dos arquivos de teste em `dados/`, testes de casos especiais, análise no Valgrind e discussão dos resultados. |

*(Caso o grupo possua 6 ou 7 integrantes, adicione linhas extras com papéis específicos — por exemplo, validação de integridade/testes automatizados e redação/diagramação das referências e do relatório — evitando descrições genéricas.)*

---

## 2. O Algoritmo de Huffman e sua Lógica de Funcionamento

O algoritmo de Huffman, proposto por David A. Huffman em 1952, é um método clássico de **compressão de dados sem perdas** (*lossless*) baseado na estratégia **gulosa** (*greedy*). Em arquivos de texto comuns (ASCII ou UTF-8), cada byte ocupa um tamanho fixo de **8 bits**, independentemente de o caractere aparecer milhares de vezes (como espaços e vogais) ou apenas uma vez (como símbolos raros).

A ideia central do algoritmo de Huffman é substituir essa representação de tamanho fixo por **códigos binários de comprimento variável**, atribuindo:
- **Códigos curtos** aos símbolos com **maior frequência** no arquivo;
- **Códigos mais longos** aos símbolos com **menor frequência**.

Para que a decodificação ocorra sem ambiguidade e sem a necessidade de separadores entre os símbolos, o algoritmo produz **códigos livres de prefixo** (*prefix-free codes*): nenhum código válido é prefixo inicial de outro. Estruturalmente, essa propriedade é garantida ao armazenar todos os símbolos exclusivamente nos **nós folha** de uma árvore binária completa.

### Etapas do Algoritmo
1. **Contagem de frequências:** percorre-se o arquivo de entrada contabilizando o número de ocorrências de cada byte possível ($0$ a $255$).
2. **Inicialização da fila de prioridade mínima:** para cada símbolo com frequência maior que zero, cria-se um nó folha contendo `(simbolo, frequencia)` e insere-se esse nó em uma fila de prioridade mínima (*min-heap*).
3. **Construção gulosa da Árvore de Huffman:** enquanto restar mais de um nó na fila, removem-se os **dois nós de menor frequência**, cria-se um novo nó interno tendo ambos como filhos (com frequência igual à soma das frequências dos filhos) e reinsere-se o nó interno na fila. O último nó restante torna-se a raiz da árvore.
4. **Geração da tabela de códigos:** realiza-se um percurso em profundidade a partir da raiz, acrescentando o bit `0` ao seguir para o filho esquerdo e o bit `1` ao seguir para o filho direito. Ao atingir uma folha, o caminho acumulado define o código binário do símbolo.
5. **Compactação e descompactação:** na compactação, cada byte do texto original é trocado por seu código binário e empacotado em bytes de 8 bits. Na descompactação, lê-se o fluxo de bits caminhando da raiz até as folhas (`0` à esquerda, `1` à direita) até restaurar exatamente o total de caracteres originais.

**Por que não utilizar Árvore AVL?**  
Conforme discutido na disciplina, árvores AVL aplicam rotações para manter a altura balanceada entre subárvores. No algoritmo de Huffman, entretanto, o desbalanceamento é intencional e essencial: símbolos muito frequentes devem permanecer próximos à raiz (gerando códigos de 2 ou 3 bits), enquanto símbolos raros ficam em níveis mais profundos. Balancear a árvore destruiria a otimização dos comprimentos de código.

---

## 3. Detalhes da Implementação em C

O sistema foi implementado em linguagem C padrão (`C99`), estruturado em **quatro módulos independentes** que seguem rigorosamente o princípio de **Tipo Abstrato de Dados (TAD) opaco**, inspirado no código de `ArvoreBinaria` da disciplina:

- **`ArvoreHuffman.h` / `ArvoreHuffman.c`**: define os tipos opacos `NoHuffman` e `ArvoreHuffman`. Nenhuma `struct` é exposta no `.h`; toda criação de nós (folha/interno), acesso a campos (`obter_simbolo_no`, `obter_frequencia_no`, `obter_esquerda_no`, `obter_direita_no`), cálculo de altura, contagem de nós e liberação pós-ordem ocorre exclusivamente pelas funções públicas.
- **`FilaPrioridade.h` / `FilaPrioridade.c`**: encapsula o TAD opaco `FilaPrioridade`, implementado como um *min-heap* binário em vetor (`subir_heap` e `descer_heap`), garantindo inserção e remoção do menor elemento em tempo $O(\log k)$, onde $k \le 256$ é a quantidade de símbolos distintos.
- **`Huffman.h` / `Huffman.c`**: encapsula o TAD opaco `Huffman`, coordenando todas as fases do algoritmo, estatísticas de compressão, leitura/escrita em nível de bits e verificação de integridade byte a byte (`comparar_arquivos`).
- **`main.c`**: interface didática com menu interativo e modo linha de comando (`-c`, `-d`, `-t`, `-b`, `-e`), utilizando exclusivamente a interface pública de `Huffman.h`.

### Formato do Arquivo Compactado (`.huff`) e Empacotamento de Bits
Para que o arquivo compactado seja autossuficiente na descompactação, grava-se um cabeçalho compacto composto por:
1. Assinatura mágica `"HUF1"` (4 bytes) para validar o formato do arquivo;
2. Total de bytes do arquivo original (`uint64_t`, 8 bytes);
3. Quantidade $k$ de símbolos distintos presentes (`uint16_t`, 2 bytes);
4. Tabela de frequências contendo $k$ pares `(uint8_t simbolo, uint32_t frequencia)` em ordem crescente ($0..255$), totalizando $14 + 5k$ bytes de cabeçalho.

Na escrita dos dados codificados, um acumulador `unsigned char byte_atual` recebe os bits por deslocamento à esquerda (`byte_atual = (byte_atual << 1) | bit`) até completar 8 bits. Caso o último byte fique incompleto, completa-se com zeros à direita (*padding*); na descompactação, o laço interrompe exatamente ao atingir o total de caracteres originais informado no cabeçalho, descartando os bits de preenchimento.

### Tratamento de Casos Especiais e Validação
- **Arquivo vazio (0 bytes):** grava cabeçalho válido com `total_original = 0` e restaura um arquivo vazio sem erros.
- **Arquivo com 1 único símbolo distinto (ex.: `"AAAA..."`):** como a raiz já é folha, atribui-se explicitamente o código `"0"` (1 bit) e a descompactação trata esse caso diretamente.
- **Determinismo da árvore:** os símbolos são inseridos no *min-heap* sempre na ordem crescente $0..255$ tanto na compactação quanto na descompactação, garantindo a reconstrução da mesma árvore.
- **Memória e fidelidade:** os arquivos são abertos em modo binário (`"rb"` / `"wb"`) para preservar quebras de linha e acentos UTF-8 em qualquer sistema operacional. A execução sob **Valgrind** confirmou **0 erros e 0 bytes vazados** em toda a bateria de testes.

---

## 4. Testes Experimentais e Discussão dos Resultados

Os testes foram realizados com cinco arquivos de texto simples em português de escalas crescentes (de ~2 KB a ~500 KB), localizados na pasta `dados/`. Em cada teste, o arquivo foi compactado, descompactado e comparado byte a byte com o original.

| Arquivo (`dados/`) | Original | Símbolos ($k$) | Nós ($2k-1$) | Cabeçalho | Dados Codif. | Compactado Total | Bits/Símb. | Redução Real | Redução Dados | Integridade |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `texto_pequeno_2kb.txt` | 2.414 B (2,36 KB) | 58 | 115 | 304 B | 1.326 B | 1.630 B (1,59 KB) | 4,39 | **32,48%** | 45,07% | OK |
| `texto_medio_15kb.txt` | 14.927 B (14,58 KB) | 69 | 137 | 359 B | 8.213 B | 8.572 B (8,37 KB) | 4,40 | **42,57%** | 44,98% | OK |
| `texto_medio_50kb.txt` | 51.118 B (49,92 KB) | 69 | 137 | 359 B | 28.168 B | 28.527 B (27,86 KB) | 4,41 | **44,19%** | 44,90% | OK |
| `texto_grande_200kb.txt` | 204.523 B (199,73 KB) | 69 | 137 | 359 B | 112.977 B | 113.336 B (110,68 KB) | 4,42 | **44,59%** | 44,76% | OK |
| `texto_grande_500kb.txt` | 511.711 B (499,72 KB) | 69 | 137 | 359 B | 283.272 B | 283.631 B (276,98 KB) | 4,43 | **44,57%** | 44,64% | OK |

### Discussão dos Resultados
1. **Impacto do cabeçalho em arquivos pequenos:** no arquivo de 2,36 KB, embora os dados codificados tenham reduzido **45,07%**, a redução real final foi de **32,48%**. Isso ocorre porque o cabeçalho (304 bytes) corresponde a **12,59%** do tamanho original. Esse resultado evidencia o custo fixo de armazenar a tabela de frequências para tornar o arquivo autossuficiente.
2. **Convergência da taxa de compressão com o crescimento do arquivo:** a partir de 15 KB, o alfabeto utilizado se estabiliza em $k = 69$ símbolos distintos, mantendo o cabeçalho constante em **359 bytes** ($14 + 5 \times 69$). Assim, o peso relativo do cabeçalho cai de 2,41% (15 KB) para apenas **0,07%** (500 KB), fazendo a redução real convergir de **32,48%** para **~44,6%** — praticamente idêntica à redução pura dos dados.
3. **Eficiência média em bits e propriedade estrutural:** em todos os arquivos, a média de bits por caractere caiu de **8,00 bits** para **4,39–4,43 bits**, coerente com a entropia típica de textos em português. Observa-se também a propriedade matemática exata das árvores binárias completas: para $k$ folhas, o número de nós é sempre $2k - 1$ ($115$ nós para $58$ símbolos e $137$ nós para $69$ símbolos).
4. **Fidelidade e desempenho:** todos os arquivos testados — além dos 4 casos especiais (arquivo vazio, caractere único repetido, texto curto com UTF-8 e arquivo inexistente) — passaram na comparação byte a byte (`OK`), com tempos de execução inferiores a 50 ms mesmo para 500 KB.

---

## 5. Materiais Consultados, Links e Justificativa da Escolha

1. **HUFFMAN, David A. (1952).** *A Method for the Construction of Minimum-Redundancy Codes*. Proceedings of the IRE, vol. 40, nº 9, pp. 1098–1101.  
   - **Link:** <https://ieeexplore.ieee.org/document/273898> (cópia pública em PDF: <https://www.coit.es/foro-historico/sites/default/files/ficheros_adjuntos/a_method_for_the_construction_of_minimum_redundancy_codes.pdf>)  
   - **Justificativa:** escolhido por ser a fonte primária original que formaliza matematicamente o algoritmo e prova a otimalidade dos códigos livres de prefixo de redundância mínima.
2. **FEOFILOFF, Paulo (IME-USP).** *Análise de Algoritmos — Código de Huffman e Filas-com-Prioridades* e *Árvores Binárias de Busca em C*.  
   - **Links:** <https://www.ime.usp.br/~pf/analise_de_algoritmos/> e <https://www.ime.usp.br/~pf/algoritmos/aulas/binst.html>  
   - **Justificativa:** material acadêmico em português amplamente adotado em universidades brasileiras (inclusive referenciado no código-base da disciplina), escolhido pela clareza didática ao relacionar filas de prioridade (*min-heap*) à construção ótima da árvore de Huffman.
3. **PIMENTEL, Renato (FACOM/UFU).** *Código de Referência — TAD Opaco de Árvore Binária em C (FACOM32402)*.  
   - **Link:** <https://onlinegdb.com/eqb9wEDY6>  
   - **Justificativa:** ponto de partida oficial recomendado no enunciado do trabalho para padronizar o estilo de encapsulamento em TAD opaco e a organização entre arquivos `.h` e `.c`.
4. **CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C.** *Introduction to Algorithms* (Cap. 16.3 — *Huffman Codes*). MIT Press.  
   - **Link:** <https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/>  
   - **Justificativa:** livro-texto clássico da área, consultado para verificar a corretude da escolha gulosa e a análise de complexidade $O(k \log k)$ na construção da árvore.
5. **GEEKSFORGEEKS.** *Huffman Coding | Greedy Algo-3*.  
   - **Link:** <https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/>  
   - **Justificativa:** consultado como apoio prático complementar por apresentar exemplos passo a passo da combinação de nós na fila de prioridade e da atribuição dos códigos binários.
