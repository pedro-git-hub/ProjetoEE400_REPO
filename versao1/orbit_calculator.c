#include "orbit_calculator.h"
#include <math.h>

/*
 * Funcao responsavel por adicionar numeros complexos.
 *
 * Recebe 2 numeros complexos.
 *
 * Retorna outro numero complexo.
 */
Complex add_complex(Complex a, Complex b){
	Complex c;
	c.Re = a.Re + b.Re;
	c.Im = a.Im + b.Im;
	return c;
}


/*
 * Funcao que multiplica dois complexos de acordo com as
 * regras de multiplicacao de complexos.
 *
 * Recebe 2 complexos.
 *
 * Retorna outro numero complexo.
 */
Complex mult_complex(Complex a, Complex b) {
	Complex c;
	c.Re = ((a.Re*b.Re) - (a.Im*b.Im));
	c.Im = ((a.Im*b.Re)+(a.Re*b.Im));
       return c;	
}

/*
 * Funcao que calculo o modulo ao quadrado
 * de um numero complexo, feito dessa forma
 * para poupar poder computacional nos
 * calculus
 *
 * Recebe 1 numero complexo.
 *
 * Retorna um numero real(double).
 */
double modulus_squared_complex(Complex a) {
	return ((a.Re*a.Re) + (a.Im*a.Im));
}

/*
 * Funcao que faz a potenciacao de numeros complexos(z^k).
 *
 * Recebe um numero complexo e o seu expoente.
 *
 * Retorna outro numero complexo.
 */
Complex pow_complex_int(Complex a, int k) {
	Complex result = {1.0, 0.0}; // Elemento neutro da multiplicacao
	for (int i = 0; i < k; i++) {
		result = mult_complex(result, a);
	}

	return result;
}

/*
 * Funcao que verifica se um dado ponto esta ou nao
 * dentro do conjunto de Mandelbrot.
 *
 * Recebe 1 complexo c e 2 inteiros, expoente k e maximo de
 * iteracoes.
 *
 * Retorna a struct com ou o valor do iterando i ate o
 * ponto em que o raio de convergencia foi superado(fora do conjunto)
 * ou retorna o maximo de iteracoes(dentro do conjunto)
 */
OrbitResult mandelbrot_orbit(Complex c, int k, int maxit) {
	Complex z = {0.0, 0.0}; // z0 padrao eh a origem
	OrbitResult r; 

	for (int i = 0; i < maxit; i++) {
		z = add_complex(pow_complex_int(z, k), c);
		
		// qualquer valor que supere o raio de convergencia
		// nao esta contido no conjunto
		if (modulus_squared_complex(z) > 4.0) {
			r.iter = i;
			r.z = z;
			return r;
		}
	}

	// um ponto que nao diverge para o dado
	// numero de iteracoes maxima eh assumido
	// como contido no conjunto
	r.iter = maxit;
	r.z = z;
	return r;
}

// Analogo a funcao anterior, muda a logica baseado no comportamento
// dos conjuntos de Julia
OrbitResult julia_orbit(Complex c, Complex z0, int k, int maxit) {
	Complex z = z0;
	OrbitResult r;

	for (int i = 0; i < maxit; i++) {
		z = add_complex(pow_complex_int(z,k), c);

		if (modulus_squared_complex(z) > 4.0) {
			r.iter = i;
			r.z = z;
			return r;
		}
	}

	r.iter = maxit;
	r.z = z;
	return r;
}
