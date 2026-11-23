# Compressão de Textos com o Algoritmo de Huffman

Trabalho da disciplina **FACOM32402 – Estrutura de Dados II (UFU / FACOM — 2026/2)**, ministrada pelo **Prof. Renato Pimentel**.

Implementação em linguagem C do algoritmo de compressão de Huffman utilizando **Tipos Abstratos de Dados (TADs) opacos**, baseada na estrutura de `ArvoreBinaria` apresentada em aula.

---

## Estrutura do Projeto

```text
algoritmo-huffman/
├── src/                        # Código-fonte em C (.c e .h) com includes planos
│   ├── ArvoreHuffman.h / .c    # TAD opaco da Árvore de Huffman (nós folha, internos e consultas)
│   ├── FilaPrioridade.h / .c   # TAD opaco da Fila de Prioridade Mínima (Min-Heap em vetor)
│   ├── Huffman.h / .c          # TAD opaco principal (frequências, códigos, cabeçalho, bits, métricas)
│   └── main.c                  # Interface interativa, linha de comando e bateria de testes
├── dados/                      # Obras literárias reais de Machado de Assis (Domínio Público / MEC)
│   ├── texto_pequeno_2kb.txt   # 3.282 B   (~3,2 KB)  — Carta à Imprensa Acadêmica (1864)
│   ├── texto_medio_15kb.txt    # 16.453 B  (~16,1 KB) — Crítica Literária: Castro Alves (1868)
│   ├── texto_medio_50kb.txt    # 50.976 B  (~49,8 KB) — Peça Teatral: As Forcas Caudinas
│   ├── texto_grande_200kb.txt  # 207.000 B (~202,1 KB)— Romance: A Mão e a Luva (1874)
│   └── texto_grande_500kb.txt  # 458.506 B (~447,8 KB)— Romance: Quincas Borba (1891)
├── relatorio/                  # Relatório técnico em exatamente 3 páginas A4
│   ├── relatorio.pdf           # Relatório final compilado via pdflatex (3 páginas A4)
│   ├── relatorio.tex           # Código-fonte LaTeX
│   ├── relatorio.md            # Versão em Markdown
│   └── relatorio.html          # Versão em HTML formatada para impressão A4
├── contexto_regras/            # Enunciado, rubrica e código-base de referência do professor
├── Makefile                    # Atalhos para compilar, testar, gerar PDF e checar no Valgrind
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

# Executar testes de casos de fronteira + bateria completa dos 5 textos
make testar

# Verificar ausência de vazamentos de memória (Valgrind)
make valgrind

# Recompilar o relatório em PDF (gera relatorio/relatorio.pdf)
make pdf

# Remover binário e arquivos temporários
make limpar
```

### Compilação direta com `gcc` (pasta plana, como na entrega do Teams)
Como os `#include` são planos e `main.c` localiza os arquivos `.txt` tanto em `dados/` quanto no diretório atual, o programa compila e roda perfeitamente quando todos os arquivos `.c`, `.h` e `.txt` estão juntos na mesma pasta:

```bash
gcc -Wall -Wextra -std=c99 *.c -o huffman
./huffman -b
```

---

## Checklist para Entrega no Teams (SEM `.zip`)

Submeter os **13 arquivos individualmente** na tarefa do Teams:
1. **7 arquivos de código-fonte (`.c` e `.h` apenas):**
   - `ArvoreHuffman.h`, `ArvoreHuffman.c`
   - `FilaPrioridade.h`, `FilaPrioridade.c`
   - `Huffman.h`, `Huffman.c`
   - `main.c`
2. **1 relatório em PDF (exatamente 3 páginas A4):**
   - `relatorio.pdf` (localizado em `relatorio/relatorio.pdf`)
3. **5 arquivos de texto utilizados nos testes:**
   - `texto_pequeno_2kb.txt`
   - `texto_medio_15kb.txt`
   - `texto_medio_50kb.txt`
   - `texto_grande_200kb.txt`
   - `texto_grande_500kb.txt`
