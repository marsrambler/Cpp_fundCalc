#include <stdio.h>
#include <iostream>
#include <string>
#include "FavorFundsProcessor.h"
#include "GenPeriodRepo_Loader.h"
using namespace std;

#define REPO_NUM 4

int main(int argc, char* argv[])
{
    GenPeriodRepo_Loader* repo_loader = new GenPeriodRepo_Loader();
    repo_loader->buildLoaderCfg("../cfg/fund-repos.cfg");

    repo_loader->loadMergedFundItemGen();
    repo_loader->buildDumpDirectory("../cfg/fund-stock.cfg");
    //repo_loader->dumpMergedFundTopN2File();
    
    repo_loader->calcVarianceAfterOfflineLoad();
    repo_loader->calcUnionVariance();
    //repo_loader->dumpMergedFundTopN2FileByVariance();

#if 1
    FavorFundsProcessor* processor_p = new FavorFundsProcessor();  
    processor_p->buildFavoriteFunds(string("../cfg/favor-funds.cfg")); 

    vector<vector<FavorFundItem> >& vvFavorFunds = processor_p->vvFavorFund;
    for (int idxVV = 0; idxVV < vvFavorFunds.size(); idxVV++)
    {
        vector<FavorFundItem>& vFavorFunds = vvFavorFunds[idxVV];
        for (int i = 0; i < vFavorFunds.size(); i++)
        {
            FavorFundItem& item = vFavorFunds[i];
            int repo_idx = repo_loader->getRepoIdxForFund(item.id);
            if ( repo_idx == -1)
            {
                cout << "error, can not get fundId: " << item.id << endl;
                continue;
            }

            MergedFundItemGen& mergedItem = repo_loader->getMergedFundItemGenByFundId(item.id);
            item.assignMergedFundItemGen(mergedItem);
            item.stockTypeName = repo_loader->getRepoType(repo_idx);
            item.totalFundNum  = repo_loader->getRepoMergedItemSize(repo_idx);
/*
            for (int j = 0; j < REPO_NUM; j++)
            {
                if (!genPeriodRepo_p[j]->isFundExistInRepoMergedGen(item.id)) continue;
                int ipos = genPeriodRepo_p[j]->getFundPosInMergedFundTopN(item.id);
                MergedFundItemGen& mergedItem = genPeriodRepo_p[j]->vMergeFundTopN[ipos];
                item.assignMergedFundItemGen(mergedItem);
                item.stockTypeName = genPeriodRepo_p[j]->repoString[genPeriodRepo_p[j]->repo_type];
                item.totalFundNum  = (genPeriodRepo_p[j]->vMergeFundTopN).size();
            }
*/
        }

    }
    //processor_p->displayFavorFundsStatus();
    processor_p->dumpFavorFund2File(repo_loader->getFavorDumpRootPath());

    delete processor_p;
#endif
    
    return 1;
}
