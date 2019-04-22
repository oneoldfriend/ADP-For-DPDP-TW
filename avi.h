#pragma once
#include "mdp.h"
#include "vfa.h"
#include <math.h>
#include <fstream>

#define MAXWORKTIME 720.0
#define UNITTIME 1
#define CAPACITY 10
#define PENALTYFACTOR 5
#define MAXCOST 999999.0
#define MAXEDGE 100.0
#define CUSTOMERNUMBER 10
#define MAXVEHICLE 4
#define MAXSIMULATION 10

class AVI
{
public:
  void approximation(ValueFunction *valueFunction);
};