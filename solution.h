#pragma once
#include <vector>

using namespace std;

class Action;
class Route;

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