#include "orbit_calculator.h"
#include <math.h>


Complex add_complex(Complex a, Complex b){
	Complex c;
	c.Re = a.Re + b.Re;
	c.Im = a.Im + b.Im;
	return c;
}

Complex mult_complex(Complex a, Complex b) {
	Complex c;
	c.Re = ((a.Re*b.Re) - (a.Im*b.Im));
	c.Im = ((a.Im*b.Re)+(a.Re*b.Im));
       return c;	
}

double modulus_squared_complex(Complex a) {
	return ((a.Re*a.Re) + (a.Im*a.Im));
}


Complex pow_complex_int(Complex a, int k) {
	Complex result = {1.0, 0.0};
	for (int i = 0; i < k; i++) {
		result = mult_complex(result, a);
	}

	return result;
}


int mandelbrot_orbit(Complex c, int k, int maxit) {
	Complex z = {0.0, 0.0};

	for (int i = 0; i < maxit; i++) {
		z = add_complex(pow_complex_int(z, k), c);

		if (modulus_squared_complex(z) > 4.0)
			return i;
	}

	return maxit;
}


int julia_orbit(Complex c, Complex z0, int k, int maxit) {
	return 0;
}
