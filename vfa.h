#pragma once
#include "mdp.h"
#include "customer.h"
#include "route.h"
#include "util.h"
#include <vector>
#define STEPSIZE 0.1
#define PARTITIONTHRESHOLD 0.5

class Aggregation
{
  public:
    double currentTime, remainTime;
    void aggregate(State S, Action a);
    Aggregation();
};
class LookupTable
{
  public:
    map<pair<Aggregation, double>, double> value;
    map<pair<Aggregation, double>, pair<int, vector<double>>> tableInfo;
    double lookup(Aggregation postDecisionState);
    void infoUpdate();
    void partitionUpdate(vector<pair<Aggregation, double>> valueAtThisSimulation);
    void partition(map<pair<Aggregation, double>, double>::iterator tableIter);
    LookupTable();
};
class ValueFunction
{
  public:
    LookupTable lookupTable;
    double getValue(Aggregation postDecisionState, double reward);
    void updateValue(vector<pair<Aggregation, double>> valueAtThisSimulation);
    ValueFunction();
};