#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <functional>
#include "FavorFundsProcessor.h"
#include "FavorFileDumper.h"
#include "GenPeriodRepo.h"
using namespace std;

void FavorFundsProcessor::buildFavoriteFunds(string cfgFilename)
{
    vFavorFund.clear();
    vvFavorFund.clear();    

    ifstream ifile(cfgFilename.c_str());
    string oneLine = ""; 
    bool bPri_1Sec = false;
    bool bPri_2Sec = false;
    bool bPri_4Sec = false;
    bool bPri_5Sec = false;
    int times_1 = 0;
    int times_2 = 0;
    int times_4 = 0;
    int times_5 = 0;
    
    while(getline(ifile, oneLine))
    {
        int s = oneLine.find_first_not_of(" \t");
        int e = oneLine.find_last_not_of(" \t\r");
        if (s != string::npos && e != string:: npos /*&& s != e*/)
        {
            oneLine = oneLine.substr(s, e-s+1);
            if (oneLine.find(string("#")) != string::npos) continue;

            if (oneLine.find(string("[priority-1]")) != string::npos)
            {
                bPri_1Sec = true;
            }
            else if (oneLine.find(string("[priority-2]")) != string::npos)
            {
                bPri_1Sec = false;
                bPri_2Sec = true;
            }
            else if (oneLine.find(string("[priority-4]")) != string::npos)
            {
                bPri_1Sec = false;
                bPri_2Sec = false;                
                bPri_4Sec = true;
            }
            else if (oneLine.find(string("[priority-5]")) != string::npos)
            {
                bPri_1Sec = false;
                bPri_2Sec = false;               
                bPri_4Sec = false;
                bPri_5Sec = true;
            }
            else
            {
                FavorFundItem item;
                if (getOneFavorFundItem(oneLine, item) == true)
                {
                    if (bPri_1Sec) {item.priority = string("p1"); item.priComments = string("BOC");         times_1++; }
                    if (bPri_2Sec) {item.priority = string("p2"); item.priComments = string("ICBC/Major");  times_2++; }
                    if (bPri_4Sec) {item.priority = string("p4"); item.priComments = string("ICBC/GiveUp"); times_4++; }
                    if (bPri_5Sec) {item.priority = string("p5"); item.priComments = string("BOC/GiveUp");  times_5++; }
                    //cout << item.id  << " " << item.priority << " " << item.priComments << endl;
                    if ((times_2 == 1 && times_1 >= 0) ||
                        (times_4 == 1 && times_2 >= 0) ||
                        (times_5 == 1 && times_4 >= 0) )
                    {
                        vvFavorFund.push_back(vFavorFund);
                        vFavorFund.clear();
                    }
                    vFavorFund.push_back(item);
                }
                else
                {
                    cout << "line ERROR: " << oneLine << endl;
                }
            }
        }
        else
        {
            //cout << "bypass:" << oneLine << endl;
            continue; 
        }
    }

    if (vFavorFund.size() > 0)
    {
        vvFavorFund.push_back(vFavorFund);
    }
}

bool FavorFundsProcessor::getOneFavorFundItem(string& oneLine, FavorFundItem& item)
{
    string fundId;

    if (oneLine.find(string("id=")) == string::npos) return false;

    size_t fpos = oneLine.find(string("id="));
    fundId = oneLine.substr(fpos+string("id=").length());
    item.id = trimString(fundId);

    return true;
}

string FavorFundsProcessor::trimString(string szString)
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

void FavorFundsProcessor::displayFavorFundsStatus()
{    
    char* buffer_p = new char[getMaxRankGenDispLen() + getMaxScoreGenDispLen() + 1024];
    memset(buffer_p, 0, getMaxRankGenDispLen() + getMaxScoreGenDispLen() + 1024);

    sprintf(buffer_p, "%8s %36s %12s %6s %10s %10s %6s %10s %*.*s %*.*s\n", 
                    "id", "name","comments","type","rank","rVar", "score", "sVar", 
                    getMaxRankGenDispLen(), getMaxRankGenDispLen(), "rankGens", 
                    getMaxScoreGenDispLen(), getMaxScoreGenDispLen(), "scoreGens");
    
    cout << buffer_p << endl;
    
    for (int idxVV = 0; idxVV < vvFavorFund.size(); idxVV++)
    {
        vFavorFund = vvFavorFund[idxVV];

        for (int i = 0; i < vFavorFund.size(); i++)
        {
          string rankGens;
          string scoreGens;
          int j;
          for (j = 0; j < vFavorFund[i].vRankOfGens.size(); j++)
          {
            char rank[100] = {0};
            sprintf(rank, "%d", vFavorFund[i].vRankOfGens[j]);
            rankGens += string(rank);
            if (j < vFavorFund[i].vRankOfGens.size() - 1)
            {
                rankGens += string("|");
            }
          }
          for (j = 0; j < vFavorFund[i].vScoreOfGens.size(); j++)
          {
            char score[100] = {0};
            sprintf(score, "%.1f", vFavorFund[i].vScoreOfGens[j]);
            scoreGens += string(score);
            if (j < vFavorFund[i].vScoreOfGens.size() - 1)
            {
                scoreGens += string("|");
            }
          }

          char rankbuffer[20] = {0};
          sprintf(rankbuffer, "%d/%d", vFavorFund[i].rankOfMerged, vFavorFund[i].totalFundNum);

          //char buffer[2048] = {0};
          memset(buffer_p, 0, getMaxRankGenDispLen() + getMaxScoreGenDispLen() + 1024);

          sprintf(buffer_p, "%8s %36s %12s %6s %10s %10.1f %6.1f %10.1f %*.*s %*.*s\n",
                            vFavorFund[i].id.c_str(), vFavorFund[i].name.c_str(),
                            vFavorFund[i].priComments.c_str(), vFavorFund[i].stockTypeName.c_str(),
                            rankbuffer, vFavorFund[i].rankVariance,
                            vFavorFund[i].scoreOfMerged, vFavorFund[i].scoreVariance, 
                            getMaxRankGenDispLen(), getMaxRankGenDispLen(), rankGens.c_str(),
                            getMaxScoreGenDispLen(), getMaxScoreGenDispLen(), scoreGens.c_str());

          cout << buffer_p << endl;
        }
    }
    delete [] buffer_p;

    cout << endl;
}

int FavorFundsProcessor::getMaxRankGenDispLen()
{
    vFavorFund.clear();
    for (int idxVV = 0; idxVV < vvFavorFund.size(); idxVV++)
    {
        for (int idxV = 0; idxV < vvFavorFund[idxVV].size(); idxV++)
        {
            vFavorFund.push_back(vvFavorFund[idxVV][idxV]);
        }
    }

    int maxLength = 0;
    for (int i = 0; i < vFavorFund.size(); i++)
    {
      string rankGens;
      int j;
      for (j = 0; j < vFavorFund[i].vRankOfGens.size(); j++)
      {
        char rank[100] = {0};
        sprintf(rank, "%d", vFavorFund[i].vRankOfGens[j]);
        rankGens += string(rank);
        if (j < vFavorFund[i].vRankOfGens.size() - 1)
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

int FavorFundsProcessor::getMaxScoreGenDispLen()
{
    vFavorFund.clear();
    for (int idxVV = 0; idxVV < vvFavorFund.size(); idxVV++)
    {
        for (int idxV = 0; idxV < vvFavorFund[idxVV].size(); idxV++)
        {
            vFavorFund.push_back(vvFavorFund[idxVV][idxV]);
        }
    }

    int maxLength = 0;
    for (int i = 0; i < vFavorFund.size(); i++)
    {
      string scoreGens;
      int j;
      for (j = 0; j < vFavorFund[i].vScoreOfGens.size(); j++)
      {
        char score[100] = {0};
        sprintf(score, "%.1f", vFavorFund[i].vScoreOfGens[j]);
        scoreGens += string(score);
        if (j < vFavorFund[i].vScoreOfGens.size() - 1)
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

void FavorFundsProcessor::dumpFavorFund2File(string rootPath)
{
    for (int idxVV = 0; idxVV < vvFavorFund.size(); idxVV++)
    {
        sort(vvFavorFund[idxVV].begin(), vvFavorFund[idxVV].end());
    }

    string fullpath = /*GenPeriodRepo::dumpFavorFundRootdir*/rootPath+string("//")+string("favor-funds-status.csv");

    FavorFileDumper::dumpVFavorFunds2File(vvFavorFund, getMaxRankGenDispLen(), getMaxScoreGenDispLen(), fullpath, string("csv"));
}
