#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "orbit_calculator.h"

#define WIDTH 1920
#define HEIGHT 1080

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init ERROR: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"Pixel Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT,
		SDL_WINDOW_SHOWN
		);

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED
		);

	SDL_Texture *texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH,
		HEIGHT
		);
	int mode = 0; // Mandelbrot por padrao
	Complex julia_c = {-0.8, 0.156}; // valor comum de Julia

	uint32_t *pixels = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

	int running = 1;
	SDL_Event e;

	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				running = 0;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_j) {
					mode = 1 - mode; // toggle entre Mandelbrot e Julia
				}
			}

			int k = 2;	
			double x_center = -pow(1.0 -1.0/k, (double)k/(k - 1.0));
			double y_center = 0.0;
			double dx = 6.0;
			double dy = dx * (double)HEIGHT/WIDTH;
			double xmin = x_center - dx/2.0;
			double xmax = x_center + dx/2.0;
			double ymin = y_center - dy/2.0;
			double ymax = y_center + dy/2.0;

			int maxit = 100000;

			for (int py = 0; py < HEIGHT; py++) {
				SDL_PollEvent(&e);
   	 				if (e.type == SDL_QUIT) {
       		 				running = 0;
       			 			break;
    				}
				for (int px = 0; px < WIDTH; px++){
					if (px % 200 == 0)
						SDL_PumpEvents();	
					double cr = xmin + (double)px / WIDTH * (xmax - xmin);
					double ci = ymin + (double)py/ HEIGHT * (ymax - ymin);

					OrbitResult r;
					
					if (mode == 0) {
						// Mandelbrot
						Complex c = {cr, ci};
						r = mandelbrot_orbit(c, k, maxit); 
					} else {
						// Julia
						Complex z0 = {cr, ci};
						r = julia_orbit(julia_c, z0, k, maxit);
					}

					double lambda;

					if (r.iter == maxit) {
						lambda = maxit;
					} else {
						double abs_z = sqrt(modulus_squared_complex(r.z));
						lambda = r.iter + 1 - (log(log(abs_z))) / log((double)k);
					}
					double t = lambda/(lambda+8.0);

					uint8_t rcol = (uint8_t)(9*(1-t)*t*t*t*255);
					uint8_t gcol = (uint8_t)(15*(1-t)*(1-t)*t*t*255);
					uint8_t bcol = (uint8_t)(8.5*(1-t)*(1-t)*(1-t)*t*255);
					pixels[py * WIDTH + px] = (255 << 24) | (rcol << 16) | (gcol << 8) | bcol;
				}
			}
		}
			

		SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	free(pixels);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
