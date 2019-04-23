#include "solver.h"

void Solver::solve()
{
    ValueFunction valueFunction;
    cout << "starting approximation!\n" << endl;
    //offline approximation
    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);
    cout << "finished approximation!\n" << endl;

    //online solving
    MDP realShit = MDP("testFile.txt");
    while (realShit.currentState.currentTime < realShit.sequenceData.rbegin()->first)
    {
        Action bestAction;
        realShit.findBestAction(&bestAction, valueFunction);
        Aggregation postDecisionState;
        //计算后决策状态以及聚合后的相关信息
        postDecisionState.aggregate(realShit.currentState, bestAction);
        //状态转移
        realShit.transition(bestAction);
    }

    return;
}