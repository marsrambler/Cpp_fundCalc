#include <iostream>
#include <algorithm>
#include <functional>
#include <list>
#include "GenDataCalcuter.h"
#include "FileDumper.h"

using namespace std;

string GenDataCalcuter::dumpFileName[5] = {
                        "W-M",
                        "W-M-S",
                        "W-M-S-Y",
                        "W-M-S-Y-3Y",
                        "W-M-S-Y-3Y-A"
                        };

GenDataCalcuter::GenDataCalcuter()
{
  W_M.clear();
  W_M_S.clear();
  W_M_S_Y.clear();
  W_M_S_Y_3Y.clear();
  W_M_S_Y_3Y_A.clear();
  
  printTaskMutex_p = NULL;  
  printTaskCond_p = NULL;
  threadId2LongMap_p = NULL;
}

GenDataCalcuter::~GenDataCalcuter()
{
  dropWeightFactor();
}

int GenDataCalcuter::getTopNumber(int top, int stepNum)
{
  GenPeriodData genPeriodData;
  genPeriodData.iMinFundNum = W_M.size();
  return getTopNumber(top, stepNum, genPeriodData);
}

int GenDataCalcuter::getTopNumber(int top, int stepNum, GenPeriodData& genPeriodData)
{
  int topNumber = TOP_STEP_NUM;
  
  if (stepNum == TOP_STEP_NUM)
  {
    switch (top)
    {
      case 0: topNumber = 10; break;
      case 1: topNumber = 20; break;
      case 2: topNumber = 30; break;
      case 3: topNumber = 50; break;
      case 4: topNumber = 75; break;
      case 5: topNumber = 100; break;
      case 6: topNumber = 200; break;
      case 7: topNumber = 300; break;
      case 8: topNumber = 400; break;
      case 9: topNumber = 500; break;
      case 10: topNumber = 600; break;
      case 11: topNumber = genPeriodData.iMinFundNum; break;
      default: topNumber = 10 ;break;
    } 
  } 
  else if (stepNum == genPeriodData.iMinFundNum)
  {
    topNumber = top < stepNum ? top : stepNum;
    topNumber++;
  } 
  return topNumber;
}

void GenDataCalcuter::compareChoice1(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, int topNumber, vector<FundItemGen>& vChoice)
{
  for (int i = 0; i < topNumber; i++)
  {
    for (int j = 0; j < topNumber; j++)
    {
      if (vWeek[i].id.compare(vMonth[j].id) == 0)
      {
        vChoice.push_back(vWeek[i]);
        break;
      }
    }
  }
}

void GenDataCalcuter::compareChoice2(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, int topNumber, vector<FundItemGen>& vChoice)
{
  vector<FundItemGen> tmp;
  compareChoice1(vWeek, vMonth, topNumber, tmp);
  
  for (int i = 0; i < tmp.size(); i++)
  {
    for (int j = 0; j < topNumber; j++)
    {
      if (tmp[i].id.compare(vSeason[j].id) == 0)
      {
        vChoice.push_back(tmp[i]);
        break;
      }
    }
  }
}

void GenDataCalcuter::compareChoice3(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, int topNumber, vector<FundItemGen>& vChoice)
{
  vector<FundItemGen> tmp;
  compareChoice2(vWeek, vMonth, vSeason, topNumber, tmp);
  
  for (int i = 0; i < tmp.size(); i++)
  {
    for (int j = 0; j < topNumber; j++)
    {
      if (tmp[i].id.compare(vYear[j].id) == 0)
      {
        vChoice.push_back(tmp[i]);
        break;
      }             
    } 
  } 
}

void GenDataCalcuter::compareChoice4(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, vector<FundItemGen> v3Year, int topNumber, vector<FundItemGen>& vChoice)
{
  vector<FundItemGen> tmp;
  compareChoice3(vWeek, vMonth, vSeason, vYear, topNumber, tmp);
  
  for (int i = 0; i < tmp.size(); i++)
  {
    for (int j = 0; j < topNumber; j++)
    {
      if (tmp[i].id.compare(v3Year[j].id) == 0)
      {
        vChoice.push_back(tmp[i]);
        break;
      } 
    } 
  }   
}

void GenDataCalcuter::compareChoice5(vector<FundItemGen> vWeek, vector<FundItemGen> vMonth, vector<FundItemGen> vSeason, vector<FundItemGen> vYear, vector<FundItemGen> v3Year, vector<FundItemGen> vAll, int topNumber, vector<FundItemGen>& vChoice)
{
  vector<FundItemGen> tmp;
  compareChoice4(vWeek, vMonth, vSeason, vYear, v3Year, topNumber, tmp);
  
  for (int i = 0; i < tmp.size(); i++)
  {
    for (int j = 0; j < topNumber; j++)
    {
      if (tmp[i].id.compare(vAll[j].id) == 0)
      {
        vChoice.push_back(tmp[i]);
        break;
      }
    }
  }
}

void GenDataCalcuter::setThreadPrintLogger(pthread_mutex_t* mutex_p, pthread_cond_t* cont_p, map<pthread_t, string>* map_p)
{
  printTaskMutex_p = mutex_p;
  printTaskCond_p  = cont_p;
  threadId2LongMap_p = map_p;
}

void GenDataCalcuter::calcTopNMatrix(GenPeriodData& genPeriodData)
{
  //cout << endl << "calculate date " << date << endl;
  for (int top = 0; top < genPeriodData.iMinFundNum; top++)
  {
    int topNumber = getTopNumber(top, genPeriodData.iMinFundNum, genPeriodData);
    
    //cout << "\r  calculate Top " << topNumber;
    if (printTaskMutex_p != NULL && printTaskCond_p != NULL && threadId2LongMap_p != NULL)
    {
      char buffer[2048]={0};
      sprintf(buffer, "%d/%d ", topNumber, genPeriodData.iMinFundNum);
      string logMsg(buffer);

      pthread_mutex_lock(printTaskMutex_p);

      pthread_t pid = pthread_self();
      map<pthread_t, string>::iterator iter = threadId2LongMap_p->find(pid);
      if (iter == threadId2LongMap_p->end())
      {
        threadId2LongMap_p->insert(pthread2StringPair(pid, logMsg));
      }
      else
      {
        (*threadId2LongMap_p)[pid] = logMsg;
      }

      pthread_mutex_unlock(printTaskMutex_p);
      //pthread_cond_broadcast(printTaskCond_p);
    }

    vector<FundItemGen> vItem;
    compareChoice1(genPeriodData.vWeek, genPeriodData.vMonth, topNumber, vItem);
    vChoice1.push_back(vItem);
    
    vItem.clear();
    compareChoice2(genPeriodData.vWeek, genPeriodData.vMonth, genPeriodData.vSeason, topNumber, vItem);
    vChoice2.push_back(vItem);
    
    vItem.clear();
    compareChoice3(genPeriodData.vWeek, genPeriodData.vMonth, genPeriodData.vSeason, genPeriodData.vYear, topNumber, vItem);
    vChoice3.push_back(vItem);

    vItem.clear();
    compareChoice4(genPeriodData.vWeek, genPeriodData.vMonth, genPeriodData.vSeason, genPeriodData.vYear, genPeriodData.v3Year, topNumber, vItem);
    vChoice4.push_back(vItem);

    vItem.clear();
    compareChoice5(genPeriodData.vWeek, genPeriodData.vMonth, genPeriodData.vSeason, genPeriodData.vYear, genPeriodData.v3Year, genPeriodData.vAll, topNumber, vItem);
    vChoice5.push_back(vItem);
  }
  //cout << endl;
}

void GenDataCalcuter::setDate(string dataDate)
{
  this->date = dataDate;
}

void GenDataCalcuter::initWeightFactor(GenPeriodData& genPeriodData)
{
  if (genPeriodData.iMinFundNum <= 0) return;
  
  //cout << "init weight factor matrix: " << genPeriodData.iMinFundNum << endl;

  W_M.resize(genPeriodData.iMinFundNum);
  W_M_S.resize(genPeriodData.iMinFundNum);
  W_M_S_Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y_A.resize(genPeriodData.iMinFundNum);

  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-1] = 0;  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-2] = 1;
  W_M_S_Y_3Y[genPeriodData.iMinFundNum-1]   = 0;  W_M_S_Y_3Y[genPeriodData.iMinFundNum-2]   = 2;
  W_M_S_Y[genPeriodData.iMinFundNum-1]      = 0;  W_M_S_Y[genPeriodData.iMinFundNum-2]      = 5;
  W_M_S[genPeriodData.iMinFundNum-1]        = 0;  W_M_S[genPeriodData.iMinFundNum-2]        = 4;
  W_M[genPeriodData.iMinFundNum-1]          = 0;  W_M[genPeriodData.iMinFundNum-2]          = 3;

  for (int j = genPeriodData.iMinFundNum-3; j >= 0; j--)
  {
    W_M_S_Y_3Y_A[j] = W_M_S_Y_3Y_A[j+1] + 1;
    W_M_S_Y_3Y[j]   = W_M_S_Y_3Y[j+1] + 1;
    W_M_S_Y[j]      = W_M_S_Y[j+1] + 1;
    W_M_S[j]        = W_M_S[j+1] + 1;
    W_M[j]          = W_M[j+1] + 1;
  }
}

void GenDataCalcuter::initWeightFactorModel(GenPeriodData& genPeriodData)
{

}

void GenDataCalcuter::calcTopNScore(vector<vector<FundItemGen> >& vChoice, vector<int>& baseScore, int baseSize)
{
  for (int i = 0; i < vChoice.size() && i < baseSize; i++)
  {
    vector<FundItemGen> vItem = vChoice[i];
    for (int j = 0; j < vItem.size(); j++)
    {
        FundItemGen  oneFund = vItem[j];
        int score = baseScore[i];
        int idx = findFundTopNItemById(oneFund.id);
        if (idx == -1)
        {
          FundTopNItemGen topNItem;
          topNItem.id        = oneFund.id;
          topNItem.name      = oneFund.name;
          topNItem.weekPro   = oneFund.weekPro;
          topNItem.monthPro  = oneFund.monthPro;
          topNItem.seasonPro = oneFund.seasonPro;
          topNItem.yearPro   = oneFund.yearPro;
          topNItem.yearPro_3 = oneFund.yearPro_3;
          topNItem.allPro    = oneFund.allPro;
          topNItem.score     = score;
          topNItem.ticks     = 1;
          vFundTopNItemGen.push_back(topNItem);
        }
        else
        {
          vFundTopNItemGen[idx].score += score;
          vFundTopNItemGen[idx].ticks++;
        }
    }
  }
}

void GenDataCalcuter::calcMatrixScore(GenPeriodData& genPeriodData)
{
  if (!(W_M.size()||W_M_S.size()||W_M_S_Y.size()||W_M_S_Y_3Y.size()||W_M_S_Y_3Y_A.size()))
  { 
    cout << "The weight factor matrix are all NULL!" << " date " << date  << endl;
    return;
  }

  calcTopNScore(vChoice1, W_M,          genPeriodData.iMinFundNum);
  calcTopNScore(vChoice2, W_M_S,        genPeriodData.iMinFundNum);
  calcTopNScore(vChoice3, W_M_S_Y,      genPeriodData.iMinFundNum);
  calcTopNScore(vChoice4, W_M_S_Y_3Y,   genPeriodData.iMinFundNum);
  calcTopNScore(vChoice5, W_M_S_Y_3Y_A, genPeriodData.iMinFundNum);
}

int GenDataCalcuter::findFundTopNItemById(string id)
{
  for (int i = 0; i < vFundTopNItemGen.size(); i++)
  {
    if (vFundTopNItemGen[i].id.compare(id) == 0)
      return i;
  }
  return -1;
}

void GenDataCalcuter::sortFundTopNItem()
{
  sort(vFundTopNItemGen.begin(), vFundTopNItemGen.end());
}

void GenDataCalcuter::normFundTopNScore()
{
  sortFundTopNItem();
  double maxScore = vFundTopNItemGen[0].score;
  if (maxScore < 0.00001) maxScore = 0.0001;
  for (int i = 0; i < vFundTopNItemGen.size(); i++)
  {
    vFundTopNItemGen[i].score = vFundTopNItemGen[i].score / maxScore * 100;
  }
}

void GenDataCalcuter::dropWeightFactor()
{
  W_M.clear();
  W_M_S.clear();
  W_M_S_Y.clear();
  W_M_S_Y_3Y.clear();
  W_M_S_Y_3Y_A.clear();
}

bool GenDataCalcuter::isFundExistInCalcuter(string fundId)
{
  for (int idx = 0; idx < vFundTopNItemGen.size(); idx++)
  {
    if (vFundTopNItemGen[idx].id.compare(fundId) == 0)
      return true;
  }
  return false;
}

void GenDataCalcuter::displayResultTopN(int topN)
{
  cout << endl << "dump date: " << date << " top(" << topN << "):" << endl;

  char buffer[2048] = {0};
  sprintf(buffer, "%8s %36s %10s %10s %10s %10s %10s %8s %8s %8s", "id", "name", "week", "month", "season", "year", "3 year", "all", "score", "ticks");
  
  cout << buffer << endl;

  for (int i = 0; i < vFundTopNItemGen.size() && i < topN; i++)
  {
    char buffer[2048] = {0};
    sprintf(buffer, "%8s %36s %10s %10s %10s %10s %10s %8s %8.2f %8d", 
                    vFundTopNItemGen[i].id.c_str(), vFundTopNItemGen[i].name.c_str(), vFundTopNItemGen[i].weekPro.c_str(), 
                    vFundTopNItemGen[i].monthPro.c_str(), vFundTopNItemGen[i].seasonPro.c_str(), vFundTopNItemGen[i].yearPro.c_str(), 
                    vFundTopNItemGen[i].yearPro_3.c_str(), vFundTopNItemGen[i].allPro.c_str(), vFundTopNItemGen[i].score, vFundTopNItemGen[i].ticks);
    cout << buffer << endl;
  }
  cout << endl;
} 

void GenDataCalcuter::dumpResultTopN2File(string filepath, string format)
{
  FileDumper::dumpVFundTopNItem2File(vFundTopNItemGen, filepath.c_str(), format);
}

void GenDataCalcuter::dumpMatrix2File(vector<vector<FundItemGen> >& vChoice, string filepath, string format)
{
  for (int idx = 0; idx < vChoice.size(); idx++)
  {
    int topNumber = getTopNumber(idx, W_M.size());

    vector<FundItemGen> vItem = vChoice[idx];
    //cout << filepath << " Top(" << topNumber << ") "<< " size: " << vItem.size() << endl;
    char filename[1024] = {0};
    sprintf(filename, "%s-Top%d.%s", filepath.c_str(), topNumber, format.c_str());
    FileDumper::dumpVFundItemGen2File(vItem, filename, format);
  }
}

void GenDataCalcuter::dumpMatrix2File(string rootpath, string format)
{
  dumpMatrix2File(vChoice1, rootpath+string("//")+dumpFileName[0], format);
  dumpMatrix2File(vChoice2, rootpath+string("//")+dumpFileName[1], format);
  dumpMatrix2File(vChoice3, rootpath+string("//")+dumpFileName[2], format);
  dumpMatrix2File(vChoice4, rootpath+string("//")+dumpFileName[3], format);
  dumpMatrix2File(vChoice5, rootpath+string("//")+dumpFileName[4], format);
}

bool GenDataCalcuter::operator < (const GenDataCalcuter& m) const
{
  if (date.compare(m.date) < 0) return true;
  else return false;
}

