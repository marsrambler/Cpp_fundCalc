#ifndef GEN_PERIOD_REPO_H
#define GEN_PERIOD_REPO_H

#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>
#include "GenPeriodData.h"
#include "GenDataCalcuter.h"
#include "GenDataCalcuterModel.h"
#include "GenDataCalcuterFullModel.h"
#include "MergedFundItemGen.h"
#include "VarFundItemGen.h"
#include "VarRankFundItemGen.h"
#include "VarScoreFundItemGen.h"
#include <map>
using namespace std;

typedef pair<pthread_t, string> pthread2StringPair;

#define DUMP_CONF_NUM 8

class GenPeriodRepo
{
public:
  enum REPO_TYPE
  {
    STOCK=0,
    COMP=1,
    DEBT=2,
    INDEX=3,
    TOTAL=4
  };

  enum MODEL_TYPE
  {
    FULL=0,
    SEASON=1,
    FULL_SEASON=2
  };

  REPO_TYPE repo_type;
  string cfgSectionName;
  static string repoString[TOTAL];

  MODEL_TYPE model_type;

  string genDataRootPath;
  vector<GenPeriodData> vGenPeriodData;
  vector<GenDataCalcuter> vGenCalcuter;

public:
  void mergeAllCalcuter();
  vector<MergedFundItemGen> vMergeFundTopN;
  int getFundPosInMergedFundTopN(string fundId);
  bool isFundExistInRepoMergedGen(string fundId);
  void dumpMergedFundTopN2File();
  void dumpMergedFundTopN2FileByVariance();
  void displayMergedFundTopN(int topN);

public:
  void calcUnionVariance();
  vector<VarRankFundItemGen> vRankVarItems;
  vector<VarScoreFundItemGen> vScoreVarItems;
  vector<VarFundItemGen> vVarFundItems;
  vector<MergedFundItemGen> vUnionVarItem;

public:
  void calcVarianceAfterOfflineLoad();

public:
  int getRankGenDispLength();
  int getScoreGenDispLength();

public:
  GenPeriodRepo(REPO_TYPE type);
  void buildGenPeriodFilename(string cfgFilename);
  void clearRepoContent();
  vector<string> getDateList();
  bool loadOneDate(string date);
  void loadAllDate(); 
  void debugDataStored();
  void setModelType(MODEL_TYPE type);

public:
  bool calcOneDate(string date);
  void calcAllDate();

public:
  class SubCalcThreadParam
  {
    public:
      void* class_p;
      int genPeriod_idx;
  };
  
public:
  vector<pthread_t> vThreadIds;
  static void* calcOneDataTreadFunc(void* param_p);
  pthread_t printThreadId;
  static void* printThreadProgressLogFunc(void* param_p);
  bool bPringThreadExit;// = false;
  pthread_mutex_t printThreadStartedMutex;// = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t printTaskMutex;// = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t printTaskCond;// = PTHREAD_COND_INITIALIZER;
  map<pthread_t, string> threadId2LongMap;
  pthread_mutex_t pushResultMutex;

public:
  void dumpOneGenData2File(string date, string format=string("txt"));
  void dumpAllGenData2File(string format=string("txt"));
  
  void dumpOneCalcData2File(string date, string format=string("txt"));
  void dumpAllCalcData2File(string format=string("txt"));

  void displayOneCalcResultTopN(string date, int topN);
  void displayAllCaclResultTopN(int topN);

  void dumpOneCalcResultTopN(string date);
  void dumpAllCalcResultTopN();

public:
  static string dumpDirSecName[DUMP_CONF_NUM];
  static string dumpGenPeriodRootdir;
  static string dumpSpePeriodRootdir;
  static string dumpGenTopNRootdir;
  static string dumpSpeTopNRootdir;
  static string dumpGenMergeRootdir;
  static string dumpSpeMergeRootdir;
  static string dumpFavorFundRootdir;
  static string dumpTransformRootdir;
  static void buildDumpDirectory(string cfgFilename);

private:
  static string findAndTrim(string fullString, string dirSecName);

private:
  GenPeriodRepo();
};

#endif
