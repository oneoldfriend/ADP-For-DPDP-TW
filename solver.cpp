#include "solver.h"

void Solver::solve()
{
    ValueFunction valueFunction;

    //offline approximation
    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);

    //online solving
    MDP realShit = MDP("testFile.txt");
    vector<pair<Aggregation, double> > valueAtThisSimulation;
    while (!realShit.currentState.notServicedCustomer.empty())
    {
        int actionNum = 0, maxActionNum = pow(2, realShit.currentState.newCustomers.size()), bestActionNum = -1;
        double bestActionValue = MAXVALUE;
        Action bestAction;
        while (actionNum < maxActionNum)
        {
            Action a;
            double actionValue = 0;
            realShit.getAction(actionNum, &a);
            if (realShit.checkActionFeasibility(a))
            {
                double immediateReward = realShit.reward(realShit.currentState, a);
                Aggregation postDecisionState;
                postDecisionState.aggregate(realShit.currentState, a);
                actionValue = immediateReward + valueFunction.getValue(postDecisionState, immediateReward);
                if (actionValue < bestActionValue)
                {
                    bestActionNum = actionNum;
                }
            }
            actionNum++;
        }
        realShit.getAction(bestActionNum, &bestAction);
        Aggregation postDecisionState;
        postDecisionState.aggregate(realShit.currentState, bestAction);
        valueAtThisSimulation.push_back(make_pair(postDecisionState, realShit.reward(realShit.currentState, bestAction)));
        realShit.transition(bestAction);
    }

    return;
}