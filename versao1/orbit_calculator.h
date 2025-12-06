#ifndef ORBIT_CALCULATOR_H
#define ORBIT_CALCULATOR_H

/*
 * Struct que representa um numero complexo.
 *
 * Composta por doubles, representando valores reais.
 */
typedef struct {
	double Re;
	double Im;
} Complex;

/*
 * Struct criada apenas para facilitar as funcoes
 * de verificao da continencia ou nao de um ponto dentro
 * de seu respectivo conjunto.
 *
 * Contem um complexo z, que eh o valor usado no gradiente
 * de cores o valor da iteracao final desse ponto.
 */
typedef struct {
	int iter;
	Complex z;
} OrbitResult;

// as funcoes abaixo sao criadas para definir
// as operacoes necessarias para os complexos
// para avaliar os conjuntos
Complex add_complex(Complex a, Complex b);
Complex mult_complex(Complex a, Complex b);
double modulus_squared_complex(Complex a);
Complex pow_complex_int(Complex a, int k);

/*
 * Funcao que define se um dado ponto esta ou nao
 * no conjunto de Mandelbrot ou Multibrot.
 *
 * Para um dado valor de c, baseado com z0 = 0,
 * a funcao faz a iteracao de zn+1 = pow(zn,k) + c
 * por maxit vezes.
 *
 * Se em qualquer momento o modulo ao quadrado de zn superar
 * 4, esse ponto e considerado fora do conjunto e a funcao
 * retorna, por meio da struct OrbitResult, a iteracao na qual
 * ele escapou o raio de convergencia o valor atual de zn.
 *
 * Se em nenhum momento zn superar o raio de convergencia,
 * esse ponto eh considerado pertencente ao conjunto e retorna
 * seu valor de zn final e o valor de maxit
 *
 */
OrbitResult mandelbrot_orbit(Complex c, int k, int maxit);


/*
 * Essa funcao eh analoga a funcao anterior, com algumas diferencas
 * chave que marcam a diferenca entre um conjunto de Mandelbrot e um
 * de Julia.
 *
 * Enquanto o Mandelbrot verificara diferentes valores de c na hora de
 * ser mapeado, o Julia verificara diferentes valores de z0, matendo c
 * constante atraves de todas as iteracoes, fora isso, as verificacoes
 * de pertencimento sao virtualmente as mesmas, assim como os seus retornos.
 */
OrbitResult julia_orbit(Complex c, Complex z0, int k, int maxit);

#endif
