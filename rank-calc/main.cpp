#include <stdio.h>
#include <iostream>
#include <string>
#include "GenPeriodRepo.h"
#include "GenPeriodRepo_Loader.h"
using namespace std;

#define REPO_NUM 4

int main(int argc, char* argv[])
{
#if 1
    GenPeriodRepo::REPO_TYPE repoTypes[REPO_NUM] = {GenPeriodRepo::STOCK, 
                              GenPeriodRepo::COMP, 
                              GenPeriodRepo::DEBT,
                              GenPeriodRepo::INDEX};

    GenPeriodRepo* genPeriodRepo_p[REPO_NUM];

    for (int i = 0; i < REPO_NUM; i++)
    {
        genPeriodRepo_p[i] = new GenPeriodRepo(repoTypes[i]);
        if (genPeriodRepo_p == NULL)
        {
            cerr << "New general period data repository failed" << endl;
            return 1;
        }
        genPeriodRepo_p[i]->buildGenPeriodFilename(string("../cfg/fund-stock.cfg"));
        genPeriodRepo_p[i]->buildDumpDirectory(string("../cfg/fund-stock.cfg"));

        genPeriodRepo_p[i]->loadAllDate();
        genPeriodRepo_p[i]->debugDataStored();
        genPeriodRepo_p[i]->dumpAllGenData2File("csv");

        genPeriodRepo_p[i]->setModelType(GenPeriodRepo::FULL_SEASON);
        genPeriodRepo_p[i]->calcAllDate();
        //genPeriodRepo_p[i]->dumpAllCalcData2File();
        //genPeriodRepo_p[i]->displayAllCaclResultTopN(10);
        //genPeriodRepo_p[i]->dumpAllCalcResultTopN();

        genPeriodRepo_p[i]->mergeAllCalcuter();
        genPeriodRepo_p[i]->calcUnionVariance();
        //genPeriodRepo_p[i]->displayMergedFundTopN(10);
        genPeriodRepo_p[i]->dumpMergedFundTopN2File();
        genPeriodRepo_p[i]->dumpMergedFundTopN2FileByVariance();
    }
#endif

#if 0
    GenPeriodRepo_Loader* repo_loader = new GenPeriodRepo_Loader();
    repo_loader->buildLoaderCfg("../cfg/fund-repos.cfg");

    repo_loader->loadMergedFundItemGen();
    repo_loader->buildDumpDirectory("../cfg/fund-stock.cfg");
    repo_loader->dumpMergedFundTopN2File();
    
    repo_loader->calcVarianceAfterOfflineLoad();
    repo_loader->calcUnionVariance();
    repo_loader->dumpMergedFundTopN2FileByVariance();
#endif
    return 1;
}
