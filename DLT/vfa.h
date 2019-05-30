#pragma once
#include "avi.h"
#include "customer.h"
#include "route.h"
#include "util.h"
#include <vector>
#define STEP_SIZE 0.5
#define PARTITION_THRESHOLD 1.0
#define LOOKUP_TABLE_INITIAL 10.0

class Aggregation
{
public:
  double currentTime, remainTime;
  void aggregate(State S, Action a);
  Aggregation();
};
class Entry
{
public:
  double x, y, xRange, yRange;
  Entry();
  bool operator<(const Entry &other) const
  {
    if (x < other.x)
    {
      return true;
    }
    else if (x == other.x)
    {
      if (y < other.y)
      {
        return true;
      }
    }
    return false;
  }
};

class LookupTable
{
public:
  map<Entry, double> value;
  map<Entry, pair<int, vector<double> > > tableInfo;
  double lookup(Aggregation postDecisionState);
  void partitionUpdate();
  void partition(map<Entry, double>::iterator tableIter);
  LookupTable();
};
class ValueFunction
{
public:
  LookupTable lookupTable;
  double getValue(Aggregation postDecisionState, double reward);
  void updateValue(vector<pair<Aggregation, double> > valueAtThisSimulation, bool startApproximate);
  ValueFunction();
};
