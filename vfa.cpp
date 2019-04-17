#include "vfa.h"

LookupTable::LookupTable()
{
}

double LookupTable::lookup(Aggregation postDecisionState)
{
    for (auto iter = this->value.begin(); iter != this->value.end(); ++iter)
    {
        if ((postDecisionState.currentTime >= iter->first.x - iter->first.r &&
             postDecisionState.currentTime < iter->first.x + iter->first.r) &&
            (postDecisionState.remainTime >= iter->first.x - iter->first.r &&
             postDecisionState.remainTime < iter->first.x + iter->first.r))
        {
            this->tableInfo[iter->first].first = this->tableInfo[iter->first].first + 1;
            return iter->second;
        }
    }
}

void LookupTable::partitionUpdate()
{
    map<Entry, double> entryTheta;
    int entryNum = this->value.size();
    double totalN = 0, totalTheta = 0;
    for (auto tableIter = this->value.begin(); tableIter != this->value.end(); ++tableIter)
    {
        totalN += this->tableInfo[tableIter->first].first;
        entryTheta[tableIter->first] = Util::standardDeviation(this->tableInfo[tableIter->first].second);
        totalTheta += entryTheta[tableIter->first];
    }
    double averageN = totalN / entryNum, averageTheta = totalTheta / entryNum;
    auto tableIter = this->value.begin();
    for (int count = 0; count < entryNum; count++)
    {
        double factor1 = this->tableInfo[tableIter->first].first / averageN;
        double factor2 = entryTheta[tableIter->first] / averageTheta;
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

void LookupTable::partition(map<Entry, double>::iterator tableIter)
{
    Entry partition1, partition2, partition3, partition4;
    partition1.x = tableIter->first.x + tableIter->first.r / 2.0;
    partition1.y = tableIter->first.x + tableIter->first.r / 2.0;
    partition2.x = tableIter->first.x + tableIter->first.r / 2.0;
    partition2.y = tableIter->first.y - tableIter->first.r / 2.0;
    partition3.x = tableIter->first.x - tableIter->first.r / 2.0;
    partition3.y = tableIter->first.y - tableIter->first.r / 2.0;
    partition4.x = tableIter->first.x - tableIter->first.r / 2.0;
    partition4.y = tableIter->first.y + tableIter->first.r / 2.0;
    this->value[partition1] = tableIter->second;
    this->value[partition2] = tableIter->second;
    this->value[partition3] = tableIter->second;
    this->value[partition4] = tableIter->second;
    this->tableInfo[partition1].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[partition2].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[partition3].first = this->tableInfo[tableIter->first].first / 4;
    this->tableInfo[partition4].first = this->tableInfo[tableIter->first].first / 4;
}

Aggregation::Aggregation()
{
    currentTime = 0;
    remainTime = 0;
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

Entry::Entry()
{
    x = 0.0;
    y = 0.0;
    r = 0.0;
}

ValueFunction::ValueFunction()
{
    lookupTable = LookupTable();
}

double ValueFunction::getValue(Aggregation postDecisionState, double reward)
{
    return this->lookupTable.lookup(postDecisionState);
}

void ValueFunction::updateValue(vector<pair<Aggregation, double> > valueAtThisSimulation)
{
    for (auto decisionPoint = valueAtThisSimulation.begin(); decisionPoint != valueAtThisSimulation.end(); ++decisionPoint)
    {
        for (auto tableIter = this->lookupTable.value.begin(); tableIter != this->lookupTable.value.end(); ++tableIter)
        {
            if ((decisionPoint->first.currentTime >= tableIter->first.x - tableIter->first.r &&
                 decisionPoint->first.currentTime < tableIter->first.x + tableIter->first.r) &&
                (decisionPoint->first.remainTime >= tableIter->first.y - tableIter->first.r &&
                 decisionPoint->first.remainTime < tableIter->first.y + tableIter->first.r))
            {
                this->lookupTable.tableInfo[tableIter->first].second.push_back(decisionPoint->second);
                tableIter->second = (1 - STEPSIZE) * tableIter->second + STEPSIZE * decisionPoint->second;
                break;
            }
        }
    }
    this->lookupTable.partitionUpdate();
}