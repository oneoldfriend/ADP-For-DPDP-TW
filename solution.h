#pragma once
#include "route.h"
#include "mdp.h"
#include <vector>
#define MAXVEHICLE 50

using namespace std;

class Solution
{
public:
  vector<Route> routes;
  double cost;
  bool greedyInsertion(Action a);
  void solutionCopy(Solution *source);
  void solutionDelete();
  double calcCost();
  Solution();
};