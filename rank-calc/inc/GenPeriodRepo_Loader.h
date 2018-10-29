#ifndef GEN_PERIOD_REPO_LOADER_H
#define GEN_PERIOD_REPO_LOADER_H

#include <vector>
#include "GenPeriodRepo.h"
using namespace std;

class GenPeriodRepo_Loader
{
    class LoaderCfg
    {
    public:
        string repoType;
        string repoFile;
        vector<string> vRepoDate;
    };

public:
    void buildLoaderCfg(string loaderCfgFileName);
    void loadMergedFundItemGen();
    void calcVarianceAfterOfflineLoad();
    void buildColumn(string columnText, int column_idx, MergedFundItemGen* mergedFundItemGen_p);
    void buildDumpDirectory(string cfgFilename);
    void calcUnionVariance();
    int  getRepoIdxForFund(string fundId);
    MergedFundItemGen& getMergedFundItemGenByFundId(string fundId);
    string getRepoType(int repo_idx);
    int getRepoMergedItemSize(int repo_idx);
    string getFavorDumpRootPath();
    string getTransformDumpRootPath();

public:
    void dumpMergedFundTopN2File();
    void dumpMergedFundTopN2FileByVariance();

private:
    string trimString(string szString);

public:
    vector<LoaderCfg> vLoaderCfg;
    vector<GenPeriodRepo*> v_genPeriodRepo;
};
#endif
