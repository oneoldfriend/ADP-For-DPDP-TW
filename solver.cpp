#include "solver.h"

void Solver::solve()
{
    ValueFunction valueFunction;
    printf("starting!\n");
    //offline approximation
    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);

    /*for (auto iter = valueFunction.lookupTable.value.begin(); iter != valueFunction.lookupTable.value.end(); ++iter)
    {
        cout << iter->first.x << " " << iter->first.y << " " << iter->second << endl;
    }*/

    //online solving
    MDP realShit = MDP("testFile.txt");
    while (realShit.currentState.currentTime < realShit.sequenceData.rbegin()->first)
    {
        int actionNum = 0, maxActionNum = pow(2, realShit.currentState.newCustomers.size()), bestActionNum = -1;
        double bestActionValue = MAXCOST;
        Action bestAction;
        if (realShit.currentState.newCustomers.size() != 0)
        {
            //若有新顾客被观察到
            while (actionNum < maxActionNum)
            {
                //检查每个可能动作的可行性并对可行动作进行评估
                Action a;
                double actionValue = 0;
                realShit.getAction(actionNum, realShit.currentState, &a);
                if (realShit.checkActionFeasibility(a))
                {
                    //若动作可行，则计算立即反馈和后续反馈（value）
                    double immediateReward = realShit.reward(realShit.currentState, a);
                    Aggregation postDecisionState;
                    postDecisionState.aggregate(realShit.currentState, a);
                    actionValue = immediateReward + valueFunction.getValue(postDecisionState, immediateReward);
                    if (actionValue < bestActionValue)
                    {
                        //记录更优的动作
                        bestActionValue = actionValue;
                        bestActionNum = actionNum;
                    }
                }
                actionNum++;
            }
        }
        realShit.getAction(bestActionNum, realShit.currentState, &bestAction);
        Aggregation postDecisionState;
        //计算后决策状态以及聚合后的相关信息
        postDecisionState.aggregate(realShit.currentState, bestAction);
        //状态转移
        realShit.transition(bestAction);
    }

    return;
}