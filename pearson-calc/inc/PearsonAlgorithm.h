#ifndef _PEARSON_ALGO_H
#define _PEARSON_ALGO_H

#include <vector>
using namespace std;

#define MIN_CALC_LEN 6
#define VERY_STRONG_CORR_POSI 0.8
#define VERY_STRONG_CORR_NAGT -0.8
#define STRONG_CORR_POSI 0.6
#define STRONG_CORR_NAGT -0.6
#define MIDDLE_CORR_POST 0.4
#define MIDDLE_CORR_NAGT -0.4
#define MINOR_CORR_POST 0.2
#define MINOR_CORR_NAGT -0.2
#define NO_CORR_NAGT_POST 0

class PearsonAlgorithm 
{
public:
    static double calc(vector<int> x, vector<int> y);
    static float calc(vector<int> x, vector<int> y, float* res);

public:        
    enum CORR_TYPE
    {
        VERY_STRONG_POST,
        VERY_STRONG_NAGT,
        STRONG_POST,
        STRONG_NAGT,
        MIDDLE_POST,
        MIDDLE_NAGT,
        MINOR_POST,
        MINOR_NAGT,
        WEAK,
    };

    static CORR_TYPE getCorrType(float corrVal);
};

#endif 
