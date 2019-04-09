#pragma once
#include "customer.h"
#include "solution.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class State
{
  public:
    PointOrder currentPosition;
    double currentTime;
    vector<string> unservicedCustomer;
    vector<string> newCustomers;
    State();
};

class Action
{
  public:
    map<string, bool> customerConfirmation;
    bool movement;
};

class MDP
{
    Solution solution;
    State currentState;
    void transition(State *S, Action a);
    double reward(State S, Action a);
    void observation(vector<string> *newCustomers);
};