#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    //定义计数器，包括总模拟次数和每个instance的模拟次数
    int totalSimulationCount = 0, instanceNum = 1, instanceCount = 0;
    int simulationPerInstance = MAX_SIMULATION / MAX_TRAINING_INSTANCE;
    while (totalSimulationCount++ < MAX_SIMULATION)
    {
        if (instanceCount == simulationPerInstance)
        {
            //切换到下一个instance
            instanceCount = 0;
            instanceNum++;
        }
        instanceCount++;
        char dayNum[] = {char(instanceNum / 1000000 + 48), char(instanceNum % 1000000 / 100000 + 48), char(instanceNum % 100000 / 10000 + 48),
                         char(instanceNum % 10000 / 1000 + 48), char(instanceNum % 1000 / 100 + 48),
                         char(instanceNum % 100 / 10 + 48), char(instanceNum % 10 + 48), '\0'};
        string fileName = "TrainingData/";
        fileName = fileName + dayNum + ".txt";
        //初始化马尔科夫决策过程
        MDP simulation = MDP(fileName);
        vector<pair<Aggregation, double> > valueAtThisSimulation;
        //开始mdp模拟
        while (!simulation.sequenceData.empty() || !simulation.currentState.newCustomers.empty())
        {
            Action bestAction;
            simulation.findBestAction(&bestAction, *valueFunction);
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
        cout << totalSimulationCount << " " << simulation.solution.cost << endl;
        for (auto iter = simulation.customers.begin(); iter != simulation.customers.end(); ++iter)
        {
            delete iter->second;
        }
    }
}