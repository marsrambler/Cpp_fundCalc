#include "TradeItemBoc.h"

bool TradeItemBoc::operator < (const TradeItemBoc& m) const
{
  if (tradeDate.compare(m.tradeDate) < 0) return true;
  else return false;
}
