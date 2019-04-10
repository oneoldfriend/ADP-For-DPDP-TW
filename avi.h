#pragma once
#include "mdp.h"

#define MAXSIMULATION 10000
#define MAXVALUE 10000

class LookupTable
{
  public:
    map<pair<double, double>, double> value;
    double lookup(State S, Action a);
};

class AVI
{
  public:
    LookupTable lookupTable;
    void approximation();
};