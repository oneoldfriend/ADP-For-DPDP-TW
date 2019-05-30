#include "avi.h"
#include "generator.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    //定义计数器，包括总模拟次数和每个instance的模拟次数
    int totalSimulationCount = 0;
    int lagApproximateCount = 0;
    bool startApproximate = false;
    while (totalSimulationCount++ < MAX_SIMULATION)
    {
        lagApproximateCount++;
        if (lagApproximateCount > LAG_APPROXIMATE)
        {
            startApproximate = true;
        }
        string fileName = "TrainingData.txt";
        Generator::instanceGenenrator(fileName);
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
        valueFunction->updateValue(valueAtThisSimulation, startApproximate);
        cout << totalSimulationCount << " " << simulation.solution.cost + simulation.cumRejectionReward << endl;
        for (auto iter = simulation.customers.begin(); iter != simulation.customers.end(); ++iter)
        {
            delete iter->second;
        }
    }
}
