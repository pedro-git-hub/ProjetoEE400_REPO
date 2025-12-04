#include "orbit_calculator.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <omp.h> // Importante para as funções do OpenMP

#define WIDTH 1920
#define HEIGHT 1080

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

    SDL_Window *window = SDL_CreateWindow("Mandelbrot Otimizado", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    int mode = 0; 
    Complex julia_c = {-0.8, 0.156};
    uint32_t *pixels = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

    int running = 1;
    int needs_refresh = 1; // SÓ CALCULA SE FOR 1

    // Configurações iniciais
    int k = 2;
    // Reduzi drásticamente para performance em tempo real
    int maxit = 100000; 
    
    // Calcular coordenadas
    double x_center = -0.75; 
    double y_center = 0.0;
    double dx = 3.0; 

    SDL_Event e;

    while (running) {
        // 1. Processa TODOS os eventos pendentes antes de decidir desenhar
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            
            if (e.type == SDL_KEYDOWN) {
                needs_refresh = 1; // Marcou que mudou algo, precisa redesenhar
                if (e.key.keysym.sym == SDLK_j) mode = 1 - mode;
                // Exemplo: Resetar zoom
                if (e.key.keysym.sym == SDLK_r) { dx = 3.0; x_center = -0.75; y_center = 0.0; }
            }
            // Adicione lógica de zoom/pan aqui e set needs_refresh = 1
        }

        // 2. Só entra no loop PESADO se precisar
        if (needs_refresh) {
            double dy = dx * (double)HEIGHT / WIDTH;
            double xmin = x_center - dx / 2.0;
            double ymin = y_center - dy / 2.0;
            
            // OTIMIZAÇÃO: Pré-cálculo dos fatores de escala
            double x_scale = dx / WIDTH;
            double y_scale = dy / HEIGHT;

            #pragma omp parallel for schedule(dynamic)
            for (int py = 0; py < HEIGHT; py++) {
                // Otimização: Calcular y_coord fora do loop interno
                double ci_init = ymin + py * y_scale;

                for (int px = 0; px < WIDTH; px++) {
                    double cr_init = xmin + px * x_scale;
                    
                    Complex z, c;
                    
                    if (mode == 0) { // Mandelbrot
                        c.Re = cr_init; c.Im = ci_init;
                        z.Re = 0.0;     z.Im = 0.0;
                    } else { // Julia
                        c = julia_c;
                        z.Re = cr_init; z.Im = ci_init;
                    }

                    int iter = 0;
                    double temp_re;
                    
                    // OTIMIZAÇÃO CRÍTICA: Loop desenrolado manualmente para k=2
                    // Isso evita chamadas de função e permite o compilador usar AVX
                    double z_re2 = z.Re * z.Re;
                    double z_im2 = z.Im * z.Im;

                    while ((z_re2 + z_im2 <= 4.0) && (iter < maxit)) {
                        z.Im = 2.0 * z.Re * z.Im + c.Im;
                        z.Re = z_re2 - z_im2 + c.Re;
                        
                        z_re2 = z.Re * z.Re;
                        z_im2 = z.Im * z.Im;
                        iter++;
                    }

                    // Coloração (Simplificada para evitar log/sqrt pesados dentro do loop crítico se não necessário)
                    uint32_t color;
                    if (iter == maxit) {
                        color = 0xFF000000; // Preto
                    } else {
                        // Truque simples de coloração sem muita matemática pesada
                        double t = (double)iter / (double)(iter+8);
                        uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
                        uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
                        uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
                        color = (255 << 24) | (r << 16) | (g << 8) | b;
                    }
                    pixels[py * WIDTH + px] = color;
                }
            }
            
            // Atualiza textura apenas após recalcular tudo
            SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));
            needs_refresh = 0; // Trabalho feito, descansa até o próximo evento
            
            // Renderiza na tela
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        else {
             // Se nada mudou, dorme um pouco para não fritar a CPU à toa (16ms ~ 60fps)
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
