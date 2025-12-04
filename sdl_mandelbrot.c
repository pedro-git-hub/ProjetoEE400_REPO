#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "orbit_calculator.h"

#define WIDTH 800
#define HEIGHT 600

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


	uint32_t *pixels = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

	int running = 1;
	SDL_Event e;

	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				running = 0;
			
			double xmin = -2.0;
			double xmax = 1.0;
			double ymin = -1.5;
			double ymax = 1.5;

			int k = 2;
			int maxit = 300;

			for (int py = 0; py < HEIGHT; py++) {
				for (int px = 0; px < WIDTH; px++){
					if (px % 200 == 0)
						SDL_PumpEvents();	
					double cr = xmin + (double)px / WIDTH * (xmax - xmin);
					double ci = ymin + (double)py/ HEIGHT * (ymax - ymin);

					Complex c = {cr, ci};

					OrbitResult r = mandelbrot_orbit(c, k, maxit);

					double lambda;

					if (r.iter == maxit) {
						lambda = maxit;
					} else {
						double abs_z = sqrt(modulus_squared_complex(r.z));
						lambda = r.iter + 1 - (log(log(abs_z))) / log((double)k);
					}
					double t = lambda/maxit;

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
