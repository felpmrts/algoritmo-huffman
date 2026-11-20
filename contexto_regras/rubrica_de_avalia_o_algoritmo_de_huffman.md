# Rubrica de Avaliação — Algoritmo de Huffman

## Resumo dos Critérios e Pesos

| Categoria | Critério | Pontos | Peso |
| :--- | :--- | :---: | :---: |
| **Código (C)** | Implementação em C com tipo abstrato de dados opaco | 3 pontos | 15% |
| **Código (C)** | Lógica de funcionamento correta e ausência de lacunas | 5 pontos | 25% |
| **Código (C)** | Modularização do código | 5 pontos | 25% |
| **Código (C)** | Documentação no código | 2 pontos | 10% |
| **Relatório** | Formato PDF com 3 páginas | 1 ponto | 5% |
| **Relatório** | Lógica do algoritmo e detalhes de implementação | 1 ponto | 5% |
| **Relatório** | Links/referências dos materiais utilizados e justificativa da escolha | 1 ponto | 5% |
| **Relatório** | Nomes dos integrantes e papel desempenhado | 1 ponto | 5% |
| **Relatório** | Testes para diferentes tamanhos e discussão dos resultados | 1 ponto | 5% |
| **Total** | | **20 pontos** | **100%** |

---

## Parte 1: Implementação em C (15 pontos | 75%)

| Critério | Peso / Pontos | Excelente (100 pts) | Bom (66.7 pts) | Insuficiente (33.3 pts) | Inexistente (0 pts) |
| :--- | :---: | :--- | :--- | :--- | :--- |
| **Implementação em C com tipo abstrato de dados opaco?** | 15%<br>*(3 pontos)* | O grupo implementa o algoritmo de Huffman em C e protege os dados internos com um tipo opaco, acessível apenas pelas funções previstas. | O grupo implementa o algoritmo em C e usa um tipo opaco, mas há pequenos problemas na proteção ou no acesso aos dados internos. | O grupo implementa parte do algoritmo em C, mas o tipo opaco está incompleto ou os dados internos ficam expostos. | O grupo não apresenta uma implementação funcional em C ou não usa um tipo opaco para organizar os dados. |
| **Lógica de funcionamento correta? Alguma lacuna?** | 25%<br>*(5 pontos)* | O grupo aplica corretamente as etapas do algoritmo de Huffman e trata os casos necessários, sem deixar lacunas importantes. | O grupo aplica quase todas as etapas corretamente, mas apresenta um erro pequeno ou deixa de tratar algum caso. | O grupo mostra parte da lógica do algoritmo, mas há erros ou casos importantes sem solução. | A lógica apresentada pelo grupo não segue as etapas do algoritmo de Huffman ou não permite que ele funcione. |
| **Modularização do código?** | 25%<br>*(5 pontos)* | O grupo divide o código em funções e módulos com tarefas claras, facilitando encontrar e entender cada parte. | O grupo organiza a maior parte do código em funções e módulos, embora algumas partes pudessem ficar mais claras ou separadas. | O grupo usa poucas funções ou mistura tarefas diferentes, o que torna difícil acompanhar o código. | O grupo entrega o código sem uma organização clara em funções ou módulos. |
| **Documentação no código** | 10%<br>*(2 pontos)* | O grupo comenta as partes que precisam de explicação e deixa claro o que fazem as funções principais. | O grupo explica a maior parte das funções principais, mas alguns trechos importantes ficam sem comentários claros. | O grupo inclui poucos comentários, que ajudam pouco a entender as partes mais importantes do código. | O grupo não inclui comentários úteis para explicar o código. |

---

## Parte 2: Relatório (5 pontos | 25%)

| Critério | Peso / Pontos | Excelente (100 pts) | Bom (66.7 pts) | Insuficiente (33.3 pts) | Inexistente (0 pts) |
| :--- | :---: | :--- | :--- | :--- | :--- |
| **Relatório no formato PDF, com 3 páginas?** | 5%<br>*(1 ponto)* | O grupo entrega o relatório em PDF com exatamente 3 páginas. | O grupo entrega o relatório em PDF, mas ele tem uma página a mais ou a menos do que o pedido. | O grupo entrega o relatório em PDF, mas ele fica muito distante das 3 páginas pedidas. | O grupo não entrega o relatório em PDF. |
| **Relatório: Lógica do algoritmo e detalhes de implementação?** | 5%<br>*(1 ponto)* | O grupo explica com clareza como o algoritmo de Huffman funciona e como foi implementado no programa. | O grupo explica o funcionamento do algoritmo e a implementação, mas deixa de fora algum detalhe importante. | O grupo apresenta uma explicação curta ou confusa, sem mostrar bem como o algoritmo foi implementado. | O grupo não explica a lógica do algoritmo nem os detalhes da implementação. |
| **Relatório: links ou referências para os materiais utilizados e justificativa da escolha?** | 5%<br>*(1 ponto)* | O grupo inclui links que abrem para todos os materiais consultados e explica por que eles foram escolhidos. | O grupo inclui links e explica a escolha, mas falta uma fonte ou uma justificativa mais clara. | O grupo apresenta alguns links ou uma justificativa, mas não faz as duas coisas de forma suficiente. | O grupo não inclui links para os materiais consultados nem explica por que os textos foram escolhidos. |
| **Relatório: nomes dos integrantes e papel desempenhado?** | 5%<br>*(1 ponto)* | O grupo inclui no relatório o nome de todos os integrantes e o papel desempenhado por cada um. | O grupo não destaca os papéis de cada membro, apenas colocando algo genérico como "todos colaboraram em todos os aspectos relacionados ao trabalho". | Apenas os nomes. | Nenhuma informação. |
| **Relatórios: testes para diferentes tamanhos (ordem de alguns KB, dezenas de KB, centenas de KB, etc?). Discussão dos resultados?** | 5%<br>*(1 ponto)* | O grupo apresenta os percentuais e comparativos de compactação para diferentes tamanhos de arquivos de texto. Discute os resultados. | O grupo apresenta os percentuais e comparativos de compactação para diferentes tamanhos de arquivos de texto, porém não discute ou tenta explicar a eventual diferença nos resultados. | O grupo apresenta apenas o resultado de compactação de um único documento, explicando o observado. | O grupo apresenta apenas o resultado de compactação de um único documento, mas não explica o que observou. |