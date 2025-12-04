#ifndef ORBIT_CALCULATOR_H
#define ORBIT_CALCULATOR_H

typedef struct {
	long double Re;
	long double Im;
} Complex;

typedef struct {
	int iter;
	Complex z;
} OrbitResult;

// Funções marcadas como static inline para o compilador "colar" o código direto no loop
static inline Complex add_complex(Complex a, Complex b){
    Complex c;
    c.Re = a.Re + b.Re;
    c.Im = a.Im + b.Im;
    return c;
}

static inline Complex mult_complex(Complex a, Complex b) {
    Complex c;
    c.Re = ((a.Re*b.Re) - (a.Im*b.Im));
    c.Im = ((a.Im*b.Re)+(a.Re*b.Im));
    return c;    
}

static inline long double modulus_squared_complex(Complex a) {
    return ((a.Re*a.Re) + (a.Im*a.Im));
}

OrbitResult mandelbrot_orbit(Complex c, int k, int maxit);

OrbitResult julia_orbit(Complex c, Complex z0, int k, int maxit);

#endif
