#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    int count = 0;
    while (count++ < MAXSIMULATION)
    {
        char dayNum[] = {char(count / 1000 + 48), char(count % 1000 / 100 + 48),
                      char(count / 100 + 48), char(count % 100 / 10 + 48), char(count % 10 + 48)};
        string fileName = "TrainingData/";
        fileName = fileName + +dayNum + ".txt";
        MDP simulation = MDP(fileName);
        vector<pair<Aggregation, double> > valueAtThisSimulation;
        while (!simulation.currentState.notServicedCustomer.empty())
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size()), bestActionNum = -1;
            double bestActionValue = MAXVALUE;
            Action bestAction;
            while (actionNum < maxActionNum)
            {
                Action a;
                double actionValue = 0;
                simulation.getAction(actionNum, &a);
                if (simulation.checkActionFeasibility(a))
                {
                    double immediateReward = simulation.reward(simulation.currentState, a);
                    Aggregation postDecisionState;
                    postDecisionState.aggregate(simulation.currentState, a);
                    actionValue = immediateReward + valueFunction->getValue(postDecisionState, immediateReward);
                    if (actionValue < bestActionValue)
                    {
                        bestActionNum = actionNum;
                    }
                }
                actionNum++;
            }
            simulation.getAction(bestActionNum, &bestAction);
            Aggregation postDecisionState;
            postDecisionState.aggregate(simulation.currentState, bestAction);
            valueAtThisSimulation.push_back(make_pair(postDecisionState, simulation.reward(simulation.currentState, bestAction)));
            simulation.transition(bestAction);
        }
        valueFunction->updateValue(valueAtThisSimulation);
    }
}