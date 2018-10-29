#include <time.h>
#include "FundTradeStatistics.h"

FundTradeStatistics::FundTradeStatistics()
{
    totalEarnMoney = 0;
    totalInvestAndDays = 0;
    profitRate = 0;
    totalInvest = 0;
    totalReturned = 0;
    totalHolding = 0;
}


bool FundTradeStatistics::operator < (const FundTradeStatistics& m) const
{
  if (status < m.status)
  { 
    return true;
  }
  else if (status == m.status)
  {
    if (totalEarnMoney > m.totalEarnMoney)
    {
      return true;
    }
  }
  return false;
}

int FundTradeStatistics::diffDays(unsigned ty1, unsigned ty2)
{
    unsigned th1, th2;
    th1 = th2 = 0;

    struct tm tm1,tm2;
    time_t tt1,tt2;
    float dt;

    tm1.tm_year   =      ty1/10000 - 1900;   //得到年
    tm1.tm_mon   =      (ty1%10000)/100;     //得到月
    tm1.tm_mday =       (ty1%10000)%100;     //得到日
    tm1.tm_hour   =     th1/10000;           //得到时
    tm1.tm_min    =     (th1%10000)/100;     //得到分
    tm1.tm_sec    =     (th1%10000)%100;     //得到秒

    tm2.tm_year   =     ty2/10000 - 1900;    //得到年
    tm2.tm_mon   =     (ty2%10000)/100;      //得到月
    tm2.tm_mday =      (ty2%10000)%100;      //得到日
    tm2.tm_hour   =     th2/10000;           //得到时
    tm2.tm_min    =     (th2%10000)/100;     //得到分
    tm2.tm_sec    =     (th2%10000)%100;     //得到秒

    tt1 = mktime(&tm1);
    tt2 = mktime(&tm2);

    dt = (float)(difftime(tt2,tt1))/60;    //得到相差的分钟数
    
    return (int)(dt/60/24);
    }
