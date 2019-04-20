#include "mdp.h"
#include "route.h"

using namespace std;

State::State()
{
    this->currentTime = 0;
    this->pointSolution = nullptr;
}

void MDP::getAction(int actionNum, Action *a)
{
    //根据当前动作号进行二进制转换为具体动作
    int leftOver = actionNum;
    for (auto confirmIter = a->customerConfirmation.begin(); confirmIter != a->customerConfirmation.end(); ++confirmIter)
    {
        confirmIter->second = leftOver % 2;
        leftOver = leftOver / 2;
    }
}

bool MDP::checkActionFeasibility(Action a)
{
    Solution tempSolution = Solution();
    //生成当前解的副本并对副本执行动作
    tempSolution.solutionCopy(&this->solution);
    bool feasibility = tempSolution.greedyInsertion(a);
    tempSolution.solutionDelete();
    return feasibility;
}

MDP::MDP(string fileName)
{
    this->solution = Solution();
    ifstream trainFile(fileName, ios::in);
    double appearTime = 0;
    while (!trainFile.eof())
    {
        //读取instance数据
        trainFile >> appearTime;
        Customer *customer = new Customer();
        trainFile >> customer->id;
        trainFile >> customer->origin.x;
        trainFile >> customer->origin.y;
        trainFile >> customer->dest.x;
        trainFile >> customer->dest.y;
        trainFile >> customer->startTime;
        trainFile >> customer->endTime;
        trainFile >> customer->weight;
        trainFile >> customer->priority;
        this->sequenceData.push_back(make_pair(appearTime, customer));
    }
    trainFile.close();
    this->currentState = State();
    Action a = Action();
    for (auto iter = this->sequenceData.begin(); iter != this->sequenceData.end(); ++iter)
    {
        if (iter->first == 0.0)
        {
            //对提前已知的顾客进行路径规划
            this->customers[iter->second->id] = iter->second;
            a.customerConfirmation[iter->second] = true;
            this->currentState.notServicedCustomer.push_back(iter->second->id);
        }
        else
        {
            break;
        }
    }
    this->solution.greedyInsertion(a);
    this->currentState.pointSolution = &this->solution;
}

double MDP::reward(State S, Action a)
{
    //复制当前解并对副本执行动作计算立即反馈
    Solution tempSolution = Solution();
    tempSolution.solutionCopy(&this->solution);
    double currentCost = tempSolution.cost;
    tempSolution.greedyInsertion(a);
    double newCost = tempSolution.cost;
    tempSolution.solutionDelete();
    return newCost - currentCost;
}

void MDP::transition(Action a)
{
    //执行动作
    double lastDecisionTime = this->currentState.currentTime;
    this->solution.greedyInsertion(a);
    //更新当前状态
    for (auto iter = a.customerConfirmation.begin(); iter != a.customerConfirmation.end(); ++iter)
    {
        if (iter->second)
        {
            this->currentState.notServicedCustomer.push_back(iter->first->id);
        }
    }
    double newDecisionTime = MAXWORKTIME;
    for (auto iter = this->solution.routes.begin(); iter != this->solution.routes.end(); ++iter)
    {
        //更新当前解中每条路径的当前位置
        if (iter->currentPos->next == iter->tail)
        {
            continue;
        }
        else
        {
            while (iter->currentPos->arrivalTime <= lastDecisionTime && iter->currentPos != iter->tail)
            {
                //若路径当前位置的到达时间小于当前时间
                if (!iter->currentPos->isOrigin)
                {
                    //若当前位置是顾客点，则将当前顾客视为已服务，并更新状态相关信息
                    for (auto iter2 = this->currentState.notServicedCustomer.begin(); iter2 != this->currentState.notServicedCustomer.end(); ++iter2)
                    {
                        if (iter->currentPos->customer->id == *iter2)
                        {
                            this->currentState.notServicedCustomer.erase(iter2);
                            break;
                        }
                    }
                }
                //把当前位置推到下一个不小于当前时间的访问点
                iter->currentPos = iter->currentPos->next;
            }
            if (newDecisionTime > iter->currentPos->arrivalTime)
            {
                //寻找新状态的决策时间（即解中最早到达下一个访问点的时间）
                newDecisionTime = iter->currentPos->arrivalTime;
            }
        }
    }
    this->currentState.currentTime = newDecisionTime;
    for (auto iter = this->solution.routes.begin(); iter != this->solution.routes.end(); ++iter)
    {
        //更新在仓库待命的车辆最早出发时间为当前状态的时间
        if (iter->currentPos->next == iter->tail)
        {
            iter->currentPos->departureTime = this->currentState.currentTime;
        }
    }
    //观察新顾客信息
    this->observation(lastDecisionTime);
}

void MDP::observation(double lastDecisionTime)
{
    for (auto sequenceIter = this->sequenceData.begin(); sequenceIter != this->sequenceData.end(); ++sequenceIter)
    {
        //找到上次决策时间到当前决策时间中产生的新顾客信息
        if (sequenceIter->first > lastDecisionTime && sequenceIter->first <= this->currentState.currentTime)
        {
            if (sequenceIter->second->priority == 1)
            {
                //若有新顾客产生，则将其加入到待插入顾客集中
                this->customers[sequenceIter->second->id] = sequenceIter->second;
                this->currentState.newCustomers.push_back(sequenceIter->second->id);
            }
            else
            {
                //若观察到顾客退单或催单，则直接对原有顾客信息进行更新
                this->customers[sequenceIter->second->id]->priority = sequenceIter->second->priority;
            }
        }
        if (sequenceIter->first > this->currentState.currentTime)
        {
            break;
        }
    }

    //update the solution(delete the customers with cancellation)

    for (auto routeIter = this->solution.routes.begin(); routeIter != this->solution.routes.end(); ++routeIter)
    {
        PointOrder p = routeIter->currentPos;
        bool cancellation = false;
        while (p != routeIter->tail)
        {
            //检查若有顾客退单，则将其从解中删除
            if (p->customer->priority == 0)
            {
                cancellation = true;
                PointOrder tempPtr = p->next;
                routeIter->removeOrder(p);
                p = tempPtr;
            }
            else
            {
                p = p->next;
            }
        }
        if (cancellation)
        {
            //若有顾客退单导致解的变化，则更新对应路径的信息
            routeIter->routeUpdate();
        }
    }
}