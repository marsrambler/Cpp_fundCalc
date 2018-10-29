#include <iostream>
#include <vector>
#include "PearsonAlgorithm.h"

using namespace std;

int main(int argc, char* argv[])
{
    float number = 0;

    unsigned long n = 5;
    float y[5]={2,3,5,9,7};
    float x[5]={7,9,8,4,1};

    vector<int> v_x, v_y;
    for (int i =0; i < 5; i++)
    {
        v_x.push_back(x[i]);
        v_y.push_back(y[i]);
    }   

    float *r = &number;
    float *prob = &number;
    float t=0;

    t = PearsonAlgorithm::calc(v_x, v_y, r);
    printf("calc2: %f\n", *r);

    double d =PearsonAlgorithm::calc(v_x, v_y);
    printf("calc1: %f\n", d);

    return 0;
}
