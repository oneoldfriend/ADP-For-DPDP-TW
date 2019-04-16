#include "mdp.h"

using namespace std;

State::State()
{
    this->currentTime = 0;
}

void State::getAction(int actionNum, Action *a)
{
    int leftOver = actionNum;
    for (auto confirmIter = a->customerConfirmation.begin(); confirmIter != a->customerConfirmation.end(); ++confirmIter)
    {
        confirmIter->second = leftOver % 2;
        leftOver = leftOver / 2;
    }
    a->movement = leftOver % 2;
}

bool State::checkActionFeasibility(Action a)
{
    Route tempPartialRoute = Route(true);
    tempPartialRoute.creatPartialRoute(this->currentRoute->currentPos);
    bool feasibility = tempPartialRoute.greedyInsertion(a);
    tempPartialRoute.deleteRoute();
    return feasibility;
}

MDP::MDP(int trainDayNum)
{
    this->solution = Solution();
    string num = {char(trainDayNum / 1000 + 48), char(trainDayNum % 1000 / 100 + 48),
                  char(trainDayNum / 100 + 48), char{trainDayNum % 100 / 10 + 48}, char(trainDayNum % 10 + 48)};
    num = "trainData" + num + ".txt ";
    ifstream trainFile(num.c_str(), ios::in);
    double appearTime = 0;
    trainFile >> appearTime;
    while (appearTime != EOF)
    {
        Customer *customer = new Customer();
        trainFile >> customer->id;
        trainFile >> customer->origin.x;
        trainFile >> customer->origin.y;
        trainFile >> customer->dest.x;
        trainFile >> customer->dest.y;
        trainFile >> customer->startTime;
        trainFile >> customer->endTime;
        trainFile >> customer->weight;
        this->sequenceData.push_back(make_pair(appearTime, customer));
        trainFile >> appearTime;
    }
    trainFile.close();
    this->currentState = State();
    vector<Customer *> initialCustomers;
    for (auto iter = this->sequenceData.begin(); iter != this->sequenceData.end(); ++iter)
    {
        if (iter->first == 0)
        {
            this->customers[iter->second->id] = iter->second;
            initialCustomers.push_back(iter->second);
            this->currentState.notServicedCustomer.push_back(iter->second->id);
        }
    }
    this->solution.initialConstruction(initialCustomers);
}

double MDP::reward(State S, Action a)
{
    Route tempPartialRoute = Route(true);
    tempPartialRoute.creatPartialRoute(S.currentRoute->currentPos);
    double currentCost = tempPartialRoute.cost;
    tempPartialRoute.greedyInsertion(a);
    double newCost = tempPartialRoute.cost;
    tempPartialRoute.deleteRoute();
    return newCost - currentCost;
}

void MDP::transition(Action a)
{

    ///!!!!need modified, need to find the next decision point in whole solution, not in one route, and update the currentPos for the route.

    double lastDecisionTime = this->currentState.currentTime;
    this->currentState.currentRoute->greedyInsertion(a);
    for (auto iter = a.customerConfirmation.begin(); iter != a.customerConfirmation.end(); ++iter)
    {
        if (iter->second)
        {
            this->currentState.notServicedCustomer.push_back(iter->first->id);
        }
    }
    if (a.movement)
    {
        if (!this->currentState.currentRoute->currentPos->isOrigin)
        {
            for (auto iter = this->currentState.notServicedCustomer.begin(); iter != this->currentState.notServicedCustomer.end(); ++iter)
            {
                if (*iter == this->currentState.currentRoute->currentPos->customer->id)
                {
                    this->currentState.notServicedCustomer.erase(iter);
                    break;
                }
            }
        }
        this->currentState.currentRoute->currentPos = this->currentState.currentRoute->currentPos->next;
        this->currentState.currentTime = this->currentState.currentRoute->currentPos->arrivalTime;
    }
    else
    {
        this->currentState.currentTime += 1.0;
    }
    this->observation(lastDecisionTime);
}

void MDP::observation(double lastDecisionTime)
{
    for (auto sequenceIter = this->sequenceData.begin(); sequenceIter != this->sequenceData.end(); ++sequenceIter)
    {
        if (sequenceIter->first > lastDecisionTime && sequenceIter->first <= this->currentState.currentTime)
        {
            if (sequenceIter->second->priority == 1)
            {
                this->customers[sequenceIter->second->id] = sequenceIter->second;
                this->currentState.newCustomers.push_back(sequenceIter->second->id);
            }
            else
            {
                this->customers[sequenceIter->second->id]->priority = sequenceIter->second->priority;
            }
        }
    }

    //update the solution(delete the customers with cancellation)

    for (auto routeIter = this->solution.routes.begin(); routeIter != this->solution.routes.end(); ++routeIter)
    {
        PointOrder p = routeIter->currentPos;
        while (p != nullptr)
        {
            if (p->customer->priority == 0)
            {
                PointOrder tempPtr = p->next;
                routeIter->removeOrder(p);
                p = tempPtr;
            }
            else
            {
                p = p->next;
            }
        }
        routeIter->routeUpdate();
    }
}