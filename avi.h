#pragma once
#include "mdp.h"
#include"vfa.h"

#define MAXSIMULATION 10000
#define MAXVALUE 10000


class AVI
{
  public:
    void approximation(ValueFunction *valueFunction);
};