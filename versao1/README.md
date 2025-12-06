# ProjetoEE400_REPO
Repositório que contém todos os códigos desenvolvidos pelo grupo para a resolução dos exercícios e propostas apresentadas no arquivo do projeto escolhido.

Instruções para o uso do código:

Para compilar o código usando o gcc, é necessário usar o seguinte comando no seu terminal:
"gcc mandelbrot_nparallel.c orbit_calculator.c -o mandelbrot $(sdl2-config --cflags --libs) -lm"

Após compilado, execute o executável mandelbrot usando ./mandelbrot.

O terminal irá solicitar que você coloque os valores de k, o exponente da definição. É necessário que você coloque um valor pois todos os
inputs, por questão de facilidade, foram feitos com scanf, necessitando um input do tipo de cada variável, k aqui só tolera valores positivos maiores ou iguais a 2.

iterações aceita até 4000 iterações.

os valores para as componentes reais e complexas de c para o conjunto de Julia dessa instância são livres.

Ao ser aberta a janela, o terminal terá uma porcentagem de renderização para acompanhar o tempo para o cálculo do fractal.

Aperte J enquanto na janela para alternar entre o fractal de Mandelbrot e o de Julia.

Para alterar coisas como o raio de convergência ou o z0 inicial, é necessário editar o arquivo orbit_calculator.c para adequar os parâmetros desejados.

O executável nesse documento vem com todos os valores padrões indicados no terminal.
