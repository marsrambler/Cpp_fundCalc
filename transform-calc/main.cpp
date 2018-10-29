#include <stdio.h>
#include <iostream>
#include <string>
//#include "FavorFundsProcessor.h"
#include "TransformFundItem.h"
#include "TransformProcessor.h"
#include "GenPeriodRepo_Loader.h"
#include "PearsonAlgorithm.h"
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

    TransformProcessor* processor_p = new TransformProcessor();
    processor_p->buildFavorTransformRelation(string("../cfg/fund-trans.cfg"));
    processor_p->buildFavoriteFunds(string("../cfg/favor-funds.cfg")); 

    map<string, vector<string> >& favorTransMap = processor_p->favorTransformMap;
    map<string, vector<string> >::iterator iter = favorTransMap.begin();
    
    for (; iter != favorTransMap.end(); iter++)
    {
        string mainFundId = (string)(iter->first);
        vector<string> subFundIds = (vector<string>)(iter->second);

        int repo_idx = repo_loader->getRepoIdxForFund(mainFundId);
        if ( repo_idx == -1)
        {
            cout << "error, can not get fundId: " << mainFundId << endl;
            continue;
        }
        MergedFundItemGen& mainFundItem = repo_loader->getMergedFundItemGenByFundId(mainFundId);

        vector<TransformFundItem> vMainSubFundsMap;
        vMainSubFundsMap.clear();

        for (int i = 0; i < subFundIds.size(); i++)
        {
            repo_idx = repo_loader->getRepoIdxForFund(subFundIds[i]);
            if ( repo_idx == -1)
            {
                cout << "error, can not get fundId: " << subFundIds[i] << endl;
                continue;
            }

            TransformFundItem item;
            item.id = subFundIds[i];

            MergedFundItemGen& mergedItem = repo_loader->getMergedFundItemGenByFundId(subFundIds[i]);
            item.assignMergedFundItemGen(mergedItem);
            item.stockTypeName = repo_loader->getRepoType(repo_idx);
            item.totalFundNum  = repo_loader->getRepoMergedItemSize(repo_idx); 

            double pearsonFactor = PearsonAlgorithm::calc(mainFundItem.vRankOfGens, item.vRankOfGens);
            {
                char buffer[12] = {0};
                sprintf(buffer, "%10f", pearsonFactor);
                item.pearsonFactor = string(buffer);
            }           
            //item.pearsonFactor = PearsonAlgorithm::calc(mainFundItem.vRankOfGens, item.vRankOfGens);

            vMainSubFundsMap.push_back(item);
        }
        processor_p->mainSubFundsMap.insert(make_pair(mainFundItem, vMainSubFundsMap));
    }
    
    processor_p->dumpTransformFund2File(repo_loader->getTransformDumpRootPath());

    delete processor_p;
    
    return 1;
}
