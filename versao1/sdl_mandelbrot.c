#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "orbit_calculator.h"

#define WIDTH 800
#define HEIGHT 600

int main() {

	// Constantes importantes
	int mode = 0; // Mandelbrot por padrao
	Complex julia_c = {-0.8, 0.156}; // valor comum de Julia
	int k = 2; // k = 2 padrao
	int maxit = 200; // 200 iteracoes como padrao
	int render = 1; // bit para controlar os calculos	

	printf("Escolha o expoente k [k_padrao = 2]: \n");
	scanf("%d", &k);
	if (k < 2)
		k = 2;

	printf("Escolha o numero maximo de iteracoes (<= 4000)[padrao = 200]: \n");
	scanf("%d", &maxit);
	if (maxit > 4000)
		maxit = 4000;

	printf("Escolha as componentes do c para Julia.\n");
	printf("Escolha Re(c)[padrao = -0.8]: \n");
	scanf("%lf", &julia_c.Re);

	printf("Escolha o Im(c)[padrao = 0.156]: \n");
	scanf("%lf", &julia_c.Im);

	// inicializacao da biblioteca	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init ERROR: %s\n", SDL_GetError());
		return 1;
	}
	
	// criacao da janela para a exibicao
	// dos fractais
	SDL_Window *window = SDL_CreateWindow(
		"Mandelbrot&Julia Sets",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT,
		SDL_WINDOW_SHOWN
		);
	
	// criacao do renderizador	
	SDL_Renderer *renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED
		);
	
	// criacao da textura/formato dos pixels
	SDL_Texture *texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH,
		HEIGHT
		);
	
	// alocacao da matriz de pixels
	uint32_t *pixels = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

	int running = 1; // bit para a operacao da janela
	SDL_Event e; // variavel de tratamento de eventos na janela

	// loop principal
	while (running) {
		// por simplicidade, usamos polling para
		// poder verificar as interrupcoes na janela
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) // garante que a janela
						// possa ser fechada
						// sem precisar dar kill no
						// programa
				running = 0;
			if (e.type == SDL_KEYDOWN) { // verifica se uma tecla foi apertada
				if (e.key.keysym.sym == SDLK_j) { // se j foi apertado
					mode = 1 - mode; // toggle entre Mandelbrot e Julia
					render = 1; // recalcular o conjunto
				}
			}
		}
		// render eh um equivalente
		// de um bit dirty para poder
		// garantir que o polling de eventos
		// n force o recalculamento do fractal
		// toda vez que uma interrupcao simples eh chamada
		// (movimento de mouse, minimizar a janela, etc)
		if (render) {
			double x_center = -pow(1.0 -1.0/k, (double)k/(k - 1.0)); // formula de melhor centralizacao dos
										 // fractais
			double y_center = 0.0; // simetrico em relacao a x
			double dx = 6.0; // dx maior que o normal para melhor visualizacao
			double dy = dx * (double)HEIGHT/WIDTH;
			double xmin = x_center - dx/2.0;
			double xmax = x_center + dx/2.0;
			double ymin = y_center - dy/2.0;
			double ymax = y_center + dy/2.0;

			// calculo dos pixels
			for (int py = 0; py < HEIGHT; py++) {
				printf("\rRendering %d%%", (100*py)/HEIGHT);
				fflush(stdout); // progresso dos calculus
				SDL_PollEvent(&e);
   	 				if (e.type == SDL_QUIT) {
       		 				running = 0;
       			 			break; // garante que o programa para
						       // caso uma solicitacao de fechar
						       // a janela apareca
    				}
				for (int px = 0; px < WIDTH; px++){
					if (px % 200 == 0)
						SDL_PumpEvents();	// garante que a janela nao trave enquanto
									// os calculos sao feitos
					double cr = xmin + (double)px / WIDTH * (xmax - xmin); // valor de c real
					double ci = ymin + (double)py/ HEIGHT * (ymax - ymin); // valor de c imaginario

					OrbitResult r;
					
					if (mode == 0) {
						// Mandelbrot
						Complex c = {cr, ci}; // z0 fixo, c variavel
						r = mandelbrot_orbit(c, k, maxit); 
					} else {
						// Julia
						Complex z0 = {cr, ci}; // c fixo, z0 variavel
						r = julia_orbit(julia_c, z0, k, maxit);
					}

					double lambda; // fator de correcao do gradiente

					if (r.iter == maxit) {
						lambda = maxit; // convergiu
					} else {
						double abs_z = sqrt(modulus_squared_complex(r.z));
						lambda = r.iter + 1 - (log(log(abs_z))) / log((double)k); // formula
													  // de suavizacao
													  // para o gradiente
					}
					double t = lambda/(lambda+8.0); // garante que o nao tenda a
									// 0 para iteracoes muito grandes
									// suavizando as cores

					// gradiente polinomial
					uint8_t rcol = (uint8_t)(9*(1-t)*t*t*t*255); 
					uint8_t gcol = (uint8_t)(15*(1-t)*(1-t)*t*t*255);
					uint8_t bcol = (uint8_t)(8.5*(1-t)*(1-t)*(1-t)*t*255);
					// shifts de bits para adequacao ao modelo arg8888
					pixels[py * WIDTH + px] = (255 << 24) | (rcol << 16) | (gcol << 8) | bcol;
				}
			}

			SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			render = 0; // para garantir que so ira recalcular
				    // o fractal caso haja uma solicitacao
				    // de mudanca de fractal (julia pra mandel-
				    // -brot e vice-versa)
		}
	}

	// desalocando a memoria
	free(pixels);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
