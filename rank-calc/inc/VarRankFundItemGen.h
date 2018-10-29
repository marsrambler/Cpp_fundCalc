#ifndef VAR_RANK_FUND_ITEM_H
#define VAR_RANK_FUND_ITEM_H

#include <vector>
#include <string>
#include "VarFundItemGen.h"
using namespace std;

class VarRankFundItemGen : public VarFundItemGen
{
public:
    VarRankFundItemGen(MergedFundItemGen& m);
    VarRankFundItemGen(VarFundItemGen& m);
    ~VarRankFundItemGen();
    virtual bool operator < (const VarRankFundItemGen& m) const;
    MergedFundItemGen getBaseItem();     
};

#endif