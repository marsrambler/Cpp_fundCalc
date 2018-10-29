#ifndef FILE_FAVOR_DUMPER_H
#define FILE_FAVOR_DUMPER_H

#include "FileDumper.h"
#include "FavorFundItem.h"

using namespace std;

class FavorFileDumper : public FileDumper
{
public:
  static void dumpVFavorFunds2File(vector<vector<FavorFundItem> >& vItem, int lenRank, int lenScore, const char* filename, string format);
  static void dumpVFavorFunds2File(vector<vector<FavorFundItem> >& vItem, int lenRank, int lenScore, string filepath, string format);
};

#endif
