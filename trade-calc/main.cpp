#include "BocTradeCalculator.h"
#include "BocTradeCalculator_V2.h"
#include "IcbcTradeCalculator.h"

int main(int argc, char* argv[])
{
#if 0
        BocTradeCalculator* bocTrader_p = new BocTradeCalculator();
        bocTrader_p->configureHistoryTradeFiles(string("../cfg/boc-history-trade.cfg.2.year.before"));
        if (bocTrader_p->loadFundHistoryTradeRecords() == 0)
        {
            bocTrader_p->distinguishFundTradeRecords();
            bocTrader_p->dispFundHistoryTrade();
            bocTrader_p->calculateFundTradeStatistics();
            bocTrader_p->calcAndDispTotalProfit();
        }
#endif

#if 1
    BocTradeCalculator* bocTrader_p = new BocTradeCalculator();
    bocTrader_p->configureHistoryTradeFiles(string("../cfg/boc-history-trade.cfg.2.year.before"));
    if (bocTrader_p->loadFundHistoryTradeRecords() == 0)
    {
        bocTrader_p->distinguishFundTradeRecords();
        bocTrader_p->translateTradeRecords();
        bocTrader_p->calcOneFundInAndOut();
        //bocTrader_p->calcAndDispTotalProfit();
    }

    BocTradeCalculator_V2* bocTraderV2_p = new BocTradeCalculator_V2();
    bocTraderV2_p->setPrevCalculator(bocTrader_p);

    bocTraderV2_p->configureHistoryTradeFiles(string("../cfg/boc-history-trade.cfg.2015.02.06"));
    if (bocTraderV2_p->loadFundHistoryTradeRecords() == 0)
    {
        bocTraderV2_p->distinguishFundTradeRecords();
        bocTraderV2_p->dispFundHistoryTrade();
        bocTraderV2_p->calculateFundTradeStatistics();
        bocTraderV2_p->calcAndDispTotalProfit();
    }
#endif

#if 0
    IcbcTradeCalculator* icbcTrader_p = new IcbcTradeCalculator();
    icbcTrader_p->configureHistoryTradeFiles(string("../cfg/icbc-history-trade.cfg.2015.02.05"));
    if (icbcTrader_p->loadFundHistoryTradeRecords() == 0)
    {
        icbcTrader_p->distinguishFundTradeRecords();
        icbcTrader_p->dispFundHistoryTrade();

        icbcTrader_p->distinguishFundTradeRecordsByApplyId();
        cout << "after merged by apply Id:" << endl;
        icbcTrader_p->dispFundHistoryTradeByAppId();
        if (icbcTrader_p->mergeTradeDateForSameApplyId() == 0)
        {
            icbcTrader_p->calculateFundTradeStatistics();
            icbcTrader_p->calcAndDispTotalProfit();
        }
    }
#endif
  return 1;
}
