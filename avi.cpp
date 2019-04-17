#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    int count = 0;
    while (count++ < MAXSIMULATION)
    {
        MDP simulation = MDP(count);
        vector<pair<Aggregation, double>> valueAtThisSimulation;
        while (!simulation.currentState.notServicedCustomer.empty())
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size() + 1), bestActionNum = -1;
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