#ifndef A_GEN_PERIOD_DATA_H
#define A_GEN_PERIOD_DATA_H

#include <string>
#include <vector>
#include "FundItemGen.h"
using namespace std;

#define FILE_NUM_FIX_PER 6

class GenPeriodData
{
public:
  string date;
  vector<string> vFileName;
  vector<FundItemGen> vWeek;
  vector<FundItemGen> vMonth;
  vector<FundItemGen> vSeason;
  vector<FundItemGen> vYear;
  vector<FundItemGen> v3Year;
  vector<FundItemGen> vAll;
  int iMinFundNum;
  static string filename[FILE_NUM_FIX_PER];
  static string dumpFileName[FILE_NUM_FIX_PER];

public:
  vector<FundItemGen> processOneFile(string oneFile);
  bool loadGenData();

  void dumpGenData2File(string rootPath, string format=string("txt"));
  void dumpVector2File(vector<FundItemGen> vItem, const char* filename, string format=string("txt"));
  void displayVector(vector<FundItemGen> vItem);
  void displayGenData();

  GenPeriodData();
};

#endif
