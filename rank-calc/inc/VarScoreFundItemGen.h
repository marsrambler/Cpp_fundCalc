#ifndef VAR_SCORE_FUNDITEM_H
#define VAR_SCORE_FUNDITEM_H

#include <vector>
#include <string>
#include "VarFundItemGen.h"
using namespace std;

class VarScoreFundItemGen : public VarFundItemGen
{
public:
    VarScoreFundItemGen(VarFundItemGen& m);
    VarScoreFundItemGen(MergedFundItemGen& m);
    ~VarScoreFundItemGen();
    virtual bool operator < (const VarScoreFundItemGen& m) const;
    MergedFundItemGen getBaseItem();      
};

#endif