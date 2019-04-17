#include "mdp.h"

using namespace std;

State::State()
{
    this->currentTime = 0;
    this->pointSolution = nullptr;
}

void MDP::getAction(int actionNum, Action *a)
{
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
    tempSolution.solutionCopy(&(this->solution));
    bool feasibility = tempSolution.greedyInsertion(a);
    tempSolution.solutionDelete();
    return feasibility;
}

MDP::MDP(string fileName)
{
    this->solution = Solution();
    ifstream trainFile(fileName, ios::in);
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
    Action a = Action();
    for (auto iter = this->sequenceData.begin(); iter != this->sequenceData.end(); ++iter)
    {
        if (iter->first == 0)
        {
            this->customers[iter->second->id] = iter->second;
            a.customerConfirmation[iter->second] = true;
            this->currentState.notServicedCustomer.push_back(iter->second->id);
        }
    }
    this->solution.greedyInsertion(a);
    this->currentState.pointSolution = &this->solution;
}

double MDP::reward(State S, Action a)
{
    Solution tempSolution = Solution();
    tempSolution.solutionCopy(&(this->solution));
    double currentCost = tempSolution.cost;
    tempSolution.greedyInsertion(a);
    double newCost = tempSolution.cost;
    tempSolution.solutionDelete();
    return newCost - currentCost;
}

void MDP::transition(Action a)
{

    ///!!!!need modified, need to find the next decision point in whole solution, not in one route, and update the currentPos for the route.

    double lastDecisionTime = this->currentState.currentTime;
    this->solution.greedyInsertion(a);
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
        if (iter->head->next == iter->tail)
        {
            continue;
        }
        else
        {
            while (iter->currentPos->arrivalTime <= lastDecisionTime && iter->currentPos != iter->tail)
            {
                iter->currentPos = iter->currentPos->next;
            }
            if (newDecisionTime > iter->currentPos->arrivalTime)
            {
                newDecisionTime = iter->currentPos->arrivalTime;
            }
        }
    }
    this->currentState.currentTime = newDecisionTime;
    for (auto iter = this->solution.routes.begin(); iter != this->solution.routes.end(); ++iter)
    {
        if (iter->head->next == iter->tail)
        {
            iter->head->departureTime = this->currentState.currentTime;
        }
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