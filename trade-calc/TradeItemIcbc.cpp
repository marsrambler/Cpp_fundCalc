#include "TradeItemIcbc.h"


bool TradeItemIcbc::operator < (const TradeItemIcbc& m) const
{
    if (finishDate.compare(m.finishDate) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

