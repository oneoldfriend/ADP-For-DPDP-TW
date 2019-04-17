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
    map<Aggregation, double> entryTheta;
    int entryNum = this->value.size();
    double totalN = 0, totalTheta = 0;
    for (auto tableIter = this->value.begin(); tableIter != this->value.end(); ++tableIter)
    {
        totalN += this->tableInfo[tableIter->first].first;
        entryTheta[tableIter->first.first] = Util::standardDeviation(this->tableInfo[tableIter->first].second);
        totalTheta += entryTheta[tableIter->first.first];
    }
    double averageN = totalN / entryNum, averageTheta = totalTheta / entryNum;
    auto tableIter = this->value.begin();
    for (int count = 0; count < entryNum; count++)
    {
        double factor1 = this->tableInfo[tableIter->first].first / averageN;
        double factor2 = entryTheta[tableIter->first.first] / averageTheta;
        if (factor1 * factor2 > PARTITIONTHRESHOLD)
        {
            this->partition(tableIter);
            this->value.erase(tableIter++);
        }
        else
        {
            ++tableIter;
        }
    }
}

void LookupTable::partition(map<pair<Aggregation, double>, double>::iterator tableIter)
{
    Aggregation partition1, partition2, partition3, partition4;
    partition1.currentTime = tableIter->first.first.currentTime + tableIter->first.second / 2.0;
    partition1.remainTime = tableIter->first.first.remainTime + tableIter->first.second / 2.0;
    partition2.currentTime = tableIter->first.first.currentTime + tableIter->first.second / 2.0;
    partition2.remainTime = tableIter->first.first.remainTime - tableIter->first.second / 2.0;
    partition3.currentTime = tableIter->first.first.currentTime - tableIter->first.second / 2.0;
    partition3.remainTime = tableIter->first.first.remainTime - tableIter->first.second / 2.0;
    partition4.currentTime = tableIter->first.first.currentTime - tableIter->first.second / 2.0;
    partition4.remainTime = tableIter->first.first.remainTime + tableIter->first.second / 2.0;
    this->value[make_pair(partition1, tableIter->first.second / 2)] = tableIter->second;
    this->value[make_pair(partition2, tableIter->first.second / 2)] = tableIter->second;
    this->value[make_pair(partition3, tableIter->first.second / 2)] = tableIter->second;
    this->value[make_pair(partition4, tableIter->first.second / 2)] = tableIter->second;
    this->tableInfo[make_pair(partition1, tableIter->first.second / 2)].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[make_pair(partition2, tableIter->first.second / 2)].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[make_pair(partition3, tableIter->first.second / 2)].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[make_pair(partition4, tableIter->first.second / 2)].first = this->tableInfo[tableIter->first].first / 4;
}

void Aggregation::aggregate(State S, Action a)
{
    Solution tempSolution = Solution();
    tempSolution.solutionCopy(S.pointSolution);
    tempSolution.greedyInsertion(a);
    this->currentTime = S.currentTime;
    this->remainTime = 0;
    for (auto iter = tempSolution.routes.begin(); iter != tempSolution.routes.end(); ++iter)
    {
        if (iter->head->next == iter->tail)
        {
            this->remainTime += MAXWORKTIME - iter->head->departureTime;
        }
        else
        {
            this->remainTime += MAXWORKTIME - iter->tail->departureTime;
        }
    }
    tempSolution.solutionDelete();
}

double ValueFunction::getValue(Aggregation postDecisionState, double reward)
{
    return this->lookupTable.lookup(postDecisionState);
}

void ValueFunction::updateValue(vector<pair<Aggregation, double>> valueAtThisSimulation)
{
    for (auto decisionPoint = valueAtThisSimulation.begin(); decisionPoint != valueAtThisSimulation.end(); ++decisionPoint)
    {
        for (auto tableIter = this->lookupTable.value.begin(); tableIter != this->lookupTable.value.end(); ++tableIter)
        {
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