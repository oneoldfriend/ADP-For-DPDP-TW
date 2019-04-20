#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    //定义计数器，包括总模拟次数和每个instance的模拟次数
    int totalSimulationCount = 0, instanceNum = 1, simulationPerInstance = 5, instanceCount = 0;
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
        //初始化马尔科夫决策过程
        MDP simulation = MDP(fileName);
        vector<pair<Aggregation, double> > valueAtThisSimulation;
        //开始mdp模拟
        while (simulation.currentState.currentTime < simulation.sequenceData.rbegin()->first)
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size()), bestActionNum = -1;
            double bestActionValue = MAXVALUE;
            Action bestAction;
            if (simulation.currentState.newCustomers.size() != 0)
            {
                //若有新顾客被观察到
                while (actionNum < maxActionNum)
                {
                    //检查每个可能动作的可行性并对可行动作进行评估
                    Action a;
                    double actionValue = 0;
                    simulation.getAction(actionNum, &a);
                    if (simulation.checkActionFeasibility(a))
                    {
                        //若动作可行，则计算立即反馈和后续反馈（value）
                        double immediateReward = simulation.reward(simulation.currentState, a);
                        Aggregation postDecisionState;
                        postDecisionState.aggregate(simulation.currentState, a);
                        actionValue = immediateReward + valueFunction->getValue(postDecisionState, immediateReward);
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
            simulation.getAction(bestActionNum, &bestAction);
            Aggregation postDecisionState;
            //计算后决策状态以及聚合后的相关信息
            postDecisionState.aggregate(simulation.currentState, bestAction);
            //记录这次sample path的信息
            valueAtThisSimulation.push_back(make_pair(postDecisionState, simulation.reward(simulation.currentState, bestAction)));
            //状态转移
            simulation.transition(bestAction);
        }
        //对lookup table 进行更新
        valueFunction->updateValue(valueAtThisSimulation);
    }
}