#ifndef ORBIT_CALCULATOR_H
#define ORBIT_CALCULATOR_H

typedef struct {
	double Re;
	double Im;
} Complex;

typedef struct {
	int iter;
	Complex z;
} OrbitResult;

Complex add_complex(Complex a, Complex b);
Complex mult_complex(Complex a, Complex b);
double modulus_squared_complex(Complex a);
Complex pow_complex_int(Complex a, int k);

OrbitResult mandelbrot_orbit(Complex c, int k, int maxit);

OrbitResult julia_orbit(Complex c, Complex z0, int k, int maxit);

#endif
