/* sslib.h */
/* This header file, "sslib.h", is derived from
 * http://www5.airnet.ne.jp/tomy/
 * Author & Copyright: Tomy
 * Thank you.
 */
#ifndef _SSLIB
#define _SSLIB

#include <math.h>
#include <render.h>

double nc8(double xmin, double xmax, int n,render *rh,double (render::*_f)(double,unsigned long long));

double hardy(double xmin, double xmax, int n, render *rh, double (render::*_f)(double,unsigned long long));

// Definite integral of Gauss-Legendre Method
double dgl3(double a, double b, render *rh, double (render::*_f)(double,unsigned long long));
double dgl10(double a, double b, render *rh, double (render::*_f)(double,unsigned long long));
double dgl20(double a, double b, render *rh, double (render::*_f)(double,unsigned long long));
double dgl32(double a, double b, render *rh, double (render::*_f)(double,unsigned long long));
double dgl48(double a, double b, render *rh, double (render::*_f)(double,unsigned long long));

double _Normal(double a, double b, double x);

#endif
