#include "vfa.h"

double LookupTable::lookup(Aggregation postDecisionState)
{
    for (auto iter = this->value.begin(); iter != this->value.end(); ++iter)
    {
        if ((postDecisionState.currentTime >= iter->first.first.currentTime - iter->first.second &&
             postDecisionState.currentTime < iter->first.first.currentTime + iter->first.second) &&
            (postDecisionState.remainTime >= iter->first.first.remainTime - iter->first.second &&
             postDecisionState.remainTime < iter->first.first.remainTime + iter->first.second))
        {
            this->tableInfo[iter->first].first++;
            return iter->second;
        }
    }
}

void LookupTable::partitionUpdate(vector<pair<Aggregation, double>> valueAtThisSimulation)
{
    map<Aggregation, pair<double, double>> furtherInfo;
    
}

void Aggregation::aggregate(State S, Action a)
{
    Route tempPartialRoute = Route();
    tempPartialRoute.creatPartialRoute(S.currentPosition);
    double currentCost = tempPartialRoute.cost;
    Route::greedyInsertion(tempPartialRoute.head, a.customerConfirmation);
    this->currentTime = S.currentTime;
    this->remainTime = MAXWORKTIME - tempPartialRoute.tail->departureTime;
}

double ValueFunction::getValue(Aggregation postDecisionState, double reward)
{
    return this->lookupTable.lookup(postDecisionState);
}

void ValueFunction::updateValue(vector<pair<Aggregation, double>> valueAtThisSimulation)
{
    for (auto decisionPoint = valueAtThisSimulation.begin(); decisionPoint != valueAtThisSimulation.end(); ++decisionPoint)
    {
        for (auto tableIter = this->lookupTable.value.begin(); tableIter != this->lookupTable.value.end();++tableIter){
            if ((decisionPoint->first.currentTime >= tableIter->first.first.currentTime - tableIter->first.second &&
                 decisionPoint->first.currentTime < tableIter->first.first.currentTime + tableIter->first.second) &&
                (decisionPoint->first.remainTime >= tableIter->first.first.remainTime - tableIter->first.second &&
                 decisionPoint->first.remainTime < tableIter->first.first.remainTime + tableIter->first.second))
            {
                this->lookupTable.tableInfo[tableIter->first].second.push_back(decisionPoint->second);
                tableIter->second = (1 - STEPSIZE) * tableIter->second + STEPSIZE * decisionPoint->second;
                break;
            }
        }
    }
    this->lookupTable.partitionUpdate(valueAtThisSimulation);
}