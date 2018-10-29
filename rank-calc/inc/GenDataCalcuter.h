#ifndef GEN_CALCUTER_H
#define GEN_CALCUTER_H

#include <vector>
#include <string>
#include <pthread.h>
#include <map>
#include "FundItemGen.h"
#include "FundTopNItemGen.h"
#include "GenPeriodData.h"
using namespace std;

typedef pair<pthread_t, string> pthread2StringPair;

#define TOP_STEP_NUM 12

class GenDataCalcuter
{
public:
  string date;
  vector<vector<FundItemGen> > vChoice1;
  vector<vector<FundItemGen> > vChoice2;  
  vector<vector<FundItemGen> > vChoice3;
  vector<vector<FundItemGen> > vChoice4;
  vector<vector<FundItemGen> > vChoice5;
  static string dumpFileName[5];

  vector<FundTopNItemGen> vFundTopNItemGen;

  vector<int> W_M;
  vector<int> W_M_S;
  vector<int> W_M_S_Y;
  vector<int> W_M_S_Y_3Y;
  vector<int> W_M_S_Y_3Y_A;

  int getTopNumber(int top, int stepNum);
  int getTopNumber(int top, int stepNum, GenPeriodData& genPeriodData);
  void calcTopNMatrix(GenPeriodData& genPeriodData);
  virtual void initWeightFactor(GenPeriodData& genPeriodData);
  virtual void initWeightFactorModel(GenPeriodData& genPeriodData);
  void dropWeightFactor();
  void calcMatrixScore(GenPeriodData& genPeriodData);
  void setDate(string date);

private:
  void calcTopNScore(vector<vector<FundItemGen> >& vChoice, vector<int>& baseScore, int baseSize);

  void compareChoice1(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, int topNumber, vector<FundItemGen>& vChoice);
  void compareChoice2(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, int topNumber, vector<FundItemGen>& vChoice);
  void compareChoice3(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, int topNumber, vector<FundItemGen>& vChoice);
  void compareChoice4(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, vector<FundItemGen> v3Year, int topNumber, vector<FundItemGen>& vChoice);
  void compareChoice5(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, vector<FundItemGen> v3Year, vector<FundItemGen> vAll, int topNumber, vector<FundItemGen>& vChoice);

  void dumpVector2File(vector<FundItemGen>& vItem, string filepath, string format=string("txt"));
  void dumpMatrix2File(vector<vector<FundItemGen> >& vChoice, string filepath, string format=string("txt"));

public:
  GenDataCalcuter();
  virtual ~GenDataCalcuter();

  int findFundTopNItemById(string id);
  void sortFundTopNItem();
  void normFundTopNScore();

  bool isFundExistInCalcuter(string fundId);

public:
  void dumpMatrix2File(string rootpath, string format=string("txt"));
  void displayResultTopN(int topN);
  void dumpResultTopN2File(string filepath, string format=string("txt"));

public:
  pthread_mutex_t* printTaskMutex_p;// = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t* printTaskCond_p;// = PTHREAD_COND_INITIALIZER;
  map<pthread_t, string>* threadId2LongMap_p;

  void setThreadPrintLogger(pthread_mutex_t* mutex_p, pthread_cond_t* cont_p, map<pthread_t, string>* map_p);

public:
  bool operator < (const GenDataCalcuter& m) const;
};

#endif
