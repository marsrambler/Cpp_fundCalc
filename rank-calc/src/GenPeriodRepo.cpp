#include <fstream>
#include <algorithm>
#include <functional>
#include <list>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "GenPeriodRepo.h"
#include "FileDumper.h"
using namespace std;

#define _DEBUG_UNION

string GenPeriodRepo::repoString[] = {string("stock"),
                                      string("comp"),
                                      string("debt"), 
                                      string("index")};

string GenPeriodRepo::dumpDirSecName[] = {
                                      string("dump-gen-period-rootdir"),
                                      string("dump-spe-period-rootdir"),
                                      string("dump-gen-topN-rootdir"),
                                      string("dump-spe-topN-rootdir"),
                                      string("dump-gen-merge-rootdir"),
                                      string("dump-spe-merge-rootdir"),
                                      string("dump-favor-fund-rootdir"),
                                      string("dump-transform-rootdir")
                                         };

string GenPeriodRepo::dumpGenPeriodRootdir = string("");
string GenPeriodRepo::dumpSpePeriodRootdir = string("");
string GenPeriodRepo::dumpGenTopNRootdir   = string("");
string GenPeriodRepo::dumpSpeTopNRootdir   = string("");
string GenPeriodRepo::dumpGenMergeRootdir  = string("");
string GenPeriodRepo::dumpSpeMergeRootdir  = string("");
string GenPeriodRepo::dumpFavorFundRootdir = string("");
string GenPeriodRepo::dumpTransformRootdir = string("");

GenPeriodRepo::GenPeriodRepo(REPO_TYPE type)
{
  repo_type = type;
  switch (repo_type)
  {
    case STOCK: cfgSectionName = string("[gen-period-stock]"); break;
    case COMP:  cfgSectionName = string("[gen-period-comp]");  break; 
    case DEBT:  cfgSectionName = string("[gen-period-debt]");  break;
    case INDEX: cfgSectionName = string("[gen-period-index]"); break;
    default: break;
  }
  model_type = FULL;
}

void GenPeriodRepo::clearRepoContent()
{
  vGenPeriodData.clear();
  vGenCalcuter.clear();
}

void GenPeriodRepo::setModelType(MODEL_TYPE type)
{
  model_type = type;
}

void GenPeriodRepo::buildGenPeriodFilename(string cfgFilename)
{
  clearRepoContent();

  ifstream ifile(cfgFilename.c_str());
  string oneLine = ""; 
  bool bGenPerSec = false;
  vector<string> vDate;

  while(getline(ifile, oneLine))
  {
    int s = oneLine.find_first_not_of(" \t");
    int e = oneLine.find_last_not_of(" \t\r");
    if (s != string::npos && e != string:: npos /*&& s != e*/)
    {
        oneLine = oneLine.substr(s, e-s+1);
        if (oneLine.find(string("#")) != string::npos)
        {
          continue;
        }
    }

    if (oneLine.find(cfgSectionName) != string::npos)
    {   
      bGenPerSec = true;
    }   
    else if (oneLine.find(string("[")) != string::npos)
    {   
      bGenPerSec = false;
    }
    else if (bGenPerSec)
    {
      if (oneLine.find(string("date=")) != string::npos)
      {
        size_t fpos = oneLine.find(string("date="))+5;
        string dateList = oneLine.substr(fpos);
        fpos = dateList.find(string(","));
        while (fpos != string::npos)
        {
          string date = dateList.substr(0, fpos);
          int s = date.find_first_not_of(" \t");
          int e = date.find_last_not_of(" \t");
          if (s != string::npos && e != string:: npos /*&& s != e*/)
          {
            date = date.substr(s, e-s+1);
            vDate.push_back(date);
          }
          dateList = dateList.substr(fpos+1);
          fpos = dateList.find(string(","));
        }
        int s = dateList.find_first_not_of(" \t");
        int e = dateList.find_last_not_of(" \t");
        if (s != string::npos && e != string:: npos /*&& e != s*/)
        {
          dateList = dateList.substr(s, e-s+1);
          vDate.push_back(dateList);
        } 
      }
      else if (oneLine.find(string("rootPath=")) != string::npos)
      {
        size_t fpos = oneLine.find(string("rootPath="))+9;
        string rootPath = oneLine.substr(fpos);
        int s = rootPath.find_first_not_of(" \t");
        int e = rootPath.find_last_not_of(" \t");
        if (s != string::npos && e != string:: npos /*&& s != e*/)
        {
          rootPath = rootPath.substr(s, e-s+1);
          this->genDataRootPath = rootPath; 
        }
      }  
    }     
  }

  for (int i = 0; i < vDate.size(); i++)
  {
    GenPeriodData genPeriodData;
    genPeriodData.date = vDate[i];
    //cout << "date: " << vDate[i] << endl;
    for (int j = 0; j < FILE_NUM_FIX_PER; j++)
    {
      string file = GenPeriodData::filename[j];
      string fullpath = genDataRootPath+string("//")+vDate[i]+string("//")+file;
      //cout << "path: " << fullpath << endl;
      genPeriodData.vFileName.push_back(fullpath);
    }
    vGenPeriodData.push_back(genPeriodData);
  }
}

string GenPeriodRepo::findAndTrim(string fullString,string dirSecName)
{
    if (fullString.find(dirSecName) == string::npos) return string("");

    size_t fpos = fullString.find(dirSecName)+dirSecName.length()+1; // 1 is '='
    string szString = fullString.substr(fpos);
    int s = szString.find_first_not_of(" \t");
    int e = szString.find_last_not_of(" \t");
    if (s != string::npos && e != string:: npos /*&& s != e*/)
    {
        szString = szString.substr(s, e-s+1);
        return szString;
    }
    return string("");
}

void GenPeriodRepo::buildDumpDirectory(string cfgFilename)
{
    if (dumpGenPeriodRootdir != string("")) return;
    
    ifstream ifile(cfgFilename.c_str());
    string oneLine = ""; 
    bool bDumpSec = false;

    string* dumpSecDir[DUMP_CONF_NUM] = {NULL};    
    dumpSecDir[0] = &dumpGenPeriodRootdir;
    dumpSecDir[1] = &dumpSpePeriodRootdir;
    dumpSecDir[2] = &dumpGenTopNRootdir;
    dumpSecDir[3] = &dumpSpeTopNRootdir;
    dumpSecDir[4] = &dumpGenMergeRootdir;
    dumpSecDir[5] = &dumpSpeMergeRootdir;
    dumpSecDir[6] = &dumpFavorFundRootdir;
    dumpSecDir[7] = &dumpTransformRootdir;

    while(getline(ifile, oneLine))
    {
      int s = oneLine.find_first_not_of(" \t");
      int e = oneLine.find_last_not_of(" \t\r");
      if (s != string::npos && e != string:: npos /*&& s != e*/)
      {
          oneLine = oneLine.substr(s, e-s+1);
          if (oneLine.find(string("#")) != string::npos)
          {
            continue;
          }
      }

      if (oneLine.find(string("[dump-directory]")) != string::npos)
      {   
        bDumpSec = true;
      }   
      else if (oneLine.find(string("[")) != string::npos)
      {   
        bDumpSec = false;
      }
      else if (bDumpSec)
      {
        for (int i = 0; i < DUMP_CONF_NUM; i++)
        {
            if (oneLine.find(dumpDirSecName[i]) != string::npos)
            {
                *(dumpSecDir[i]) = findAndTrim(oneLine, dumpDirSecName[i]);
            }
        }
      }
    }

    for (int i = 0; i < DUMP_CONF_NUM; i++)
    {
        DIR* dumpDir_p = opendir((*(dumpSecDir[i])).c_str());
        if (dumpDir_p == NULL)
        {
            mkdir((*(dumpSecDir[i])).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        else
        {
            closedir(dumpDir_p);
        }
        if (i < 2)
        {
            string dumpDirName = *(dumpSecDir[i]);
            for (int j = 0; j < INDEX+1; j++)
            {
                string dumpTypeDirName = dumpDirName + string("//")+repoString[j];
                DIR* dumpDir_p = opendir(dumpTypeDirName.c_str());
                if (dumpDir_p == NULL)
                {
                    mkdir(dumpTypeDirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }
                else
                {
                    closedir(dumpDir_p);
                }
            }
        }
    }
}

vector<string> GenPeriodRepo::getDateList()
{
  vector<string> vDate;
  for (int idx = 0; idx < vGenPeriodData.size(); idx++)
  {
    vDate.push_back(vGenPeriodData[idx].date);
  }
  return vDate;
}

bool GenPeriodRepo::loadOneDate(string date)
{
  int idx;
  for (idx = 0; idx < vGenPeriodData.size(); idx++)
  {
    if (vGenPeriodData[idx].date.compare(date) == 0)
      break;
  }

  if (idx == vGenPeriodData.size())
  {
    cout << "Error date: " << date << endl;
    return false;
  }
  
  return vGenPeriodData[idx].loadGenData();
}

void GenPeriodRepo::loadAllDate()
{
  vector<string> vDate = getDateList();
  for (int idx = 0; idx < vDate.size(); idx++)
  {
    if (!loadOneDate(vDate[idx]))
    {
      cout << "load data failed for date: " << vDate[idx] << endl;
    }
  }
}

void* GenPeriodRepo::printThreadProgressLogFunc(void* param_p)
{
  GenPeriodRepo* genPeriodRepo_p = (GenPeriodRepo*)param_p;
  pthread_mutex_unlock(&(genPeriodRepo_p->printThreadStartedMutex));
  //pthread_mutex_lock(&(genPeriodRepo_p->printTaskMutex));

  while (!genPeriodRepo_p->bPringThreadExit)
  {
    //int ret = pthread_cond_wait(&(genPeriodRepo_p->printTaskCond), &(genPeriodRepo_p->printTaskMutex));
    int ret = pthread_mutex_lock(&(genPeriodRepo_p->printTaskMutex));
    if (ret == 0)
    {
      //cout << "print log lock successfully!" << endl;
    }
    else
    {
      cerr << "print log lock failed!" << endl;
    }
    //cout << "\r log size: " << genPeriodRepo_p->threadId2LongMap.size() << endl;
    string msg("");
    int iCacled = 0;
    int iTotal = 0;
    map<pthread_t, string>::iterator iter = genPeriodRepo_p->threadId2LongMap.begin();
    for (; iter != genPeriodRepo_p->threadId2LongMap.end(); iter++)
    {
      msg += (string)(iter->second);
      int i,j;
      sscanf(msg.c_str(), "%d/%d", &i, &j);
      iCacled += i;
      iTotal  += j;
    }
    
    pthread_mutex_unlock(&(genPeriodRepo_p->printTaskMutex));
    //cout << "\r  " << msg;
    cout << "\r  " << iCacled << "/" << iTotal;
    usleep(2000);
  }
  cout << endl;
  //pthread_mutex_unlock(&(genPeriodRepo_p->printTaskMutex));
  pthread_exit(0);
}

void* GenPeriodRepo::calcOneDataTreadFunc(void* param_p)
{
  SubCalcThreadParam* subCalcThreadParam_p = (SubCalcThreadParam*)param_p;
  GenPeriodRepo* genPeriodRepo_p = (GenPeriodRepo*)(subCalcThreadParam_p->class_p);
  int date_idx = subCalcThreadParam_p->genPeriod_idx;  

  vector<GenPeriodData>& vGenPeriodData = genPeriodRepo_p->vGenPeriodData;
  vector<GenDataCalcuter>& vCalcuter = genPeriodRepo_p->vGenCalcuter;

  //cout << "gen:cal:idx " << vGenPeriodData.size() << " " <<  vCalcuter.size() << " " << date_idx << endl;
  
  if (genPeriodRepo_p->model_type == FULL)
  {
      GenDataCalcuter genCalcuter;
      genCalcuter.setDate(vGenPeriodData[date_idx].date);
      genCalcuter.setThreadPrintLogger(&(genPeriodRepo_p->printTaskMutex), &(genPeriodRepo_p->printTaskCond), &(genPeriodRepo_p->threadId2LongMap));
      genCalcuter.initWeightFactor(vGenPeriodData[date_idx]);
      genCalcuter.calcTopNMatrix(vGenPeriodData[date_idx]);
      genCalcuter.calcMatrixScore(vGenPeriodData[date_idx]);
      genCalcuter.sortFundTopNItem();
      genCalcuter.normFundTopNScore();

      pthread_mutex_lock(&genPeriodRepo_p->pushResultMutex);
      vCalcuter.push_back(genCalcuter);
      pthread_mutex_unlock(&genPeriodRepo_p->pushResultMutex);
  }
  else if (genPeriodRepo_p->model_type == SEASON)
  {
      GenDataCalcuterModel genCalcuterModel;
      genCalcuterModel.setDate(vGenPeriodData[date_idx].date);
      genCalcuterModel.setThreadPrintLogger(&(genPeriodRepo_p->printTaskMutex), &(genPeriodRepo_p->printTaskCond), &(genPeriodRepo_p->threadId2LongMap));
      genCalcuterModel.initWeightFactor(vGenPeriodData[date_idx]);
      genCalcuterModel.calcTopNMatrix(vGenPeriodData[date_idx]);
      genCalcuterModel.calcMatrixScore(vGenPeriodData[date_idx]);
      genCalcuterModel.sortFundTopNItem();
      genCalcuterModel.normFundTopNScore();

      pthread_mutex_lock(&genPeriodRepo_p->pushResultMutex);
      vCalcuter.push_back(genCalcuterModel.getBaseCalcuter());
      pthread_mutex_unlock(&genPeriodRepo_p->pushResultMutex);
  }
  else if (genPeriodRepo_p->model_type == FULL_SEASON)
  {
      GenDataCalcuterFullModel genCalcuterFullModel;
      genCalcuterFullModel.setDate(vGenPeriodData[date_idx].date);
      genCalcuterFullModel.setThreadPrintLogger(&(genPeriodRepo_p->printTaskMutex), &(genPeriodRepo_p->printTaskCond), &(genPeriodRepo_p->threadId2LongMap));
      genCalcuterFullModel.initWeightFactor(vGenPeriodData[date_idx]);
      genCalcuterFullModel.calcTopNMatrix(vGenPeriodData[date_idx]);
      genCalcuterFullModel.calcMatrixScore(vGenPeriodData[date_idx]);
      genCalcuterFullModel.sortFundTopNItem();
      genCalcuterFullModel.normFundTopNScore();

      pthread_mutex_lock(&genPeriodRepo_p->pushResultMutex);
      vCalcuter.push_back(genCalcuterFullModel.getBaseCalcuter());
      pthread_mutex_unlock(&genPeriodRepo_p->pushResultMutex);
  }

  delete subCalcThreadParam_p;

  pthread_exit(0);
}

bool GenPeriodRepo::calcOneDate(string date)
{
  int idx;
  for (idx = 0; idx < vGenPeriodData.size(); idx++)
  {
    if (vGenPeriodData[idx].date.compare(date) == 0)
      break;
  }

  if (idx == vGenPeriodData.size())
  {
    cout << "Error date: " << date << endl;
    return false;
  }

  pthread_mutex_init(&pushResultMutex, NULL);
  SubCalcThreadParam* subCalcThreadParam_p = new SubCalcThreadParam;
  subCalcThreadParam_p->class_p = (void*)this;
  subCalcThreadParam_p->genPeriod_idx = idx;

  //cout << "thread in: idx " << idx << " date " << date << endl;
  //printf("gen_p: %p, cal_p: %p date_idx: %d\n", param_p->vGenPeriodData_p, param_p->vCalcuter_p, param_p->date_idx);

  pthread_t tid;
  int err = pthread_create(&tid, NULL, GenPeriodRepo::calcOneDataTreadFunc, subCalcThreadParam_p);
  if (err == 0)
  {
    vThreadIds.push_back(tid);
  }
  else
  {
    cerr << "create calc thread failed. idx: " << idx << endl;
  }
}

void GenPeriodRepo::calcAllDate()
{
  bPringThreadExit = false;

  threadId2LongMap.clear();
  pthread_mutex_init(&printTaskMutex, NULL);
  pthread_cond_init(&printTaskCond, NULL);  

  pthread_mutex_init(&printThreadStartedMutex, NULL);
  pthread_mutex_lock(&printThreadStartedMutex);
  int err = pthread_create(&printThreadId, NULL, GenPeriodRepo::printThreadProgressLogFunc, this);
  if (err == 0)
  {
    cout << "print log thread started..." << endl;
  }
  else
  {
    cerr << "create log thread failed. idx: " << printThreadId << endl;
  }
  pthread_mutex_lock(&printThreadStartedMutex);

  // start log thread successfully.
  // now start the calc thread.

  cout << "now create sub calc thread..." << endl;

  vThreadIds.clear();

  vector<string> vDate = getDateList();
  for (int idx = 0; idx < vDate.size(); idx++)
  {
    calcOneDate(vDate[idx]);
  }

  void* tret;
  for (int i = 0; i < vThreadIds.size(); i++)
  {
    pthread_join(vThreadIds[i], &tret);
  }

  bPringThreadExit = true;
  pthread_join(printThreadId, &tret);

  sort(vGenCalcuter.begin(), vGenCalcuter.end());
}

void GenPeriodRepo::dumpOneGenData2File(string date, string format)
{
  int idx;
  for (idx = 0; idx < vGenPeriodData.size(); idx++)
  {
    if (vGenPeriodData[idx].date.compare(date) == 0)
    break;
  }
    
  if (idx == vGenPeriodData.size())
  {
    cout << "Error date: " << date << endl;
    return;
  }   

  string dumpGenDataPath = dumpGenPeriodRootdir+string("//")+repoString[repo_type]+string("//")+date;
  DIR* dumpDir_p = opendir(dumpGenDataPath.c_str());
  if (dumpDir_p == NULL)
  {
      mkdir(dumpGenDataPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  else
  {
      closedir(dumpDir_p);
  }
  cout << "dump one gendata " << dumpGenDataPath << " format " << format << endl;
  vGenPeriodData[idx].dumpGenData2File(dumpGenDataPath, format);
}

void GenPeriodRepo::dumpAllGenData2File(string format)
{
  cout << endl;
  vector<string> vDate = getDateList();
  for (int idx = 0; idx < vDate.size(); idx++)
  {
    dumpOneGenData2File(vDate[idx], format);
  }
  cout << endl;
}

void GenPeriodRepo::dumpOneCalcData2File(string date, string format)
{
  int idx;
  for (idx = 0; idx < vGenCalcuter.size(); idx++)
  {
    if (vGenCalcuter[idx].date.compare(date) == 0)
    break;
  }
      
  if (idx == vGenCalcuter.size())
  {
    cout << "Error date: " << date << endl;
    return;
  }   
  
  string dumpGenDataPath = dumpGenPeriodRootdir+string("//")+repoString[repo_type]+string("//")+date;
  DIR* dumpDir_p = opendir(dumpGenDataPath.c_str());
  if (dumpDir_p == NULL)
  {
      mkdir(dumpGenDataPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  else
  {
      closedir(dumpDir_p);
  }
  cout << "dump one calcuter " << dumpGenDataPath << " format " << format << endl;
  vGenCalcuter[idx].dumpMatrix2File(dumpGenDataPath, format);
}

void GenPeriodRepo::dumpAllCalcData2File(string format)
{
  cout << endl;
  vector<string> vDate = getDateList();
  for (int idx = 0; idx < vDate.size(); idx++)
  {
    dumpOneCalcData2File(vDate[idx], format);
  }
  cout << endl;  
}

void GenPeriodRepo::displayOneCalcResultTopN(string date, int topN)
{
  int idx;
  for (idx = 0; idx < vGenCalcuter.size(); idx++)
  {
    if (vGenCalcuter[idx].date.compare(date) == 0)
    break;
  }
        
  if (idx == vGenCalcuter.size())
  {
    cout << "Error date: " << date << endl;
    return;
  }   
  vGenCalcuter[idx].displayResultTopN(topN);
}

void GenPeriodRepo::displayAllCaclResultTopN(int topN)
{
    vector<string> vDate = getDateList();
    for (int idx = 0; idx < vDate.size(); idx++)
    {
      displayOneCalcResultTopN(vDate[idx], topN);
    }
}

void GenPeriodRepo::dumpOneCalcResultTopN(string date)
{
    int idx;
    for (idx = 0; idx < vGenCalcuter.size(); idx++)
    {
      if (vGenCalcuter[idx].date.compare(date) == 0)
      break;
    }
          
    if (idx == vGenCalcuter.size())
    {
      cout << "Error date: " << date << endl;
      return;
    }

    string oneCalcFileName=dumpGenTopNRootdir+string("//")+repoString[repo_type]+string("-")+date+"-top.csv";
    vGenCalcuter[idx].dumpResultTopN2File(oneCalcFileName, "csv");
}

void GenPeriodRepo::dumpAllCalcResultTopN()
{
    vector<string> vDate = getDateList();
    for (int idx = 0; idx < vDate.size(); idx++)
    {
      dumpOneCalcResultTopN(vDate[idx]);
    }
}

int GenPeriodRepo::getFundPosInMergedFundTopN(string fundId)
{
    for (int idx = 0; idx < vMergeFundTopN.size(); idx++)
    {
        if (vMergeFundTopN[idx].id.compare(fundId) == 0)
        {
            return idx;
        }
    }
    return -1;
}

void GenPeriodRepo::mergeAllCalcuter()
{
    if (vGenCalcuter.size() == 0) return;

    cout << "Merge repository of " << repoString[repo_type] << endl;

    int idx;
    for (idx = 0; idx < vGenCalcuter.size(); idx++)
    {
        cout << "  date: " << vGenCalcuter[idx].date << "  size: " << vGenCalcuter[idx].vFundTopNItemGen.size() << endl;
    }

    // merge
    vector<FundTopNItemGen>& vFundTopNItemGen0 = vGenCalcuter[0].vFundTopNItemGen;
    for (idx = 0; idx < vFundTopNItemGen0.size(); idx++)
    {
        MergedFundItemGen mergedFundItemGen;
        mergedFundItemGen.id   = vFundTopNItemGen0[idx].id;
        mergedFundItemGen.name = vFundTopNItemGen0[idx].name;
        mergedFundItemGen.vRankOfGens.push_back(idx);
        mergedFundItemGen.vScoreOfGens.push_back(vFundTopNItemGen0[idx].score);
        mergedFundItemGen.scoreOfMerged += vFundTopNItemGen0[idx].score;
        mergedFundItemGen.vTickOfGens.push_back(vFundTopNItemGen0[idx].ticks);
        mergedFundItemGen.tickOfMerged  += vFundTopNItemGen0[idx].ticks;

        vMergeFundTopN.push_back(mergedFundItemGen);
    }

    for (idx = 1; idx < vGenCalcuter.size(); idx++)
    {
        vector<FundTopNItemGen>& vFundTopNItemGen = vGenCalcuter[idx].vFundTopNItemGen;
        for (int jdx = 0; jdx < vFundTopNItemGen.size(); jdx++)
        {
            FundTopNItemGen& fundTopNItemGen = vFundTopNItemGen[jdx];
            
            int posInMerged = getFundPosInMergedFundTopN(fundTopNItemGen.id);
            if (posInMerged == -1)
            {
                //vMergeFundTopN.push_back(fundTopNItemGen);
                MergedFundItemGen mergedFundItemGen;
                mergedFundItemGen.id   = fundTopNItemGen.id;
                mergedFundItemGen.name = fundTopNItemGen.name;
                mergedFundItemGen.vRankOfGens.push_back(jdx);
                mergedFundItemGen.vScoreOfGens.push_back(fundTopNItemGen.score);
                mergedFundItemGen.scoreOfMerged += fundTopNItemGen.score;
                mergedFundItemGen.vTickOfGens.push_back(fundTopNItemGen.ticks);
                mergedFundItemGen.tickOfMerged  += fundTopNItemGen.ticks;

                vMergeFundTopN.push_back(mergedFundItemGen);
            }
            else
            {
                //vMergeFundTopN[posInMerged].score += fundTopNItemGen.score;
                //vMergeFundTopN[posInMerged].ticks += fundTopNItemGen.ticks;

                vMergeFundTopN[posInMerged].vRankOfGens.push_back(jdx);
                vMergeFundTopN[posInMerged].vScoreOfGens.push_back(fundTopNItemGen.score);
                vMergeFundTopN[posInMerged].scoreOfMerged += fundTopNItemGen.score;
                vMergeFundTopN[posInMerged].vTickOfGens.push_back(fundTopNItemGen.ticks);
                vMergeFundTopN[posInMerged].tickOfMerged  += fundTopNItemGen.ticks;
            }
        }
    }

    // sort
    sort(vMergeFundTopN.begin(), vMergeFundTopN.end());

    // norm
    double maxScore = vMergeFundTopN[0].scoreOfMerged;
    if (maxScore < 0.00001) maxScore = 0.0001;
    for (idx = 0; idx < vMergeFundTopN.size(); idx++)
    {   
        //vMergeFundTopN[idx].weekPro   = "";
        //vMergeFundTopN[idx].monthPro  = "";
        //vMergeFundTopN[idx].seasonPro = "";
        //vMergeFundTopN[idx].yearPro   = "";
        //vMergeFundTopN[idx].yearPro_3 = "";
        //vMergeFundTopN[idx].allPro    = "";
        vMergeFundTopN[idx].rankOfMerged   = idx;   
        vMergeFundTopN[idx].scoreOfMerged  = vMergeFundTopN[idx].scoreOfMerged / maxScore * 100;
        vMergeFundTopN[idx].tickOfMerged  /= vGenCalcuter.size();

        double rankVariance = 0;
        double scoreVariance = 0;
        int jdx;
        for (jdx = 0; jdx < vMergeFundTopN[idx].vRankOfGens.size(); jdx++)
        {
            double curRank = (double)(vMergeFundTopN[idx].vRankOfGens[jdx] + 1) / (double)(vMergeFundTopN.size()) * 100.00;
            double megRank = (double)(vMergeFundTopN[idx].rankOfMerged + 1) / (double)(vMergeFundTopN.size()) * 100.00;
            double deltaRank = curRank - megRank;
            //int deltaRank = vMergeFundTopN[idx].vRankOfGens[jdx] - vMergeFundTopN[idx].rankOfMerged;
            rankVariance += (deltaRank * deltaRank);
        }
        for (jdx = 0; jdx < vMergeFundTopN[idx].vScoreOfGens.size(); jdx++)
        {        
            double deltaScore = vMergeFundTopN[idx].vScoreOfGens[jdx] - vMergeFundTopN[idx].scoreOfMerged;
            scoreVariance += (deltaScore * deltaScore);
        }
        vMergeFundTopN[idx].rankVariance  = rankVariance / vMergeFundTopN[idx].vRankOfGens.size();
        vMergeFundTopN[idx].scoreVariance = scoreVariance / vMergeFundTopN[idx].vScoreOfGens.size();
    }
}

void GenPeriodRepo::calcVarianceAfterOfflineLoad()
{
    double maxScore = vMergeFundTopN[0].scoreOfMerged;
    if (maxScore < 0.00001) maxScore = 0.0001;

    for (int idx = 0; idx < vMergeFundTopN.size(); idx++)
    {
        vMergeFundTopN[idx].rankOfMerged   = idx;   
        vMergeFundTopN[idx].scoreOfMerged  = vMergeFundTopN[idx].scoreOfMerged / maxScore * 100;
   
        double rankVariance = 0;
        double scoreVariance = 0;
        int jdx;
        for (int jdx = 0; jdx < vMergeFundTopN[idx].vRankOfGens.size(); jdx++)
        {
            double curRank = (double)(vMergeFundTopN[idx].vRankOfGens[jdx] + 1) / (double)(vMergeFundTopN.size()) * 100.00;
            double megRank = (double)(vMergeFundTopN[idx].rankOfMerged + 1) / (double)(vMergeFundTopN.size()) * 100.00;
            double deltaRank = curRank - megRank;
            rankVariance += (deltaRank * deltaRank);
        }
        for (jdx = 0; jdx < vMergeFundTopN[idx].vScoreOfGens.size(); jdx++)
        {        
            double deltaScore = vMergeFundTopN[idx].vScoreOfGens[jdx] - vMergeFundTopN[idx].scoreOfMerged;
            scoreVariance += (deltaScore * deltaScore);
        }
        vMergeFundTopN[idx].rankVariance  = rankVariance / vMergeFundTopN[idx].vRankOfGens.size();
        vMergeFundTopN[idx].scoreVariance = scoreVariance / vMergeFundTopN[idx].vScoreOfGens.size();
    }    
}

int GenPeriodRepo::getRankGenDispLength()
{
    int maxLength = 0;
    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
      string rankGens;
      int j;
      for (j = 0; j < vMergeFundTopN[i].vRankOfGens.size(); j++)
      {
        char rank[100] = {0};
        sprintf(rank, "%d", vMergeFundTopN[i].vRankOfGens[j]);
        rankGens += string(rank);
        if (j < vMergeFundTopN[i].vRankOfGens.size() - 1)
        {
            rankGens += string("|");
        }
      }
      if (rankGens.size() > maxLength)
      {
        maxLength = rankGens.size();
      }
    }
    return (maxLength+3);
}

int GenPeriodRepo::getScoreGenDispLength()
{
    int maxLength = 0;
    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
      string scoreGens;
      int j;
      for (j = 0; j < vMergeFundTopN[i].vScoreOfGens.size(); j++)
      {
        char score[100] = {0};
        sprintf(score, "%.1f", vMergeFundTopN[i].vScoreOfGens[j]);
        scoreGens += string(score);
        if (j < vMergeFundTopN[i].vScoreOfGens.size() - 1)
        {
            scoreGens += string("|");
        }
      }
      if (scoreGens.size() > maxLength)
      {
        maxLength = scoreGens.size();
      }
    }
    return (maxLength+3);
}

void GenPeriodRepo::displayMergedFundTopN(int topN)
{
    cout << endl << "dump merged Gen Funds top(" << topN << "):" << endl;
    
    //char buffer[2048] = {0};
    char* buffer_p = new char[getRankGenDispLength() + getScoreGenDispLength()+ 1024];
    memset(buffer_p, 0, getRankGenDispLength() + getScoreGenDispLength()+ 1024);

    sprintf(buffer_p, "%8s %36s %4s %10s %6s %10s %*.*s %*.*s\n", 
                       "id", "name", "rank", "rVar", "score", "sVar", 
                       getRankGenDispLength(), getRankGenDispLength(), "rankGens", 
                       getScoreGenDispLength(), getScoreGenDispLength(), "scoreGens");
    
    cout << buffer_p << endl;
    //delete [] buffer_p;

    for (int i = 0; i < vMergeFundTopN.size() && i < topN; i++)
    {
      string rankGens;
      string scoreGens;
      int j;
      for (j = 0; j < vMergeFundTopN[i].vRankOfGens.size(); j++)
      {
        char rank[100] = {0};
        sprintf(rank, "%d", vMergeFundTopN[i].vRankOfGens[j]);
        rankGens += string(rank);
        if (j < vMergeFundTopN[i].vRankOfGens.size() - 1)
        {
            rankGens += string("|");
        }
      }
      for (j = 0; j < vMergeFundTopN[i].vScoreOfGens.size(); j++)
      {
        char score[100] = {0};
        sprintf(score, "%.1f", vMergeFundTopN[i].vScoreOfGens[j]);
        scoreGens += string(score);
        if (j < vMergeFundTopN[i].vScoreOfGens.size() - 1)
        {
            scoreGens += string("|");
        }
      }

      //char* buffer_p = new char[getRankGenDispLength() + getScoreGenDispLength()+ 1024];
      memset(buffer_p, 0, getRankGenDispLength() + getScoreGenDispLength()+ 1024);

      sprintf(buffer_p, "%8s %36s %4d %10.1f %6.1f %10.1f %*.*s %*.*s\n", 
                    vMergeFundTopN[i].id.c_str(), vMergeFundTopN[i].name.c_str(), 
                    vMergeFundTopN[i].rankOfMerged, vMergeFundTopN[i].rankVariance, 
                    vMergeFundTopN[i].scoreOfMerged, vMergeFundTopN[i].scoreVariance, 
                    getRankGenDispLength(), getRankGenDispLength(),rankGens.c_str(), 
                    getScoreGenDispLength(), getScoreGenDispLength(),scoreGens.c_str());

      cout << buffer_p << endl;
    }
    delete [] buffer_p;
    cout << endl;
}

void GenPeriodRepo::dumpMergedFundTopN2File()
{
    string mergedFileName = dumpGenMergeRootdir+string("//")+repoString[repo_type]+string("-merged-top-by-rank.csv");
    FileDumper::dumpVMergedFundTopNItem2File(vMergeFundTopN, getRankGenDispLength(), getScoreGenDispLength(), mergedFileName, "csv");
}

void GenPeriodRepo::calcUnionVariance()
{
    vRankVarItems.clear();
    vScoreVarItems.clear();

    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
        VarRankFundItemGen  varRankItem(vMergeFundTopN[i]);
        VarScoreFundItemGen varScoreItem(vMergeFundTopN[i]);
        //VarFundItemGen varFundItem(vMergeFundTopN[i]);

        vRankVarItems.push_back(varRankItem);
        vScoreVarItems.push_back(varScoreItem);
        //vVarFundItems.push_back(varFundItem);
    }

    sort(vRankVarItems.begin(), vRankVarItems.end());

    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
        for (int j = 0; j < vRankVarItems.size(); j++)
        {
            if (vMergeFundTopN[i].id.compare(vRankVarItems[j].id) == 0)
            {
                vMergeFundTopN[i].rankOfRankVar = j;
                break;
            }
        }
    }

    sort(vScoreVarItems.begin(), vScoreVarItems.end());

    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
        for (int j = 0; j < vScoreVarItems.size(); j++)
        {
            if (vMergeFundTopN[i].id.compare(vScoreVarItems[j].id) == 0)
            {
                vMergeFundTopN[i].rankOfScoreVar = j;
                break;
            }
        }
    }

    /*
    input:  vMergeFundTopN, vRankVarItems, vScoreVarItems
    output: vUnionVarItem
    */
#if 1
    vVarFundItems.clear();

    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
        VarFundItemGen varFundItem(vMergeFundTopN[i]);
        vVarFundItems.push_back(varFundItem);
    }

    int idxRankVar, idxScoreVar;

    for (int i = 0; i < vVarFundItems.size(); i++)
    {
        idxRankVar = idxScoreVar = -1;

        int idx;
        for (idx = 0; idx < vRankVarItems.size(); idx++)
        {
            if (vRankVarItems[idx].id.compare(vVarFundItems[i].id) == 0) break;
        }

        idxRankVar = idx;

        for (idx = 0; idx < vScoreVarItems.size(); idx++)
        {
            if (vScoreVarItems[idx].id.compare(vVarFundItems[i].id) == 0) break;
        }

        idxScoreVar = idx;

        //int varWorseRank = idxRankVar > idxScoreVar ? idxRankVar : idxScoreVar;

        //int varEqualRank = (idxRankVar + idxScoreVar) / 2;
        int varEqualRank = idxRankVar;
        int rankRank = vVarFundItems[i].rankOfMerged;

        long f1 = (vVarFundItems.size()-rankRank)*(vVarFundItems.size()-rankRank);
        long f2 = (vVarFundItems.size()-varEqualRank)*(vVarFundItems.size()-varEqualRank);       

        vVarFundItems[i].unionVarRankAndRank = f1*60/100+f2*40/100;

#if 1
        printf("id: %3d rankVar: %3d scoreVar: %3d worseVar: %3d rank: %3d unionRank: %3d\n",
                i, idxRankVar, idxScoreVar, varEqualRank, rankRank, vVarFundItems[i].unionVarRankAndRank);
#endif
    }

    sort(vVarFundItems.begin(), vVarFundItems.end());

    for (int i = 0; i < vMergeFundTopN.size(); i++)
    {
        for (int j = 0; j < vVarFundItems.size(); j++)
        {
            if (vMergeFundTopN[i].id.compare(vVarFundItems[j].id) == 0)
            {
                vMergeFundTopN[i].rankOfUnionRankAndScore = j;
                break;
            }
        }
    }

    vUnionVarItem.clear();

    for (int i = 0; i < vVarFundItems.size(); i++)
    {
        vUnionVarItem.push_back(vVarFundItems[i].getBaseItem());
    }

    for (int i = 0; i < vUnionVarItem.size(); i++)
    {
        for (int j = 0; j < vMergeFundTopN.size(); j++)
        {
            if (vUnionVarItem[i].id.compare(vMergeFundTopN[j].id) == 0)
            {
                vUnionVarItem[i].rankOfRankVar      = vMergeFundTopN[j].rankOfRankVar;
                vUnionVarItem[i].rankOfScoreVar     = vMergeFundTopN[j].rankOfScoreVar;
                vUnionVarItem[i].rankOfUnionRankAndScore = vMergeFundTopN[j].rankOfUnionRankAndScore;
                break;
            }
        }
    }
#endif
}

void GenPeriodRepo::dumpMergedFundTopN2FileByVariance()
{
    string mergedFileName = dumpGenMergeRootdir+string("//")+repoString[repo_type]+string("-merged-top-by-union-var.csv");
    FileDumper::dumpVMergedFundTopNItem2File(vUnionVarItem, getRankGenDispLength(), getScoreGenDispLength(), mergedFileName, "csv");
}

bool GenPeriodRepo::isFundExistInRepoMergedGen(string fundId)
{
    for (int idx = 0; idx < vMergeFundTopN.size(); idx++)
    {
        if (vMergeFundTopN[idx].id.compare(fundId) == 0)
        {
            return true;
        }
    }
    return false;
}

void GenPeriodRepo::debugDataStored()
{
  cout << "*** " << repoString[repo_type] << " ***" << endl;
  cout << "Date count (" << vGenPeriodData.size() << "): " << endl;
  vector<string> vDate = getDateList();
  for (int idx = 0; idx < vDate.size(); idx++) cout << vDate[idx] << " ";
  cout << endl;

  for (int i = 0; i < vGenPeriodData.size(); i++)
  {
    cout << "idx " << i << endl;
    for (int j = 0; j < vGenPeriodData[i].vFileName.size(); j++)
    {
      cout << "  " << vGenPeriodData[i].vFileName[j] << endl;
    }
    cout << "  size (W,M,S,Y,3Y,A,min)" << vGenPeriodData[i].vWeek.size() << " " << vGenPeriodData[i].vMonth.size() << " " << vGenPeriodData[i].vSeason.size() << " " << vGenPeriodData[i].vYear.size() << " " << vGenPeriodData[i].v3Year.size() << " " << vGenPeriodData[i].vAll.size() << " " << vGenPeriodData[i].iMinFundNum << endl;
  }
  cout << endl;   
}
