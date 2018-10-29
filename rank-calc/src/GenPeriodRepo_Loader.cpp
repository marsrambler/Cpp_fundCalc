#include <fstream>
#include <algorithm>
#include <functional>
#include <list>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "GenPeriodRepo_Loader.h"
#include "FileDumper.h"
using namespace std;

//#define _DEBUG 0

void GenPeriodRepo_Loader::buildLoaderCfg(string loaderCfgFileName)
{
    vLoaderCfg.clear();

    ifstream ifile(loaderCfgFileName.c_str());
    string oneLine = ""; 
    bool bGenPerSec = false;
    LoaderCfg loaderCfg;    

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
    
      if (oneLine.find("[repo]") != string::npos)
      {   
        bGenPerSec = true;
      }   
      else if (oneLine.find(string("[")) != string::npos)
      {   
        bGenPerSec = false;
      }
      else if (bGenPerSec)
      {
        if (oneLine.find(string("type=")) != string::npos)
        {
            size_t fpos = oneLine.find(string("type="))+5;
            string repoType = oneLine.substr(fpos);
            int s = repoType.find_first_not_of(" \t");
            int e = repoType.find_last_not_of(" \t");
            if (s != string::npos && e != string:: npos /*&& s != e*/)
            {
              repoType = repoType.substr(s, e-s+1);
              loaderCfg.repoType = repoType; 
            }
        }
        else if (oneLine.find(string("file=")) != string::npos)
        {
          size_t fpos = oneLine.find(string("file="))+5;
          string filePath = oneLine.substr(fpos);
          int s = filePath.find_first_not_of(" \t");
          int e = filePath.find_last_not_of(" \t");
          if (s != string::npos && e != string:: npos /*&& s != e*/)
          {
            filePath = filePath.substr(s, e-s+1);
            loaderCfg.repoFile = filePath; 
          }
        }
        else if (oneLine.find(string("date=")) != string::npos)
        {
#if 0
          vector<string> vDate;

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
          loaderCfg.vRepoDate = vDate;
#endif
          vLoaderCfg.push_back(loaderCfg);
        }
      }  // if end   
    } // while end
}

void GenPeriodRepo_Loader::loadMergedFundItemGen()
{
    if (vLoaderCfg.size() == 0) 
    {
        cerr << "need to build loader cfg firstly" << endl;
        return;
    }
#ifdef _DEBUG 
    else 
    {
        for (int di = 0; di < vLoaderCfg.size(); di++)
        {
            LoaderCfg loadCfg = vLoaderCfg[di];
            cout << "i:        " << di << endl;
            cout << "type:     " << loadCfg.repoType << endl;
            cout << "file:     " << loadCfg.repoFile << endl;
            cout << "dateSize: " << loadCfg.vRepoDate.size() << endl;
        }
    }
#endif

    v_genPeriodRepo.clear();

    for (int i = 0; i < vLoaderCfg.size(); i++)
    {
        GenPeriodRepo* repo_p = NULL;
        LoaderCfg loadCfg = vLoaderCfg[i];

        if (loadCfg.repoType.find("stock") != string::npos)
        {
            repo_p = new GenPeriodRepo(GenPeriodRepo::STOCK);
        } 
        else if (loadCfg.repoType.find("debt") != string::npos)
        {
            repo_p = new GenPeriodRepo(GenPeriodRepo::DEBT);
        }
        else if (loadCfg.repoType.find("comp") != string::npos)
        {
            repo_p = new GenPeriodRepo(GenPeriodRepo::COMP);
        }
        else if (loadCfg.repoType.find("index") != string::npos)
        {
            repo_p = new GenPeriodRepo(GenPeriodRepo::INDEX);
        }
        else
        {
            cerr << "error repo type: " << loadCfg.repoType << endl;
            exit(0);
        }

        if (repo_p != NULL)
        {
            string fileName = loadCfg.repoFile;
            ifstream ifile(fileName.c_str());
            string oneLine = ""; 

            getline(ifile, oneLine);

            int times = 0;
            size_t pos = oneLine.find(",");
            while (pos != string::npos)
            {
                pos = oneLine.find(",", pos+1);
                times++;
            }            

            if (times != 6)
            {
                cerr << "the repo file may not match the algorithm of process." << endl;
                cerr << "file: " << fileName << "   " << "times: " << times  << "   " << "i: "  << i << endl;
                exit(0);
            }

            while(getline(ifile, oneLine))
            {
                MergedFundItemGen mergedFundItemGen;

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
                oneLine = trimString(oneLine);
                
                size_t fpos = 0;  
                int column_idx = 0;
                string columnList = oneLine.substr(fpos);
                fpos = columnList.find(string(","));
                while (fpos != string::npos)
                {
                  string column = columnList.substr(0, fpos);
                  int s = column.find_first_not_of(" \t");
                  int e = column.find_last_not_of(" \t");
                  if (s != string::npos && e != string::npos /*&& s != e*/)
                  {
                    column = column.substr(s, e-s+1);
                    buildColumn(column, column_idx, &mergedFundItemGen);
                    column_idx++;
                  }
                  columnList = columnList.substr(fpos+1);
                  fpos = columnList.find(string(","));
                }

                s = columnList.find_first_not_of(" \t");
                e = columnList.find_last_not_of(" \t");
                if (s != string::npos && e != string:: npos /*&& e != s*/)
                {
                  columnList = columnList.substr(s, e-s+1);
                  buildColumn(columnList, column_idx, &mergedFundItemGen);
                }

                repo_p->vMergeFundTopN.push_back(mergedFundItemGen);
            }

            v_genPeriodRepo.push_back(repo_p);
        }
    }
}

void GenPeriodRepo_Loader::calcVarianceAfterOfflineLoad()
{
    if (v_genPeriodRepo.size() == 0)
    {
        cerr << "the repo size should > 0, exit." << endl;
        exit(0);
    }
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        repo_p->calcVarianceAfterOfflineLoad();
    }
}

string GenPeriodRepo_Loader::trimString(string szString)
{
    int s = szString.find_first_not_of(" \t");
    int e = szString.find_last_not_of(" \t\r");
    if (s != string::npos && e != string:: npos /*&& s != e*/)
    {
        string sz1 = szString.substr(s, e-s+1);
        return sz1;
    }
    return string("");
}

void GenPeriodRepo_Loader::buildColumn(string columnText, int column_idx, MergedFundItemGen* mergedFundItemGen_p)
{
    switch (column_idx)
    {
        case 0:
                {
                    mergedFundItemGen_p->id = columnText;
                    break;
                }
        case 1:
                {
                    mergedFundItemGen_p->name = columnText;
                    break;
                }
        case 2:
                {
                    int i;
                    sscanf(columnText.c_str(), "%d", &i);
                    mergedFundItemGen_p->rankOfMerged = i;
                    break;
                }
        case 3:
                {
                    int i;
                    sscanf(columnText.c_str(), "%d", &i);
                    mergedFundItemGen_p->rankOfRankVar = i;
                    break;
                }
        case 4:
                {
                    int i;
                    sscanf(columnText.c_str(), "%d", &i);
                    mergedFundItemGen_p->rankOfUnionRankAndScore = i;
                    break;
                }
        case 5:
                {
                    vector<int> rankOfRankVar;

                    size_t fpos = 0;  
                    string columnList = columnText.substr(fpos);
                    fpos = columnList.find(string("|"));
                    while (fpos != string::npos)
                    {
                        string column = columnList.substr(0, fpos);
                        int s = column.find_first_not_of(" \t");
                        int e = column.find_last_not_of(" \t");
                        if (s != string::npos && e != string::npos /*&& s != e*/)
                        {
                          column = column.substr(s, e-s+1);
                          int i;
                          sscanf(column.c_str(), "%d", &i);
                          rankOfRankVar.push_back(i);
                        }
                        columnList = columnList.substr(fpos+1);
                        fpos = columnList.find(string("|"));
                    }

                    int s = columnList.find_first_not_of(" \t");
                    int e = columnList.find_last_not_of(" \t");
                    if (s != string::npos && e != string:: npos /*&& e != s*/)
                    {
                        columnList = columnList.substr(s, e-s+1);
                        int i;
                        sscanf(columnList.c_str(), "%d", &i);
                        rankOfRankVar.push_back(i);
                    }
                    mergedFundItemGen_p->vRankOfGens = rankOfRankVar;
                    break;
                }
        case 6:
                {
                    vector<double> vScoreOfGens;
                    double totalScore = 0;

                    size_t fpos = 0;  
                    string columnList = columnText.substr(fpos);
                    fpos = columnList.find(string("|"));
                    while (fpos != string::npos)
                    {
                        string column = columnList.substr(0, fpos);
                        int s = column.find_first_not_of(" \t");
                        int e = column.find_last_not_of(" \t");
                        if (s != string::npos && e != string::npos /*&& s != e*/)
                        {
                          column = column.substr(s, e-s+1);
                          float i;
                          sscanf(column.c_str(), "%f", &i);
                          totalScore += i;
                          vScoreOfGens.push_back(i);
                        }
                        columnList = columnList.substr(fpos+1);
                        fpos = columnList.find(string("|"));
                    }

                    int s = columnList.find_first_not_of(" \t");
                    int e = columnList.find_last_not_of(" \t");
                    if (s != string::npos && e != string:: npos /*&& e != s*/)
                    {
                        columnList = columnList.substr(s, e-s+1);
                        float i;
                        sscanf(columnList.c_str(), "%f", &i);
                        totalScore += i;
                        vScoreOfGens.push_back(i);
                    }
                    mergedFundItemGen_p->vScoreOfGens  = vScoreOfGens;
                    mergedFundItemGen_p->scoreOfMerged = totalScore;
                    break;
                }
        default:
                {
                    break;
                } 
    }
}

void GenPeriodRepo_Loader::buildDumpDirectory(string cfgFilename)
{
    if (v_genPeriodRepo.size() == 0)
    {
        cerr << "the repo size should > 0, exit." << endl;
        exit(0);
    }
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        repo_p->buildDumpDirectory(cfgFilename);
    }
}

void GenPeriodRepo_Loader::calcUnionVariance()
{
    if (v_genPeriodRepo.size() == 0)
    {
        cerr << "the repo size should > 0, exit." << endl;
        exit(0);
    }
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        repo_p->calcUnionVariance();
    }
}

void GenPeriodRepo_Loader::dumpMergedFundTopN2File()
{
    if (v_genPeriodRepo.size() == 0)
    {
        cerr << "the repo size should > 0, exit." << endl;
        exit(0);
    }
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        repo_p->dumpMergedFundTopN2File();
    }
}

void GenPeriodRepo_Loader::dumpMergedFundTopN2FileByVariance()
{
    if (v_genPeriodRepo.size() == 0)
    {
        cerr << "the repo size should > 0, exit." << endl;
        exit(0);
    }
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        repo_p->dumpMergedFundTopN2FileByVariance();
    }
}

int GenPeriodRepo_Loader::getRepoIdxForFund(string fundId)
{
    for (int i = 0; i < v_genPeriodRepo.size(); i++)
    {
        GenPeriodRepo* repo_p = v_genPeriodRepo[i];
        if (repo_p->isFundExistInRepoMergedGen(fundId))
        {
            return i;
        }
    }
    return -1;    
}

MergedFundItemGen& GenPeriodRepo_Loader::getMergedFundItemGenByFundId(string fundId)
{
    int i = getRepoIdxForFund(fundId);
    if (i == -1)
    {
        cerr << "error, can not find fundId: " << fundId << endl;
        exit(0);
    }

    GenPeriodRepo* repo_p = v_genPeriodRepo[i];
    int ipos = repo_p->getFundPosInMergedFundTopN(fundId);
    MergedFundItemGen& mergedItem = repo_p->vMergeFundTopN[ipos];

    return mergedItem;
}

string GenPeriodRepo_Loader::getRepoType(int repo_idx)
{
    GenPeriodRepo* repo_p = v_genPeriodRepo[repo_idx];
    return repo_p->repoString[repo_p->repo_type];
}

int GenPeriodRepo_Loader::getRepoMergedItemSize(int repo_idx)
{
    GenPeriodRepo* repo_p = v_genPeriodRepo[repo_idx];
    return repo_p->vMergeFundTopN.size();
}

string GenPeriodRepo_Loader::getFavorDumpRootPath()
{
    GenPeriodRepo* repo_p = v_genPeriodRepo[0];
    return repo_p->dumpFavorFundRootdir;
}

string GenPeriodRepo_Loader::getTransformDumpRootPath()
{
    GenPeriodRepo* repo_p = v_genPeriodRepo[0];
    return repo_p->dumpTransformRootdir;
}