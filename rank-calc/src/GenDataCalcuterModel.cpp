#include "GenDataCalcuterModel.h"

GenDataCalcuterModel::~GenDataCalcuterModel()
{
}

void GenDataCalcuterModel::initWeightFactor(GenPeriodData& genPeriodData)
{
  if (genPeriodData.iMinFundNum <= 0) return;
  
  //cout << "init weight factor matrix: " << genPeriodData.iMinFundNum << endl;

  W_M.resize(genPeriodData.iMinFundNum);
  W_M_S.resize(genPeriodData.iMinFundNum);
  W_M_S_Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y_A.resize(genPeriodData.iMinFundNum);

  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-1] = 0;  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-2] = 0;
  W_M_S_Y_3Y[genPeriodData.iMinFundNum-1]   = 0;  W_M_S_Y_3Y[genPeriodData.iMinFundNum-2]   = 0;
  W_M_S_Y[genPeriodData.iMinFundNum-1]      = 0;  W_M_S_Y[genPeriodData.iMinFundNum-2]      = 0;
  W_M_S[genPeriodData.iMinFundNum-1]        = 0;  W_M_S[genPeriodData.iMinFundNum-2]        = 2;
  W_M[genPeriodData.iMinFundNum-1]          = 0;  W_M[genPeriodData.iMinFundNum-2]          = 1;

  for (int j = genPeriodData.iMinFundNum-3; j >= 0; j--)
  {
    W_M_S_Y_3Y_A[j] = 0;
    W_M_S_Y_3Y[j]   = 0;
    W_M_S_Y[j]      = 0;
    W_M_S[j]        = W_M_S[j+1] + 1;
    W_M[j]          = W_M[j+1] + 1;
  }
}

GenDataCalcuter GenDataCalcuterModel::getBaseCalcuter()
{
  return *this;
}
