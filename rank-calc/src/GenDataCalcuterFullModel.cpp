#include "GenDataCalcuterFullModel.h"

GenDataCalcuterFullModel::~GenDataCalcuterFullModel()
{
}

void GenDataCalcuterFullModel::initWeightFactor(GenPeriodData& genPeriodData)
{
  if (genPeriodData.iMinFundNum <= 0) return;
  
  //cout << "init weight factor matrix: " << genPeriodData.iMinFundNum << endl;

  W_M.resize(genPeriodData.iMinFundNum);
  W_M_S.resize(genPeriodData.iMinFundNum);
  W_M_S_Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y.resize(genPeriodData.iMinFundNum);
  W_M_S_Y_3Y_A.resize(genPeriodData.iMinFundNum);

  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-1] = 0;  W_M_S_Y_3Y_A[genPeriodData.iMinFundNum-2] = 1;
  W_M_S_Y_3Y[genPeriodData.iMinFundNum-1]   = 0;  W_M_S_Y_3Y[genPeriodData.iMinFundNum-2]   = 2;
  W_M_S_Y[genPeriodData.iMinFundNum-1]      = 0;  W_M_S_Y[genPeriodData.iMinFundNum-2]      = 5;
  W_M_S[genPeriodData.iMinFundNum-1]        = 0;  W_M_S[genPeriodData.iMinFundNum-2]        = 6;
  W_M[genPeriodData.iMinFundNum-1]          = 0;  W_M[genPeriodData.iMinFundNum-2]          = 4;

  for (int j = genPeriodData.iMinFundNum-3; j >= 0; j--)
  {
    W_M_S_Y_3Y_A[j] = W_M_S_Y_3Y_A[j+1] + 1;
    W_M_S_Y_3Y[j]   = W_M_S_Y_3Y[j+1] + 1;
    W_M_S_Y[j]      = W_M_S_Y[j+1] + 2;
    W_M_S[j]        = W_M_S[j+1] + 2;
    W_M[j]          = W_M[j+1] + 2;
  }
}

GenDataCalcuter GenDataCalcuterFullModel::getBaseCalcuter()
{
  return *this;
}

