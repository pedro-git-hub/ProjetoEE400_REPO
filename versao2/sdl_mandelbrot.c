#include "orbit_calculator.h"
#include <SDL2/SDL.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>

#define WIDTH 1920
#define HEIGHT 1080
#define DX_original 8.0

long double raio_convergencia_quadrado; // Ele deve ser ao quadrado
                                        // pra evitar ter que elevar
                                        // um número complexo ao quadrado
                                        // desnecessariamente

// Essa função lê o stdin e se for vazio, retorna o valor padrão. Mas se
// recebermos um valor novo, então retorna este valor novo
long double ler_longdouble_com_padrao(const char *mensagem,
                                      long double padrao) {
  char buffer[100];
  long double valor;

  printf("%s [%Lf]: ", mensagem, padrao);

  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Se for só Enter, retorna o padrão
    if (buffer[0] == '\n') {
      return padrao;
    }
    // Tenta ler o número, se falhar, retorna o padrão
    if (sscanf(buffer, "%Lf", &valor) == 1) {
      return valor;
    }
  }
  return padrao;
}

// Essa função lê o stdin e se for vazio, retorna o valor padrão. Mas se
// recebermos um valor novo, então retorna este valor novo
int ler_inteiro_com_padrao(const char *mensagem, int padrao) {
  char buffer[100];
  int valor;

  printf("%s [%d]: ", mensagem, padrao);

  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Se for só Enter, retorna o padrão
    if (buffer[0] == '\n') {
      return padrao;
    }
    // Tenta ler o número, se falhar, retorna o padrão
    if (sscanf(buffer, "%d", &valor) == 1) {
      return valor;
    }
  }
  return padrao;
}

// Este é um enumerador para podermos escolher entre o Mandelbrot ou o Julia
enum Modo { MANDELBROT, JULIA };

// Essa função calcula os pixels e já os coloca na matrix de pixels
//
// Ela é um pouco invasiva pois estava escrita na função main. Decidimos tirá-la
// de lá para que o código seja mais legível.
//
// Contudo, isso fez com que fosse necessário passar diversos parâmetros, assim
// como ponteiros para variáveis externas. Se o código tivesse sido melhor
// planejado antes de começarmos, provavelmente essa função seria mais limpa
void calcular_pixels(long double dx, long double x_center, long double y_center,
                     enum Modo mode, Complex julia_c, int maxit, int k,
                     uint32_t *pixels, int *lines_finished) {

  // Isso aqui tem que ser calculado fora do For Loop, mais pra frente o
  // motivo dessa variável será explicado
  int progress_step = HEIGHT / 50;
  if (progress_step == 0)
    progress_step = 1;

  long double dy = dx * (long double)HEIGHT / WIDTH;
  long double xmin = x_center - dx / 2.0;
  long double ymin = y_center - dy / 2.0;

  long double x_scale = dx / WIDTH;
  long double y_scale = dy / HEIGHT;

  // Aqui estamos paralelizando de forma que o scheduling seja dinâmica. FAzemos
  // isso porque não podemos dividir os pixels em quadrantes para cada thread,
  // pois essa seria uma divisão injusta.
  //
  // Threads que pegassem um pixel longe do centro realizariam menos iterações
  // do que threads que pegassem pixels perto das bordas, pois teriam que fazer
  // mais iterações até descobrir se converge ou diverge.
  //
  // Assim, com o scheduling dinâmico, as threads podem pegar novos trabalhos
  // assim que finalizarem o seu trabalho. Permitindo uma divisão de trabalhos
  // mais justa
#pragma omp parallel for schedule(dynamic)
  for (int py = 0; py < HEIGHT; py++) {

    long double ci_init = ymin + py * y_scale;

    for (int px = 0; px < WIDTH; px++) {
      long double cr_init = xmin + px * x_scale;

      Complex z, c;

      if (mode == MANDELBROT) {
        // z0 é fixo e c varia
        c.Re = cr_init;
        c.Im = ci_init;
        z.Re = 0.0;
        z.Im = 0.0;
      } else {
        // z0 varia e c é fixo
        c = julia_c;
        z.Re = cr_init;
        z.Im = ci_init;
      }

      int iter = 0;
      long double temp_re;

      long double z_re2 = z.Re * z.Re;
      long double z_im2 = z.Im * z.Im;

      while ((z_re2 + z_im2 <= raio_convergencia_quadrado) && (iter < maxit)) {

        // Aqui a fizemos uma otimização para o k = 2 pois ele é tão simples que
        // não é necessário entrar em um loop para executá-lo
        if (k == 2) {
          z.Im = 2.0 * z.Re * z.Im + c.Im;
          z.Re = z_re2 - z_im2 + c.Re;
        }

        // Mas, se o k não for igual a 2, então precisamos executar o for
        else {
          double temp_re = z.Re;
          double temp_im = z.Im;

          double acc_re = z.Re;
          double acc_im = z.Im;

          for (int i = 1; i < k; i++) {
            double old_acc_re = acc_re;
            acc_re = (old_acc_re * temp_re) - (acc_im * temp_im);
            acc_im = (old_acc_re * temp_im) + (acc_im * temp_re);
          }

          z.Re = acc_re + c.Re;
          z.Im = acc_im + c.Im;
        }

        z_re2 = z.Re * z.Re;
        z_im2 = z.Im * z.Im;

        iter++;
      }

      // Coloração (Simplificada para evitar log/sqrt)
      //
      // TODO: voltar com o log e o sqrt para ver se não fica mais bonito.
      uint32_t color;
      if (iter == maxit) {
        color = 0xFF000000; // Preto
      } else {
        long double t = (long double)iter / (long double)(iter + 8);
        uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
        uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
        uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
        color = (255 << 24) | (r << 16) | (g << 8) | b;
      }
      pixels[py * WIDTH + px] = color;
    }

    // Essa parte aqui é para a barra de progresso. Para as threads não se
    // sobrescreverem, precisamos fazer as coisas com operações atômicas
    int current_count;

// Incremento atômico (seguro para threads)
#pragma omp atomic capture
    current_count = ++*lines_finished;

    // Aqui utilizamos o progress_step. Em vez de atualizarmos a barra de
    // progresso a cada nova linha calculada, atualizamos ela só quando
    // chegarmos em uma determinada quantidade de linhas calculadas
    if (current_count % progress_step == 0 || current_count == HEIGHT) {

// "critical" impede que duas threads escrevam no terminal ao mesmo tempo
#pragma omp critical
      {
        float percent = (float)current_count / HEIGHT;
        int bar_width = 50;
        int pos = bar_width * percent;

        printf("\rRenderizando: [");
        for (int i = 0; i < bar_width; ++i) {
          if (i < pos)
            printf("=");
          else if (i == pos)
            printf(">");
          else
            printf(" ");
        }
        printf("] %d%%", (int)(percent * 100));
        fflush(stdout); // Força o terminal a atualizar na hora
      }
    }
  }
}

// Essa função desenha os eixos (Real e Imaginário) na tela para melhor nos localizarmos
void desenhar_eixos(long double dx, long double x_center, long double y_center,
                    SDL_Renderer *renderer) {

  // Primeiro calculamos onde o 0.0 está na tela
  // Como 0 = xmin + (px / width) * dx  --> isolando px:
  long double dy = dx * (long double)HEIGHT / WIDTH;
  long double xmin = x_center - dx / 2.0;
  long double ymin = y_center - dy / 2.0;
  int axis_x = (int)((0.0 - xmin) / dx * WIDTH);
  int axis_y = (int)((0.0 - ymin) / dy * HEIGHT);

  // Linha Branca
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // R, G, B, Alpha

  // Desenhamos o Eixo Imaginário se estiver visível
  if (axis_x >= 0 && axis_x < WIDTH) {
    SDL_RenderDrawLine(renderer, axis_x, 0, axis_x, HEIGHT);
  }

  // Desenhamos o Eixo Real se estiver visível
  if (axis_y >= 0 && axis_y < HEIGHT) {
    SDL_RenderDrawLine(renderer, 0, axis_y, WIDTH, axis_y);
  }
}

int main() {

  enum Modo mode = MANDELBROT;
  Complex julia_c = {0.355, 0.355};
  uint32_t *pixels = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

  int running = 1; // Essa flag define se devemos fechar o programa
  int needs_refresh = 1; // Essa flag define se devemos recalcular

  // Configurações iniciais
  int k = 2;
  int maxit = 2000;

  // Calcular coordenadas
  long double x_center = -0.75;
  long double y_center = 0.0;
  long double dx = DX_original;

  // Obter valores customizados
  printf("Digite o Julia desejado (ou aperte Enter para utilizar o padrão):\n");
  julia_c.Re = ler_longdouble_com_padrao("Re", julia_c.Re);
  julia_c.Im = ler_longdouble_com_padrao("Im", julia_c.Im);

  long double raio_convergencia = ler_longdouble_com_padrao("Raio de Convergência", 2.0);

  raio_convergencia_quadrado = raio_convergencia * raio_convergencia;

  maxit = ler_inteiro_com_padrao("Qual o número de iterações máximo", maxit);

  k = ler_inteiro_com_padrao("Qual o k desejado", k);


  // O zoom modifica o maxit. Então precisamos guardar o valor original para
  // restaurar
  int maxit_original = maxit; 

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    return 1;

  SDL_Window *window =
      SDL_CreateWindow("Mandelbrot Otimizado", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  SDL_Event e;

  while (running) {
    // Antes de desenharmos qualquer coisa, verificamos se alguma tecla foi
    // apertada
    while (SDL_PollEvent(&e)) {
      // Fechar Programa
      if (e.type == SDL_QUIT)
        running = 0;

      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {

        // Zoom (aproxima onde o mouse está)
        case SDLK_z: {
          int mx, my;
          SDL_GetMouseState(&mx, &my); // Pega a posição atual do mouse

          // Bordas
          long double current_dy = dx * (long double)HEIGHT / WIDTH;
          long double cur_xmin = x_center - dx / 2.0;
          long double cur_ymin = y_center - current_dy / 2.0;

          // Coordenadas do mouse
          long double mouse_re = cur_xmin + ((long double)mx / WIDTH) * dx;
          long double mouse_im =
              cur_ymin + ((long double)my / HEIGHT) * current_dy;
          x_center = mouse_re;
          y_center = mouse_im;

          // Dá um zoom para 50%
          dx *= 0.5;

          // Aumentamos as iterações quanto mais fundo formos
          maxit += 50;

          needs_refresh = 1;
          break;
        }

        // RESET
        case SDLK_r:
          dx = DX_original;
          x_center = -0.75;
          y_center = 0.0;
          maxit = maxit_original; // Reseta iterações também se tiver alterado
          needs_refresh = 1;
          break;

        // Alterar modo
        case SDLK_j:
          mode = !mode;
          needs_refresh = 1;
          break;
        }
      }
    }

    // Só recalculamos tudo se for necessário
    if (needs_refresh) {

      printf("Zoom atual (largura da tela): %.18Lf\n", dx);

      // Configurações iniciais da barra de progresso
      int lines_finished = 0;

      printf("Renderizando: "
             "[..................................................] 0%%\r");

      fflush(stdout);

      // Aqui calculamos os pixels de fato, de forma paralelizada
      calcular_pixels(dx, x_center, y_center, mode, julia_c, maxit, k, pixels,
                      &lines_finished);

      // Atualiza textura apenas após recalcular tudo
      SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));

      needs_refresh = 0; // Trabalho feito, descansa até o próximo evento

      // Renderiza na tela
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);

      // Inserimos os eixos em cima de tudo
      desenhar_eixos(dx, x_center, y_center, renderer);

      SDL_RenderPresent(renderer);
    } else {
      // Não precisamos recalcular a todo momento, colocamos um Delay aqui para
      // aliviar a CPU
      SDL_Delay(16);
    }
  }

  free(pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
