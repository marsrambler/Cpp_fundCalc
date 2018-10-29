#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <functional>
#include "TransformProcessor.h"
#include "TransformFileDumper.h"
#include "GenPeriodRepo.h"
using namespace std;

#define _DEBUG 1

void TransformProcessor::buildFavorTransformRelation(string cfgFilename)
{
    ifstream ifile(cfgFilename.c_str());
    string oneLine = "";   
    int iLoadCount = 0;
    string fundId = "";    
    string fileName = "";

    while(getline(ifile, oneLine))
    {
        int s = oneLine.find_first_not_of(" \t");
        int e = oneLine.find_last_not_of(" \t\r");
        if (s != string::npos && e != string:: npos /*&& s != e*/)
        {
            oneLine = oneLine.substr(s, e-s+1);
            if (oneLine.find(string("#")) != string::npos) continue;
            
            if (oneLine.find(string("[")) != string::npos)
            {
                iLoadCount++;
                continue;
            } 
            else
            {
#if 0
                cout << "load count: " << iLoadCount << endl;
                cout << "line: " << oneLine << endl;
#endif          
                string pureLine = trimString(oneLine);
                if (pureLine.size() == 0) continue;

                if (pureLine.find(string("id=")) != string::npos)
                {
                    size_t fpos = pureLine.find(string("id="));
                    fundId = pureLine.substr(fpos + string("id=").length());
                    fundId = trimString(fundId);
                }
                else if (pureLine.find(string("file=")) != string::npos)
                {
                    size_t fpos = pureLine.find(string("file="));
                    fileName = pureLine.substr(fpos + string("file=").length());
                    fileName = trimString(fileName);
                    vector<string> vTransFunds = getTransFunds(fileName);
                    if (vTransFunds.size() == 0)
                    {
                        cerr << "get fundId: " << fundId << " error, size is 0" << endl;
                        continue;
                    }
                    favorTransformMap.insert(make_pair(fundId, vTransFunds));
#ifdef _DEBUG
                    cout << "insert " << fundId << " size: " << vTransFunds.size() << endl;
#endif
                }
            }            
        }
        else
        {
#ifdef _DEBUG
            cerr << "bypass:" << oneLine << endl;
#endif
            continue; 
        }
    }

    if (vFavorFund.size() > 0)
    {
        vvFavorFund.push_back(vFavorFund);
    }
}

vector<string> TransformProcessor::getTransFunds(string fileName)
{
    ifstream ifile(fileName.c_str());
    string oneLine = ""; 
    vector<string> vFundIds;
    vFundIds.clear();

    while(getline(ifile, oneLine))
    {
        int s = oneLine.find_first_not_of(" \t");
        int e = oneLine.find_last_not_of(" \t\r");
        if (s != string::npos && e != string:: npos /*&& s != e*/)
        {
            oneLine = oneLine.substr(s, e-s+1);
            if (oneLine.find(string("#")) != string::npos) continue;
            
            oneLine = trimString(oneLine);
            if (oneLine.size() == 0) continue;
            
            vFundIds.push_back(oneLine);
        }
    }
    
    return vFundIds;
}

int TransformProcessor::getMaxRankGenDispLen(vector<vector<TransformFundItem> >& vvItem)
{
    vector<TransformFundItem> vTransItems;

    vTransItems.clear();
    for (int idxVV = 0; idxVV < vvItem.size(); idxVV++)
    {
        for (int idxV = 0; idxV < vvItem[idxVV].size(); idxV++)
        {
            vTransItems.push_back(vvItem[idxVV][idxV]);
        }
    }

    int maxLength = 0;
    for (int i = 0; i < vTransItems.size(); i++)
    {
      string rankGens;
      int j;
      for (j = 0; j < vTransItems[i].vRankOfGens.size(); j++)
      {
        char rank[100] = {0};
        sprintf(rank, "%d", vTransItems[i].vRankOfGens[j]);
        rankGens += string(rank);
        if (j < vTransItems[i].vRankOfGens.size() - 1)
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

int TransformProcessor::getMaxScoreGenDispLen(vector<vector<TransformFundItem> >& vvItem)
{
    vector<TransformFundItem> vTransItems;

    vTransItems.clear();
    for (int idxVV = 0; idxVV < vvItem.size(); idxVV++)
    {
        for (int idxV = 0; idxV < vvItem[idxVV].size(); idxV++)
        {
            vTransItems.push_back(vvItem[idxVV][idxV]);
        }
    }

    int maxLength = 0;
    for (int i = 0; i < vTransItems.size(); i++)
    {
      string scoreGens;
      int j;
      for (j = 0; j < vTransItems[i].vScoreOfGens.size(); j++)
      {
        char score[100] = {0};
        sprintf(score, "%.1f", vTransItems[i].vScoreOfGens[j]);
        scoreGens += string(score);
        if (j < vTransItems[i].vScoreOfGens.size() - 1)
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


void TransformProcessor::dumpTransformFund2File(string rootPath)
{
    map<MergedFundItemGen, vector<TransformFundItem> >::iterator iter = mainSubFundsMap.begin();
    
    for (; iter != mainSubFundsMap.end(); iter++)
    {
        MergedFundItemGen mainFundItem = (MergedFundItemGen)(iter->first);
        vector<TransformFundItem> subFundItems = (vector<TransformFundItem>)(iter->second);
        //sort(subFundItems.begin(), subFundItems.end());

        string fullpath = rootPath+string("//")+mainFundItem.id+string(".csv");

        vector<vector<TransformFundItem> > vvSubFundItems;
        vvSubFundItems.clear();

        GenPeriodRepo::REPO_TYPE total = GenPeriodRepo::TOTAL;

        for (int i = 0; i < total; i++)
        {
            vector<TransformFundItem> vSubFundItem;
            vSubFundItem.clear();

            string repoType = GenPeriodRepo::repoString[i];
            for (int j = 0; j < subFundItems.size(); j++)
            {
                TransformFundItem& item = subFundItems[j];
                if (repoType.compare(item.stockTypeName) == 0)
                {
                    vSubFundItem.push_back(item);
                }
            }
            sort(vSubFundItem.begin(), vSubFundItem.end());
            vvSubFundItems.push_back(vSubFundItem);
        }        

        TransformFileDumper::dumpVTransformFunds2File(vvSubFundItems, 
                                                      getMaxRankGenDispLen(vvSubFundItems), 
                                                      getMaxScoreGenDispLen(vvSubFundItems), 
                                                      fullpath, string("csv"));
    }
}

