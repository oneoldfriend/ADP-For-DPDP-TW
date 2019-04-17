#include "solution.h"
#include "route.h"

Solution::Solution()
{
    for (int i = 0; i < MAXVEHICLE; i++)
    {
        Route route = Route(false);
        this->routes.push_back(route);
    }
    this->cost = 0;
}

bool Solution::greedyInsertion(Action a)
{
    vector<Customer *> customerToBeInserted;
    for (auto iter = a.customerConfirmation.begin(); iter != a.customerConfirmation.end(); ++iter)
    {
        if (iter->second)
        {
            customerToBeInserted.push_back(iter->first);
        }
    }
    for (auto customerIter = customerToBeInserted.begin(); customerIter != customerToBeInserted.end(); ++customerIter)
    {
        bool feasibility = false;
        PointOrder origin = new Order(*customerIter, true);
        PointOrder dest = new Order(*customerIter, false);
        pair<PointOrder, PointOrder> bestOriginPos, bestDestPos;
        PointRoute bestRoute = nullptr;
        double bestCost = MAXCOST;
        Solution tempSolution;
        for (auto routeIter = this->routes.begin(); routeIter != this->routes.end(); ++routeIter)
        {
            double oldCost = bestCost;
            if (routeIter->findBestPosition(origin, dest, &bestOriginPos, &bestDestPos, &bestCost))
            {
                feasibility = true;
                if (oldCost > bestCost)
                {
                    bestRoute = &(*routeIter);
                }
            }
        }
        if (feasibility == false)
        {
            delete origin;
            delete dest;
            return feasibility;
        }
        else
        {
            origin->prior = bestOriginPos.first;
            origin->next = bestOriginPos.second;
            dest->prior = bestDestPos.first;
            dest->next = bestDestPos.second;
            bestRoute->insertOrder(dest);
            bestRoute->insertOrder(origin);
            bestRoute->routeUpdate();
        }
    }
    this->cost = this->calcCost();
    return true;
}

double Solution::calcCost()
{
    double cost = 0;
    for (auto iter = this->routes.begin(); iter != this->routes.end(); ++iter)
    {
        cost += iter->cost;
    }
    return cost;
}

void Solution::solutionCopy(Solution *source)
{
    for (auto iter = source->routes.begin(); iter != source->routes.end(); ++iter)
    {
        Route *newCopyRoute = new Route(true);
        newCopyRoute->routeCopy(*iter);
    }
    this->cost = source->cost;
}

void Solution::solutionDelete()
{
    for (auto iter = this->routes.begin(); iter != this->routes.end(); ++iter)
    {
        iter->deleteRoute();
    }
}