#ifndef FILE_TRANSFORM_DUMPER_H
#define FILE_TRANSFORM_DUMPER_H

#include "FileDumper.h"
#include "TransformFundItem.h"

using namespace std;

class TransformFileDumper : public FileDumper
{
public:
  static void dumpVTransformFunds2File(vector<vector<TransformFundItem> >& vItem, int lenRank, int lenScore, const char* filename, string format);
  static void dumpVTransformFunds2File(vector<vector<TransformFundItem> >& vItem, int lenRank, int lenScore, string filepath, string format);
};

#endif

