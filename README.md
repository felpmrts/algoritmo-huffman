# Compressão de Textos com o Algoritmo de Huffman

Trabalho da disciplina **FACOM32402 – Estrutura de Dados II (UFU / FACOM — 2026/2)**, ministrada pelo **Prof. Renato Pimentel**.

Implementação didática, modular e em português do algoritmo de compressão de Huffman em linguagem C, utilizando **Tipos Abstratos de Dados (TADs) opacos** inspirados na estrutura de `ArvoreBinaria` apresentada em aula.

---

## Estrutura do Projeto

```text
algoritmo-huffman/
├── src/                        # Código-fonte em C (.c e .h) com includes planos
│   ├── ArvoreHuffman.h / .c    # TAD opaco da Árvore de Huffman (nós folha, internos e consultas)
│   ├── FilaPrioridade.h / .c   # TAD opaco da Fila de Prioridade Mínima (Min-Heap em vetor)
│   ├── Huffman.h / .c          # TAD opaco principal (frequências, códigos, cabeçalho, bits, métricas)
│   └── main.c                  # Interface interativa, linha de comando e bateria de testes
├── dados/                      # Arquivos de texto simples (.txt) de diferentes tamanhos
│   ├── texto_pequeno_2kb.txt   # ~2,4 KB  (ordem de alguns KB)
│   ├── texto_medio_15kb.txt    # ~14,6 KB (ordem de dezenas de KB)
│   ├── texto_medio_50kb.txt    # ~49,9 KB (ordem de dezenas de KB)
│   ├── texto_grande_200kb.txt  # ~199,7 KB (ordem de centenas de KB)
│   └── texto_grande_500kb.txt  # ~499,7 KB (ordem de centenas de KB)
├── relatorio/                  # Relatório técnico estruturado para exatamente 3 páginas A4
│   ├── relatorio.md            # Versão editável em Markdown
│   ├── relatorio.tex           # Versão LaTeX (pronta para compilar no Overleaf)
│   └── relatorio.html          # Versão HTML (abrir no navegador e imprimir -> Salvar como PDF)
├── contexto_regras/            # Enunciado, rubrica e código-base de referência do professor
├── Makefile                    # Atalhos para compilar, testar e checar memória com Valgrind
└── README.md
```

---

## Como Compilar e Executar

### Usando `make`
```bash
# Compilar o projeto
make

# Executar menu interativo
./huffman

# Executar testes de casos especiais + bateria completa da pasta dados/
make testar

# Verificar ausência de vazamentos de memória (Valgrind)
make valgrind

# Remover binário e arquivos temporários
make limpar
```

### Compilação direta com `gcc` (sem `Makefile`)
Como os `#include` em `src/` são planos, os arquivos `.c` e `.h` compilam diretamente mesmo quando colocados todos juntos em uma única pasta (como na entrega do Teams):

```bash
gcc -Wall -Wextra -std=c99 src/*.c -o huffman
```

### Opções via linha de comando
```bash
./huffman                         # Abre o menu interativo
./huffman -c entrada.txt out.huff # Compacta um arquivo de texto
./huffman -d out.huff saida.txt   # Descompacta um arquivo .huff
./huffman -t dados/texto_pequeno_2kb.txt  # Compacta, descompacta e valida byte a byte
./huffman -b                      # Executa a bateria de testes nos arquivos de dados/
./huffman -e                      # Executa os testes de casos especiais (vazio, 1 símbolo, etc.)
```

---

## Checklist para Entrega no Teams (conforme enunciado e rubrica)

1. **Preencher os nomes e matrículas reais dos integrantes** (5 a 7 pessoas) no relatório (`relatorio/relatorio.tex` ou `relatorio/relatorio.html`) e exportar o PDF final conferindo que ele possui **exatamente 3 páginas A4**.
2. **Submeter os arquivos individualmente (SEM compactar em `.zip`/`.rar`)**:
   - **Código-fonte (`.c` e `.h` apenas):** todos os 7 arquivos da pasta `src/`:
     - `ArvoreHuffman.h`, `ArvoreHuffman.c`
     - `FilaPrioridade.h`, `FilaPrioridade.c`
     - `Huffman.h`, `Huffman.c`
     - `main.c`
   - **Relatório (somente em PDF, 3 páginas):** `relatorio.pdf`
   - **Arquivos de dados utilizados:** os 5 arquivos `.txt` da pasta `dados/`
