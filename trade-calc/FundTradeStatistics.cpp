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

    tm1.tm_year   =      ty1/10000 - 1900;   //�õ���
    tm1.tm_mon   =      (ty1%10000)/100;     //�õ���
    tm1.tm_mday =       (ty1%10000)%100;     //�õ���
    tm1.tm_hour   =     th1/10000;           //�õ�ʱ
    tm1.tm_min    =     (th1%10000)/100;     //�õ���
    tm1.tm_sec    =     (th1%10000)%100;     //�õ���

    tm2.tm_year   =     ty2/10000 - 1900;    //�õ���
    tm2.tm_mon   =     (ty2%10000)/100;      //�õ���
    tm2.tm_mday =      (ty2%10000)%100;      //�õ���
    tm2.tm_hour   =     th2/10000;           //�õ�ʱ
    tm2.tm_min    =     (th2%10000)/100;     //�õ���
    tm2.tm_sec    =     (th2%10000)%100;     //�õ���

    tt1 = mktime(&tm1);
    tt2 = mktime(&tm2);

    dt = (float)(difftime(tt2,tt1))/60;    //�õ����ķ�����
    
    return (int)(dt/60/24);
    }
