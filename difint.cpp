/* difint.cpp */
/* This program, "difint.cpp", is derived from
 * http://www5.airnet.ne.jp/tomy/
 * Author & Copyright: Tomy
 * Thank you.
 */

#include <stdio.h>
#include <stdlib.h>
#include "sslib.h"

double nc8(double xmin, double xmax, int n,render *rh,double (render::*_f)(double,unsigned long long)){
  double h, hh, h2, h3, h4, h5, h6, h7, hi, s, x;
  int i;

  if(n < 2)
    {
      fprintf(stderr, "Error : n < 2  in nc8()\n");
      return 0.;
    }
  if(xmin == xmax)return 0.;
  s = (rh->*(_f))(xmin,(unsigned long)xmax) - 989. * (rh->*(_f))(xmax,(unsigned long)xmax);
  h = (xmax - xmin) / (double)n;
  hh = h / 8.;
  h2 = h / 4.;
  h3 = h * .375;
  h4 = h / 2.;
  h5 = h / 1.6;
  h6 = h * .75;
  h7 = h * .875;
  for(i = 0, hi = 1.; i < n; i++, hi += 1.)
    {
      x = xmin + h * hi;
      s += (1978. * (rh->*(_f))(x,(unsigned long)xmax) +  5888. * ((rh->*(_f))(x - h7,(unsigned long)xmax) + (rh->*(_f))(x - hh,(unsigned long)xmax))
	    -   928. * ((rh->*(_f))(x - h6,(unsigned long)xmax) + (rh->*(_f))(x - h2,(unsigned long)xmax))
	    + 10496. * ((rh->*(_f))(x - h5,(unsigned long)xmax) + (rh->*(_f))(x - h3,(unsigned long)xmax))
	    -  4540. *  (rh->*(_f))(x - h4,(unsigned long)xmax));
    }
  return s * h / 28350.;
}


double hardy(double xmin, double xmax, int n, render *rh, double (render::*_f)(double,unsigned long long)){
  double h, hh, h3, h5, hi, s, x;
  int i;

  if(n < 2){
    fprintf(stderr, "Error : n < 2  in hardy()\n");
    return 0.;
  }
  if(xmin == xmax)
    return 0.;
  s = (rh->*(_f))(xmin,(unsigned long long)xmax) - 28. * (rh->*(_f))(xmax,(unsigned long long)xmax);
  h = (xmax - xmin) / (double)n;
  hh = h / 6.;
  h3 = h / 2.;
  h5 = h / 1.2;
  for(i = 0, hi = 1.; i < n; i++, hi += 1.){
    x = xmin + h * hi;
    s += (56. * (rh->*(_f))(x,(unsigned long long)xmax) + 162. * (rh->*(_f))(x - h5,(unsigned long long)xmax) + (rh->*(_f))(x - hh,(unsigned long long)xmax)) + 220. *  (rh->*(_f))(x - h3,(unsigned long long)xmax);
  }
  return s * h / 600.;
}

double dgl3(double a, double b, render *rh, double (render::*_f)(double,unsigned long long)){
  static double g = 0.774596669241483;
  static double w1 = 0.555555555555556;
  static double w0 = 0.88888888888889;
  double w;
  int mflag;
  
  if(a == b)
    return 0.;
  mflag = 0;
  if(a > b){
    mflag = 1;
    w = a;
    a = b;
    b = w;
  }
  w = (rh->*(_f))(_Normal(a, b, 0.),(unsigned long long)b) * w0;
  w += (rh->*(_f))(_Normal(a, b, g),(unsigned long long)b) + (rh->*(_f))(_Normal(a, b, -g),(unsigned long long)b) * w1;
  w *= ((b - a) / 2.);
  if(mflag)
    return -w;
  return w;
}

double dgl10(double a, double b, render *rh, double (render::*_f)(double,unsigned long long)){
  static double g[5] = {0.148874338981631, 0.433395394129247,
			0.679409568299024, 0.865063366688985,
			0.973906528517172};
  static double w[5] = {0.295524224714753, 0.269266719309996,
			0.219086362515982, 0.149451349150581,
			0.066671344308688};
  double s;
  int i, mflag;

  if(a == b)
    return 0.;
  mflag = 0;
  if(a > b){
    mflag = 1;
    s = a;
    a = b;
    b = s;
  }
  for(i = 0, s = 0.; i < 5; i++)
    s += w[i] *((rh->*(_f))(_Normal(a, b, g[i]),(unsigned long long)b) + (rh->*(_f))(_Normal(a, b, -g[i]),(unsigned long long)b));
  s *= ((b - a) / 2.);
  if(mflag)
    return -s;
  return s;
}

double dgl20(double a, double b, render *rh, double (render::*_f)(double,unsigned long long)){
  static double g[10] = {0.076526521133497333, 0.227785851141645078,
			 0.373706088715419561, 0.510867001950827098,
			 0.636053680726515025, 0.746331906460150793,
			 0.839116971822218823, 0.912234428251325906,
			 0.963971927277913791, 0.993128599185094925};
  static double w[10] = {0.152753387130725851, 0.149172986472603747,
			 0.142096109318382051, 0.131688638449176627,
			 0.118194531961518417, 0.101930119817240435,
			 0.083276741576704749, 0.062672048334109064,
			 0.040601429800386941, 0.017614007139152118};
  double s;
  int i, mflag;
  
  if(a == b)
    return 0.;
  mflag = 0;
  if(a > b){
    mflag = 1;
    s = a;
    a = b;
    b = s;
  }
  for(i = 0, s = 0.; i < 10; i++)
    s += w[i] *((rh->*(_f))(_Normal(a, b, g[i]),(unsigned long long)b) + (rh->*(_f))(_Normal(a, b, -g[i]),(unsigned long long)b));
  s *= ((b - a) / 2.);
  if(mflag)
    return -s;
  return s;
}

double dgl32(double a, double b, render *rh, double (render::*_f)(double,unsigned long long)){
  static double g[16] = {0.048307665687738316, 0.144471961582796494,
			 0.239287362252137075, 0.331868602282127650,
			 0.421351276130635345, 0.506899908932229390,
			 0.587715757240762329, 0.663044266930215201,
			 0.732182118740289680, 0.794483795967942407,
			 0.849367613732569970, 0.896321155766052124,
			 0.934906075937739690, 0.964762255587506431,
			 0.985611511545268335, 0.997263861849481564};
  static double w[16] = {0.096540088514727801, 0.095638720079274859,
			 0.093844399080804566, 0.091173878695763885,
			 0.087652093004403811, 0.083311924226946755,
			 0.078193895787070306, 0.072345794108848506,
			 0.065822222776361847, 0.058684093478535547,
			 0.050998059262376176, 0.042835898022226681,
			 0.034273862913021433, 0.025392065309262059,
			 0.016274394730905671, 0.007018610009470097};
  double s;
  int i, mflag;
  
  if(a == b)
    return 0.;
  mflag = 0;
  if(a > b){
    mflag = 1;
    s = a;
    a = b;
    b = s;
  }
  for(i = 0, s = 0.; i < 16; i++)
    s += w[i] *((rh->*(_f))(_Normal(a, b, g[i]),(unsigned long long)b) + (rh->*(_f))(_Normal(a, b, -g[i]),(unsigned long long)b));
  s *= ((b - a) / 2.);
  if(mflag)
    return -s;
  return s;
}

double dgl48(double a, double b, render *rh, double (render::*_f)(double,unsigned long long)){
  static double g[24] = {0.032380170962869362, 0.097004699209462699,
			 0.161222356068891718, 0.224763790394689061,
			 0.287362487355455577, 0.348755886292160738,
			 0.408686481990716730, 0.466902904750958405,
			 0.523160974722233034, 0.577224726083972704,
			 0.628867396776513624, 0.677872379632663905,
			 0.724034130923814655, 0.767159032515740339,
			 0.807066204029442627, 0.843588261624393531,
			 0.876572020274247886, 0.905879136715569673,
			 0.931386690706554333, 0.952987703160430861,
			 0.970591592546247250, 0.984124583722826858,
			 0.993530172266350758, 0.998771007252426119};
  static double w[24] = {0.064737696812683923, 0.064466164435950082,
			 0.063924238584648187, 0.063114192286254026,
			 0.062039423159892664, 0.060704439165893880,
			 0.059114839698395636, 0.057277292100403216,
			 0.055199503699984163, 0.052890189485193667,
			 0.050359035553854475, 0.047616658492490475,
			 0.044674560856694280, 0.041545082943464749,
			 0.038241351065830706, 0.034777222564770439,
			 0.031167227832798089, 0.027426509708356948,
			 0.023570760839324379, 0.019616160457355528,
			 0.015579315722943849, 0.011477234579234539,
			 0.007327553901276262, 0.003153346052305839};
  double s;
  int i, mflag;
  
  if(a == b)
    return 0.;
  mflag = 0;
  if(a > b){
    mflag = 1;
    s = a;
    a = b;
    b = s;
  }
  for(i = 0, s = 0.; i < 24; i++)
    s += w[i] *((rh->*(_f))(_Normal(a, b, g[i]),(unsigned long long)b) + (rh->*(_f))(_Normal(a, b, -g[i]),(unsigned long long)b));
  s *= ((b - a) / 2.);
  if(mflag)
    return -s;
  return s;
}

double _Normal(double a, double b, double x){
  return ((b - a) * x + a + b) / 2.;
}
