#ifndef VAR_FUND_ITEM_GEN_H
#define VAR_FUND_ITEM_GEN_H

#include <vector>
#include <string>
#include "MergedFundItemGen.h"
using namespace std;

class VarFundItemGen : public MergedFundItemGen
{
public:
    int unionVarRankAndRank;
    VarFundItemGen();
    VarFundItemGen(MergedFundItemGen& m);
    ~VarFundItemGen();
    virtual bool operator < (const VarFundItemGen& m) const;
    MergedFundItemGen getBaseItem();
};

#endif
