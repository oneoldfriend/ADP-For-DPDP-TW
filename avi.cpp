#include "avi.h"

void AVI::approximation(ValueFunction *valueFunction)
{
    //定义计数器，包括总模拟次数和每个instance的模拟次数
    int totalSimulationCount = 0, instanceNum = 1, simulationPerInstance = 5, instanceCount = 0;
    while (totalSimulationCount++ < MAXSIMULATION)
    {
        if (instanceCount == simulationPerInstance)
        {
            instanceCount = 1;
            instanceNum++;
        }
        else
        {
            instanceCount++;
        }
        char dayNum[] = {char(instanceNum / 1000 + 48), char(instanceNum % 1000 / 100 + 48),
                         char(instanceNum % 100 / 10 + 48), char(instanceNum % 10 + 48), '\0'};
        string fileName = "TrainingData/";
        fileName = fileName + dayNum + ".txt";
        //初始化马尔科夫决策过程
        MDP simulation = MDP(fileName);
        vector<pair<Aggregation, double> > valueAtThisSimulation;
        //开始mdp模拟
        while (!simulation.sequenceData.empty() || !simulation.currentState.newCustomers.empty())
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size()), bestActionNum = -1;
            double bestActionValue = MAXCOST;
            Action bestAction;
            if (simulation.currentState.newCustomers.size() != 0)
            {
                //若有新顾客被观察到
                while (actionNum < maxActionNum)
                {
                    //检查每个可能动作的可行性并对可行动作进行评估
                    Action a;
                    double actionValue = 0;
                    simulation.getAction(actionNum, simulation.currentState, &a);
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

            simulation.getAction(bestActionNum, simulation.currentState, &bestAction);
            Aggregation postDecisionState;
            //计算后决策状态以及聚合后的相关信息
            postDecisionState.aggregate(simulation.currentState, bestAction);
            //记录这次sample path的信息
            valueAtThisSimulation.push_back(make_pair(postDecisionState, simulation.reward(simulation.currentState, bestAction)));
            /*cout << "current time:" << simulation.currentState.currentTime << endl;
            for (auto iter = simulation.currentState.newCustomers.begin(); iter != simulation.currentState.newCustomers.end(); ++iter)
            {
                cout << "new customer:" << *iter << endl;
            }
            for (auto iter = bestAction.customerConfirmation.begin(); iter != bestAction.customerConfirmation.end(); ++iter)
            {
                if (iter->second == false)
                {
                    cout << "rejected" << iter->first->id << endl;
                }
                else
                {
                    cout << "accept" << iter->first->id << endl;
                }
            }*/
            //状态转移
            simulation.transition(bestAction);
            for (auto iter = simulation.solution.routes.begin(); iter != simulation.solution.routes.end(); ++iter)
            {
                PointOrder p = iter->head;
                while (p != nullptr)
                {
                    cout << p->customer->id << " ";
                    p = p->next;
                }
                cout << endl;
            }
        }
        //对lookup table 进行更新
        valueFunction->updateValue(valueAtThisSimulation);
        for (auto iter = simulation.solution.routes.begin(); iter != simulation.solution.routes.end(); ++iter)
        {
            PointOrder p = iter->head;
            while (p != nullptr)
            {
                cout << p->customer->id << p->isOrigin << " ";
                p = p->next;
            }
            cout << endl;
        }
        return;
    }
}