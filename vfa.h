#pragma once
#include "mdp.h"
#include <vector>

class LookupTable
{
  public:
    map<pair<double, double>, double> value;
    double lookup(State S, Action a);
};
class ValueFunction
{
  public:
    LookupTable lookupTable;
    double getValue(State S, Action a, double reward);
};