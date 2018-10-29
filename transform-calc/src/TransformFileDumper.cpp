#include <iostream>
#include <string.h>
#include "TransformFileDumper.h"
using namespace std;

void TransformFileDumper::dumpVTransformFunds2File(vector<vector<TransformFundItem> >& vvItem, int lenRank, int lenScore, const char* filename, string format)
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

    if (string("csv").compare(format) == 0)
    {
        sprintf(buffer_p, "\
%8s,%36s,%6s,%10s,\
%5s,%5s,%12s,\
%*.*s,%*.*s\n", \
"id", "name","type","rank", 
"rkrk", "rkun", "pearson",
lenRank, lenRank, "rankGens",
lenScore, lenScore, "scoreGens");
    }
    else
    {
        sprintf(buffer_p, "\
%8s %36s %6s %10s \
%5s %5s %12s \
%*.*s %*.*s\n", \
"id", "name","type","rank", \
"rkrk", "rkun", "pearson",\
lenRank, lenRank,  "rankGens", \
lenScore, lenScore, "scoreGens");
    }

    fprintf(fp, "%s", buffer_p);
   
    for (int idxVV = 0; idxVV < vvItem.size(); idxVV++)
    {
        vector<TransformFundItem> vItem = vvItem[idxVV];

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

          char rankbuffer[20] = {0};
          sprintf(rankbuffer, "%d/%d", vItem[i].rankOfMerged, vItem[i].totalFundNum);

          //char buffer[2048] = {0};
          memset(buffer_p, 0, lenRank + lenScore + 1024);

          if (string("csv").compare(format) == 0)
          {  
              sprintf(buffer_p, "\
%8s,%36s,%6s,%10s,\
%5d,%5d,%12s,\
%*.*s,%*.*s\n", \
vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].stockTypeName.c_str(), rankbuffer, \
vItem[i].rankOfRankVar, vItem[i].rankOfUnionRankAndScore, vItem[i].pearsonFactor.c_str(),\
lenRank, lenRank, rankGens.c_str(), lenScore, lenScore, scoreGens.c_str());
          }
          else
          {
              sprintf(buffer_p, "\
%8s %36s %6s %10s \
%5d %5d %12s \
%*.*s %*.*s\n", \
vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].stockTypeName.c_str(), rankbuffer, \
vItem[i].rankOfRankVar, vItem[i].rankOfUnionRankAndScore, vItem[i].pearsonFactor.c_str(),\
lenRank, lenRank, rankGens.c_str(), lenScore, lenScore, scoreGens.c_str());
          }
          fprintf(fp, "%s", buffer_p);
        }
        fprintf(fp, "%s", "\n");
    }

    fflush(fp);
    fclose(fp);
    delete [] buffer_p;
}

void TransformFileDumper::dumpVTransformFunds2File(vector<vector<TransformFundItem> >& vvItem, int lenRank, int lenScore, string filepath, string format)
{
    dumpVTransformFunds2File(vvItem, lenRank, lenScore, filepath.c_str(), format);
}


