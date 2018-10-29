#ifndef FILE_DUMPER_H
#define FILE_DUMPER_H

#include <vector>
#include <string>
#include "FundItemGen.h"
#include "FundTopNItemGen.h"
#include "MergedFundItemGen.h"

using namespace std;

class FileDumper
{
public:
  static void dumpVFundItemGen2File(vector<FundItemGen> vItem, const char* filename, string format);
  static void dumpVFundTopNItem2File(vector<FundTopNItemGen>& vItem, const char* filename, string format);
  static void dumpVFundTopNItem2File(vector<FundTopNItemGen>& vItem, string filepath, string format);

  static void dumpVMergedFundTopNItem2File(vector<MergedFundItemGen>& vItem, int lenRank, int lenScore, const char* filename, string format);
  static void dumpVMergedFundTopNItem2File(vector<MergedFundItemGen>& vItem, int lenRank, int lenScore, string filepath, string format);

};

#endif
