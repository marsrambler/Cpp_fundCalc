#include "TradeItemBoc_V2.h"

bool TradeItemBoc_V2::operator < (const TradeItemBoc_V2& m) const
{
  if (tradeDate.compare(m.tradeDate) < 0) return true;
  else return false;
}

