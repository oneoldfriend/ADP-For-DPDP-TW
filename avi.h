#pragma once
#include "mdp.h"
#include "vfa.h"
#include <math.h>
#include <fstream>

#define MAXSIMULATION 10
#define MAXVALUE 10000

class AVI
{
public:
  void approximation(ValueFunction *valueFunction);
};