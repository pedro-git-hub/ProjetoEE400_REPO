Neste repositório há duas versões.

A primeira versão foi feita de forma que o programa funcionasse com todas
as funcionalidades que queríamos.

A segunda versão foi feita após observarmos alguns problemas com a primeira versão.
Mas devido ao tempo, não conseguimos finalizá-la. Aqui vamos esclarecer as diferenças
entre cada uma.

| Funcionalidade | Versão 1 | Versão 2 |
|----------|----------|----------|
| paralelização   | ❌ | ✅ |
| funciona para diferentes k's   | ✅ | ❌ |
| colorização polinomial   | ✅ | ❌ |
| zoom   | ❌ | ✅ |
| eixos xy   | ❌ | ✅ |

### Paralelização

A motivação para a segunda versão foi a paralelização. Então todos os esforços
foram direcionados para isso. Portanto a versão 2 é paralelizada, mas a
versão 1 não é

### Funciona para diferentes k's

Em algum momento mexemos em uma parte do código que não deveriamos, e na hora
de enviar o projeto percebemos que não estava funcionando para diferentes valores
de k. E por falta de tempo não conseguimos resolver esse bug

### Colorização polinomial

Apesar de ser uma forma muito melhor de visualizar o código, percebmos que 
executar funções como log e raiz quadrada levavam muito tempo. Então decidimos
tirá-las em favor de acelerar a execução do código.

Contudo passamos a acreditar que possa ter sido uma otimiazação precoce, pois
deveríamos ter testado o com o código paralelizado se essas funções de fato
tomavam tanto tempo.

Infelizmente pela falta de tempo não conseguimos retomar a versão com colorização
polinomial

### Zoom e Eixos xy

A funcionalidade de zoom foi uma que sentimos falta na versão 1, e por isso
a implementamos na versão 2. Assim como a falta dos eixos deixava o usuário bastante
desorientado, por isso os implementamos na versão 2.

#### Extra

Percebemos que utilizar o `scanf` para obtermos os valores customizados não
foi a melhor opção, pois o usuário precisa reescrevê-los caso queira manter o
padrão. Na segunda versão isso não é necessário, basta apertar `Enter` que o
valor padrão já é selecionado

### Conclusão

Em resumo, **a versão 2 é uma versão funcional e melhorada em relação à versão 1**. 
Apenas alguns detalhes que o tornam um pouco mais limitado como a colorização e
não permitir diferentes valores de k
