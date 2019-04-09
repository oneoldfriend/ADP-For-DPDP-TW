#include "mdp.h"

using namespace std;

double MDP::reward(State S, Action a)
{
    Route tempPartialRoute = Route();
    tempPartialRoute.creatPartialRoute(S.currentPosition);
    double currentCost = tempPartialRoute.cost;
    Route::greedyInsertion(tempPartialRoute.head, a.customerConfirmation);
    return tempPartialRoute.cost - currentCost;
}

void MDP::transition(State *S, Action a)
{
    Route::greedyInsertion(S->currentPosition, a.customerConfirmation);
    for (auto iter = a.customerConfirmation.begin(); iter != a.customerConfirmation.end(); ++iter)
    {
        if (iter->second)
        {
            S->unservicedCustomer.push_back(iter->first);
        }
    }
    if (a.movement)
    {
        if (!S->currentPosition->isOrigin)
        {
            for (auto iter = S->unservicedCustomer.begin(); iter != S->unservicedCustomer.end(); ++iter)
            {
                if (*iter == S->currentPosition->customer->id)
                {
                    S->unservicedCustomer.erase(iter);
                    break;
                }
            }
        }
        S->currentPosition = S->currentPosition->next;
        S->currentTime = S->currentPosition->arrivalTime;
    }
    else
    {
        S->currentTime += 1.0;
    }
    this->observation(&S->newCustomers);
}

void MDP::observation(vector<string> *newCustomer)
{
    
}