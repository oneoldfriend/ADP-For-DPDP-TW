#pragma once
#include "route.h"
#include <vector>
#define MAXVEHICLE 50

using namespace std;

class Solution
{
  public:
    vector<Route> routes;
    double cost;
    void initialConstruction(vector<Customer*> initialCustomers);//!!!
    Solution();
}