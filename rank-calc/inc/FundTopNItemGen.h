#ifndef FUNDTOPNITEM
#define FUNDTOPNITEM

#include <string>
using namespace std;

class FundTopNItemGen
{
public:
  string id;
  string name;
  string weekPro;
  string monthPro;
  string seasonPro;
  string yearPro;
  string yearPro_3;
  string allPro;
  double score;
  int ticks;
 
  FundTopNItemGen(); 
  bool operator < (const FundTopNItemGen& m) const;
};

#endif

