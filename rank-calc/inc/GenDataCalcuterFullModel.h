#ifndef GEN_DATA_CAL_FULL_MODEL_H
#define GEN_DATA_CAL_FULL_MODEL_H

#include "GenDataCalcuter.h"

class GenDataCalcuterFullModel : public GenDataCalcuter
{
public:
    virtual ~GenDataCalcuterFullModel();
    virtual void initWeightFactor(GenPeriodData& genPeriodData);
    GenDataCalcuter getBaseCalcuter();
};

#endif

