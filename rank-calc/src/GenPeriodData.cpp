#include "GenPeriodData.h"
#include <fstream>
#include <iostream>
#include "FileDumper.h"
using namespace std;

string GenPeriodData::filename[FILE_NUM_FIX_PER] = {
                        "week.txt",
                        "month.txt",
                        "season.txt",
                        "year.txt",
                        "3year.txt",
                        "all.txt"
                        };

string GenPeriodData::dumpFileName[FILE_NUM_FIX_PER] = {
                        "week-profit.",
                        "month-profit.",
                        "season-profit.",
                        "year-profit.",
                        "3year-profit.",
                        "all-profit."
                        };

GenPeriodData::GenPeriodData()
{
  iMinFundNum = 0;
}

vector<FundItemGen> GenPeriodData::processOneFile(string oneFile)
{
  ifstream ifile(oneFile.c_str());

  string oneLine = "";
  int i = 0;
  vector<FundItemGen> vItem;

  while(getline(ifile, oneLine))
  {
    if (i%2 == 1) {i++; continue;}
    
    i++;
    FundItemGen fItem;
    
    //cout << oneLine << endl;

    size_t fpos  = oneLine.find_first_not_of((char)0x09);
    string str1  = oneLine.substr(fpos);
    fpos = str1.find((char)0x09);
    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);

    //cout << "remain:" << str1 << endl << endl;   
 
    fpos = str1.find((char)0x09);
    string id = str1.substr(0, fpos);
    fItem.id = id;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);     
    str1 = str1.substr(fpos);

    //cout << "id:" << id << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string name = str1.substr(0, fpos);
    fItem.name = name;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);

    //cout << "name:" << name << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);

    //cout << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);

    //cout << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string weekPro = str1.substr(0, fpos);
    fItem.weekPro = weekPro;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "weekPro:" << weekPro << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string monthPro = str1.substr(0, fpos);
    fItem.monthPro = monthPro;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "monthPro:" << monthPro << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string seasonPro = str1.substr(0, fpos);
    fItem.seasonPro = seasonPro;
  
    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "seasonPro:" << seasonPro << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string yearPro = str1.substr(0, fpos);
    fItem.yearPro = yearPro;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "yearPro:" << yearPro << "remain:" << str1 << endl << endl; 

    
    fpos = str1.find((char)0x09);
    string yearPro_3 = str1.substr(0, fpos);
    fItem.yearPro_3 = yearPro_3;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "yearPro_3:" << yearPro_3 << "remain:" << str1 << endl << endl;   

    fpos = str1.find((char)0x09);
    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);

    //cout << "remain:" << str1 << endl << endl;

    fpos = str1.find((char)0x09);
    string allPro = str1.substr(0, fpos);
    fItem.allPro = allPro;

    str1 = str1.substr(fpos);
    fpos = str1.find_first_not_of((char)0x09);
    str1 = str1.substr(fpos);
    //cout << "allPro:" << allPro << "remain:" << str1 << endl << endl;

    vItem.push_back(fItem);
  }
  return vItem;
}

bool GenPeriodData::loadGenData() 
{
  if (vFileName.size() != FILE_NUM_FIX_PER)
  {
    cout << "ERROR: file number not equal! (" << vFileName.size() << " vs " << FILE_NUM_FIX_PER << ")" << endl;
    return false;
  }  

  vWeek.clear();
  vMonth.clear();
  vSeason.clear();
  vYear.clear();
  v3Year.clear();
  vAll.clear();

  vWeek = processOneFile(vFileName[0]);
  iMinFundNum = vWeek.size();
  //cout << "Dump week profit, size: " << vWeek.size() << endl;
  //dumpToFile(vWeek, ".//dump//Week-Profit.txt");

  vMonth = processOneFile(vFileName[1]);
  iMinFundNum = iMinFundNum < vMonth.size() ? iMinFundNum : vMonth.size();
  //cout << "Dump month profit, size: " << vMonth.size() << endl;
  //dumpToFile(vMonth, ".//dump//Month-Profit.txt");

  vSeason = processOneFile(vFileName[2]);
  iMinFundNum = iMinFundNum < vSeason.size() ? iMinFundNum : vSeason.size();
  //cout << "Dump season profit, size: " << vSeason.size() << endl;
  //dumpToFile(vSeason, ".//dump//Season-Profit.txt");

  vYear = processOneFile(vFileName[3]);
  iMinFundNum = iMinFundNum < vYear.size() ? iMinFundNum : vYear.size();
  //cout << "Dump year profit, size: " << vYear.size() << endl;
  //dumpToFile(vYear, ".//dump//Year-Profit.txt");

  v3Year = processOneFile(vFileName[4]);
  iMinFundNum = iMinFundNum < v3Year.size() ? iMinFundNum : v3Year.size();
  //cout << "Dump year_3 profit, size: " << v3Year.size() << endl;
  //dumpToFile(v3Year, ".//dump//Year_3-Profit.txt");

  vAll = processOneFile(vFileName[5]);
  iMinFundNum = iMinFundNum < vAll.size() ? iMinFundNum : vAll.size();
  //cout << "Dump all profit, size: " << vAll.size() << endl;
  //dumpToFile(vAll, ".//dump//All-Profit.txt");

  if (iMinFundNum == 0)
  {
    cout << "There may be no data for the date " << date  << endl;
    return false;
  } 
 
  return true;
}

void GenPeriodData::displayVector(vector<FundItemGen> vItem)
{
  char buffer[2048] = {0};
  sprintf(buffer, "%10s %40s %10s %10s %10s %10s %10s %10s", "id", "name", "week", "month", "season", "year", "3 year", "all");

  cout << buffer << endl;

  for (int i = 0; i < vItem.size(); i++)
  {
    char buffer[2048] = {0};
    sprintf(buffer, "%10s %40s %10s %10s %10s %10s %10s %10s", vItem[i].id.c_str(), vItem[i].name.c_str(), vItem[i].weekPro.c_str(), vItem[i].monthPro.c_str(), vItem[i].seasonPro.c_str(), vItem[i].yearPro.c_str(), vItem[i].yearPro_3.c_str(), vItem[i].allPro.c_str());
    cout << buffer << endl;
  }
}

void GenPeriodData::displayGenData()
{
  displayVector(vWeek);
  displayVector(vMonth);
  displayVector(vSeason);
  displayVector(vYear);
  displayVector(v3Year);
  displayVector(vAll);
} 

void GenPeriodData::dumpGenData2File(string rootPath, string format)
{
  vector<string> vDumpFileName;
  for (int idx = 0; idx < FILE_NUM_FIX_PER; idx++)
  {
    string dumpFile = rootPath + string("//") + dumpFileName[idx] + format;
    vDumpFileName.push_back(dumpFile);
  } 
   
  FileDumper::dumpVFundItemGen2File(vWeek,   vDumpFileName[0].c_str(), format);
  FileDumper::dumpVFundItemGen2File(vMonth,  vDumpFileName[1].c_str(), format);
  FileDumper::dumpVFundItemGen2File(vSeason, vDumpFileName[2].c_str(), format);
  FileDumper::dumpVFundItemGen2File(vYear,   vDumpFileName[3].c_str(), format);
  FileDumper::dumpVFundItemGen2File(v3Year,  vDumpFileName[4].c_str(), format);
  FileDumper::dumpVFundItemGen2File(vAll,    vDumpFileName[5].c_str(), format);
}
