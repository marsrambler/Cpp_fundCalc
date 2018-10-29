#include <vector>
#include <cmath>
#include "PearsonAlgorithm.h"
using namespace std;

PearsonAlgorithm::CORR_TYPE PearsonAlgorithm::getCorrType(float corrVal)
{
    if (corrVal > VERY_STRONG_CORR_POSI) return VERY_STRONG_POST;
    if (corrVal < VERY_STRONG_CORR_NAGT) return VERY_STRONG_NAGT;
    if (corrVal > STRONG_CORR_POSI) return STRONG_POST;
    if (corrVal < STRONG_CORR_NAGT) return STRONG_NAGT;
    if (corrVal > MIDDLE_CORR_POST) return MIDDLE_POST;
    if (corrVal < MIDDLE_CORR_NAGT) return MIDDLE_NAGT;
    if (corrVal > MINOR_CORR_POST) return MINOR_POST;
    if (corrVal < MINOR_CORR_NAGT) return MINOR_NAGT;
    return WEAK;
}

double PearsonAlgorithm::calc(vector<int> x, vector<int> y) 
{
  int x_n = x.size();
  int y_n = y.size();
    
  if (x_n != y_n || x_n == 0) {
    return 0;
  }
 
  double n = (double)(x_n);
  double sum_x = 0;
  double sum_y = 0;
  double sum_x_sq = 0;
  double sum_y_sq = 0;
  double sum_x_by_y = 0;
   
  for (int i = 0; i < x_n; ++i) {
    sum_x += x[i];
    sum_y += y[i];
    sum_x_sq += x[i]*x[i];
    sum_y_sq += y[i]*y[i];
    sum_x_by_y += x[i]*y[i];
  }
  double res = n*sum_x_by_y - sum_x*sum_y;
  res /= sqrt(n*sum_x_sq - sum_x*sum_x);
  res /= sqrt(n*sum_y_sq - sum_y*sum_y);
  return res;
}

#define TINY 1.0e-20

float PearsonAlgorithm::calc(vector<int> x, vector<int> y, float* res)
{
    int n = x.size();

    unsigned long j;
    float yt,xt;
    float t,df;
    float syy=0.0,sxy=0.0,sxx=0.0,ay=0.0,ax=0.0;
    for (j=0;j<n;j++) 
    {
        ax += x[j];
        ay += y[j];
    }
    ax = ax/n;
    ay = ay/n;
    for (j=0;j<n;j++) 
    {
        xt=x[j]-ax;
        yt=y[j]-ay;
        sxx += xt*xt;
        syy += yt*yt;
        sxy += xt*yt;
    }
    *res=sxy/(sqrt(sxx*syy)+TINY);

    df=n-2;
    t=(*res)*sqrt(df/((1.0-(*res)+TINY)*(1.0+(*res)+TINY)));
    return ax;
}