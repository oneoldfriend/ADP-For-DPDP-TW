#include "vfa.h"

LookupTable::LookupTable()
{
    double initialValue = - MAXEDGE * double(CUSTOMERNUMBER);
    double xTick = double(MAXWORKTIME) / double(LOOKUPTABLEINITIAL),
    yTick = double(MAXWORKTIME) * double(MAXVEHICLE) / double(LOOKUPTABLEINITIAL);
    for (int xCount = 0; xCount < LOOKUPTABLEINITIAL; xCount++)
    {
        for (int yCount = 0; yCount < LOOKUPTABLEINITIAL; yCount++)
        {
            Entry newEntry;
            newEntry.x = xTick / 2.0 + double(xCount) * xTick;
            newEntry.y = yTick / 2.0 + double(yCount) * yTick;
            newEntry.xRange = xTick;
            newEntry.yRange = yTick;
            this->value[newEntry] = initialValue;
        }
    }
}

double LookupTable::lookup(Aggregation postDecisionState)
{
    //找到aggregation 在lookup table 中对应的entry，并返回其value
    for (auto iter = this->value.begin(); iter != this->value.end(); ++iter)
    {
        if ((postDecisionState.currentTime >= iter->first.x - iter->first.xRange &&
             postDecisionState.currentTime < iter->first.x + iter->first.xRange) &&
            (postDecisionState.remainTime >= iter->first.y - iter->first.yRange &&
             postDecisionState.remainTime < iter->first.y + iter->first.yRange))
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
    //计算\hat{N}和\hat{theta}
    double averageN = totalN / entryNum, averageTheta = totalTheta / entryNum;
    auto tableIter = this->value.begin();
    for (int count = 0; count < entryNum; count++)
    {
        //计算N/\hat{N}和theta/\hat{theta}
        double factor1 = this->tableInfo[tableIter->first].first / averageN;
        double factor2 = entryTheta[tableIter->first] / averageTheta;
        if (factor1 * factor2 > PARTITIONTHRESHOLD)
        {
            //若该entry 达到threshold，则对entry 进行再划分
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
    //将当前entry 再划分为4个entry，并继承相关信息
    Entry partition1, partition2, partition3, partition4;
    partition1.x = tableIter->first.x + tableIter->first.xRange / 2.0;
    partition1.y = tableIter->first.y + tableIter->first.yRange / 2.0;
    partition2.x = tableIter->first.x + tableIter->first.xRange / 2.0;
    partition2.y = tableIter->first.y - tableIter->first.yRange / 2.0;
    partition3.x = tableIter->first.x - tableIter->first.xRange / 2.0;
    partition3.y = tableIter->first.y - tableIter->first.yRange / 2.0;
    partition4.x = tableIter->first.x - tableIter->first.xRange / 2.0;
    partition4.y = tableIter->first.y + tableIter->first.yRange / 2.0;
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
    //对执行动作后的解进行相关的信息提取
    this->currentTime = S.currentTime;
    this->remainTime = 0;
    //计算每条路径的剩余可规划时间
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
    xRange = 0.0;
    yRange = 0.0;
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
            //对这次simulation 所查询过的entry 对应的value 进行更新
            if ((decisionPoint->first.currentTime >= tableIter->first.x - tableIter->first.xRange &&
                 decisionPoint->first.currentTime < tableIter->first.x + tableIter->first.xRange) &&
                (decisionPoint->first.remainTime >= tableIter->first.y - tableIter->first.yRange &&
                 decisionPoint->first.remainTime < tableIter->first.y + tableIter->first.yRange))
            {
                //记录该entry 的相关信息（被查找次数和更新的value）
                this->lookupTable.tableInfo[tableIter->first].first++;
                this->lookupTable.tableInfo[tableIter->first].second.push_back(decisionPoint->second);
                //更新value
                tableIter->second = (1 - STEPSIZE) * tableIter->second + STEPSIZE * decisionPoint->second;
                break;
            }
        }
    }
    this->lookupTable.partitionUpdate();
}