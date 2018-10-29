#ifndef FAVOR_FUND_PROCESSOR
#define FAVOR_FUND_PROCESSOR

#include <vector>
#include "FavorFundItem.h"
using namespace std;

class FavorFundsProcessor
{
public:
  vector<FavorFundItem> vFavorFund;
  vector<vector<FavorFundItem> > vvFavorFund;

public:
  void buildFavoriteFunds(string cfgFilename);

public:
  bool getOneFavorFundItem(string& oneLine, FavorFundItem& item);
  string trimString(string szString);

private:
  int getMaxRankGenDispLen();
  int getMaxScoreGenDispLen();

public:
  void displayFavorFundsStatus();
  void dumpFavorFund2File(string rootPath); 
};

#endif