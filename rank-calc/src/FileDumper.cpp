#include <iostream>
#include <string.h>
#include "FileDumper.h"
using namespace std;

void FileDumper::dumpVFundItemGen2File(vector<FundItemGen> vItem, const char* filename, string format)
{
    if (filename == NULL) return;
    FILE* fp = fopen(filename,"w");
    if (fp == NULL)
    {
      cerr << "open " << filename << " failed" << endl;
      return;
    }
    
    char buffer[2048] = {0};
    if (string("txt").compare(format) == 0)
    {
      sprintf(buffer, "%10s %40s %10s %10s %10s %10s %10s %10s\n", "id", "name", "week", "month", "season", "year", "3 year", "all");
    }
    else
    {
      sprintf(buffer, "%10s, %40s, %10s, %10s, %10s, %10s, %10s, %10s\n", "id", "name", "week", "month", "season", "year", "3 year", "all");
    }
    fprintf(fp, "%s", buffer);
    
    for (int i = 0; i < vItem.size(); i++)
    {
      char buffer[2048] = {0};
      if (string("txt").compare(format) == 0)
      {
        sprintf(buffer, "%10s %40s %10s %10s %10s %10s %10s %10s\n", vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].weekPro.c_str(), vItem[i].monthPro.c_str(), vItem[i].seasonPro.c_str(), vItem[i].yearPro.c_str(), vItem[i].yearPro_3.c_str(), vItem[i].allPro.c_str());
      }
      else
      {
        sprintf(buffer, "%10s, %40s, %10s, %10s, %10s, %10s, %10s, %10s\n", vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].weekPro.c_str(), vItem[i].monthPro.c_str(), vItem[i].seasonPro.c_str(), vItem[i].yearPro.c_str(), vItem[i].yearPro_3.c_str(), vItem[i].allPro.c_str());
      }
      fprintf(fp, "%s", buffer);
    }
    
    fflush(fp);
    fclose(fp);
}

void FileDumper::dumpVFundTopNItem2File(vector<FundTopNItemGen>& vFundTopNItemGen, const char* filename, string format)
{
    if (filename == NULL) return;
    FILE* fp = fopen(filename,"w");
    if (fp == NULL)
    {
      cerr << "open " << filename << " failed" << endl;
      return;
    }

    char buffer[2048] = {0};
    if (string("txt").compare(format) == 0)
    {
      sprintf(buffer, "%8s %36s %10s %10s %10s %10s %10s %8s %8s %8s\n", "id", "name", "week", "month", "season", "year", "3 year", "all", "score", "ticks");
    }
    else
    {
      sprintf(buffer, "%8s,%36s,%10s,%10s,%10s,%10s,%10s,%8s,%8s,%8s\n", "id", "name", "week", "month", "season", "year", "3 year", "all", "score", "ticks");
    }

    fprintf(fp, "%s", buffer);
    
    for (int i = 0; i < vFundTopNItemGen.size(); i++)
    {
      char buffer[2048] = {0};
      
      if (string("txt").compare(format) == 0)
      {
        sprintf(buffer, "%8s %36s %10s %10s %10s %10s %10s %8s %8.2f %8d\n", 
                      vFundTopNItemGen[i].id.c_str(), vFundTopNItemGen[i].name.c_str(), vFundTopNItemGen[i].weekPro.c_str(), 
                      vFundTopNItemGen[i].monthPro.c_str(), vFundTopNItemGen[i].seasonPro.c_str(), vFundTopNItemGen[i].yearPro.c_str(), 
                      vFundTopNItemGen[i].yearPro_3.c_str(), vFundTopNItemGen[i].allPro.c_str(), vFundTopNItemGen[i].score, vFundTopNItemGen[i].ticks);
      }
      else
      {
        sprintf(buffer, "%8s,%36s,%10s,%10s,%10s,%10s,%10s,%8s,%8.2f,%8d\n", 
                        vFundTopNItemGen[i].id.c_str(), vFundTopNItemGen[i].name.c_str(), vFundTopNItemGen[i].weekPro.c_str(), 
                        vFundTopNItemGen[i].monthPro.c_str(), vFundTopNItemGen[i].seasonPro.c_str(), vFundTopNItemGen[i].yearPro.c_str(), 
                        vFundTopNItemGen[i].yearPro_3.c_str(), vFundTopNItemGen[i].allPro.c_str(), vFundTopNItemGen[i].score, vFundTopNItemGen[i].ticks);

      }
      fprintf(fp, "%s", buffer);
    }

    fflush(fp);
    fclose(fp);
}

void FileDumper::dumpVFundTopNItem2File(vector<FundTopNItemGen>& vItem, string filepath, string format)
{
    dumpVFundTopNItem2File(vItem, filepath.c_str(), format);
}

void FileDumper::dumpVMergedFundTopNItem2File(vector<MergedFundItemGen>& vItem, int lenRank, int lenScore, const char* filename, string format)
{
    if (filename == NULL) return;
    FILE* fp = fopen(filename,"w");
    if (fp == NULL)
    {
      cerr << "open " << filename << " failed" << endl;
      return;
    }

    char* buffer_p = new char[lenRank + lenScore + 1024];
    memset(buffer_p, 0, lenRank + lenScore + 1024);

    if (string("txt").compare(format) == 0)
    {
      sprintf(buffer_p, "\
%8s %36s %4s %4s \
%4s \
%*.*s %*.*s\n", \
"id", "name", "rank", "rkrk", \
"rkun", \
lenRank, lenRank, "rankGens", lenScore, lenScore, "scoreGens");
    }
    else
    {
      sprintf(buffer_p, "\
%8s,%36s,%4s,%4s,\
%4s,\
%*.*s,%*.*s\n",\
"id", "name", "rank", "rkrk", \
"rkun", \
lenRank, lenRank, "rankGens", lenScore, lenScore, "scoreGens");
    }

    fprintf(fp, "%s", buffer_p);
    
    for (int i = 0; i < vItem.size(); i++)
    {
      string rankGens;
      string scoreGens;
      int j;
      for (j = 0; j < vItem[i].vRankOfGens.size(); j++)
      {
        char rank[100] = {0};
        sprintf(rank, "%d", vItem[i].vRankOfGens[j]);
        rankGens += string(rank);
        if (j < vItem[i].vRankOfGens.size() - 1)
        {
            rankGens += string("|");
        }
      }
      for (j = 0; j < vItem[i].vScoreOfGens.size(); j++)
      {
        char score[100] = {0};
        sprintf(score, "%.1f", vItem[i].vScoreOfGens[j]);
        scoreGens += string(score);
        if (j < vItem[i].vScoreOfGens.size() - 1)
        {
            scoreGens += string("|");
        }
      }

      //char buffer[2048] = {0};
      memset(buffer_p, 0, lenRank + lenScore + 1024);
      
      if (string("txt").compare(format) == 0)
      {
        sprintf(buffer_p, "\
%8s %36s %4d %4d \
%4d \
%*.*s %*.*s\n", \
vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].rankOfMerged, vItem[i].rankOfRankVar, \
vItem[i].rankOfUnionRankAndScore, \
lenRank, lenRank, rankGens.c_str(), lenScore, lenScore, scoreGens.c_str());
      }
      else
      {
        sprintf(buffer_p, "\
%8s,%36s,%4d,%4d,\
%4d,\
%*.*s,%*.*s\n",\
vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].rankOfMerged, vItem[i].rankOfRankVar, \
vItem[i].rankOfUnionRankAndScore, \
lenRank, lenRank, rankGens.c_str(), lenScore, lenScore, scoreGens.c_str());
      }
      fprintf(fp, "%s", buffer_p);
    }
    delete [] buffer_p;

    fflush(fp);
    fclose(fp);
}

void FileDumper::dumpVMergedFundTopNItem2File(vector<MergedFundItemGen>& vItem, int lenRank, int lenScore, string filepath, string format)
{
    dumpVMergedFundTopNItem2File(vItem, lenRank, lenScore, filepath.c_str(), format);
}
