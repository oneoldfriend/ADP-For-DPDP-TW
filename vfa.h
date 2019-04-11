#pragma once
#include "mdp.h"
#include"customer.h"
#include <vector>

class LookupTable
{
  public:
    map<pair<Position, double>, double> value;
    map<pair<Position, double>, pair<int, vector<double>>> tableInfo;
    double lookup(Position postDecisionState);
    void infoUpdate();
    void partitionUpdate();
};
class ValueFunction
{
  public:
    LookupTable lookupTable;
    double getValue(State S, Action a, double reward);
    void aggregation(State S, Action a, Position *postDecisionState);
};