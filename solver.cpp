#include "solver.h"

void Solver::solve()
{
    ValueFunction valueFunction;
    cout << "starting approximation!\n"
         << endl;
    //offline approximation
    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);
    cout << "finished approximation!\n"
         << endl;

    //online solving
    int instanceNum = 0;
    vector<double> testResult;
    while (instanceNum++ < MAX_TEST_INSTANCE)
    {
        char dayNum[] = {char(instanceNum / 10000 + 48), char(instanceNum % 10000 / 1000 + 48), char(instanceNum % 1000 / 100 + 48),
                         char(instanceNum % 100 / 10 + 48), char(instanceNum % 10 + 48), '\0'};
        string fileName = "TestData/";
        fileName = fileName + dayNum + ".txt";
        MDP realShit = MDP(fileName);
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
        testResult.push_back(realShit.solution.cost);
    }
    double resultSum = 0;
    for (auto iter = testResult.begin(); iter != testResult.end(); ++iter)
    {
        resultSum += *iter;
    }
    cout << "Test Average Cost: " << resultSum / double(testResult.size()) << endl;
    return;
}