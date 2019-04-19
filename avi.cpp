#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    int totalSimulationCount = 0, instanceNum = 1, simulationPerInstance = 10, instanceCount = 0;
    while (totalSimulationCount++ < MAXSIMULATION)
    {
        if (instanceCount == simulationPerInstance)
        {
            instanceCount = 0;
            instanceNum++;
        }
        else
        {
            instanceCount++;
        }
        cout << instanceNum << " " << instanceCount << endl;
        char dayNum[] = {char(instanceNum / 1000 + 48), char(instanceNum % 1000 / 100 + 48),
                         char(instanceNum % 100 / 10 + 48), char(instanceNum % 10 + 48), '\0'};
        string fileName = "TrainingData/";
        fileName = fileName + dayNum + ".txt";
        MDP simulation = MDP(fileName);
        vector<pair<Aggregation, double> > valueAtThisSimulation;
        while (simulation.currentState.currentTime < simulation.sequenceData.rbegin()->first)
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size()), bestActionNum = -1;
            double bestActionValue = MAXVALUE;
            Action bestAction;
            if (simulation.currentState.newCustomers.size() != 0)
            {
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
                            bestActionValue = actionValue;
                            bestActionNum = actionNum;
                        }
                    }
                    actionNum++;
                }
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