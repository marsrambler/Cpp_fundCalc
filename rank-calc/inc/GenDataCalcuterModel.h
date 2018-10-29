#ifndef GEN_DATA_CAL_MODEL_H
#define GEN_DATA_CAL_MODEL_H

#include "GenDataCalcuter.h"

class GenDataCalcuterModel : public GenDataCalcuter
{
public:
    virtual ~GenDataCalcuterModel();
    virtual void initWeightFactor(GenPeriodData& genPeriodData);
    GenDataCalcuter getBaseCalcuter();
};

#endif
