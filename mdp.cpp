#include "mdp.h"

using namespace std;


void State::getAction(int actionNum, Action *a){
    int leftOver = actionNum;
    for (auto confirmIter = a->customerConfirmation.begin(); confirmIter != a->customerConfirmation.end();++confirmIter){
        confirmIter->second = leftOver % 2;
        leftOver = leftOver / 2;
    }
    a->movement = leftOver % 2;
}

bool State::checkActionFeasibility(Action a){
    Route tempPartialRoute = Route(true);
    tempPartialRoute.creatPartialRoute(this->currentRoute->currentPos);
    bool feasibility = tempPartialRoute.greedyInsertion(a);
    tempPartialRoute.deleteRoute();
    return feasibility;
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
    this->observation();
}

void MDP::observation()
{
    //read info from files and update the customer and the state

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