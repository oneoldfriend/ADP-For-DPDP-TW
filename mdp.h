#pragma once
#include "customer.h"
#include "solution.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class Action
{
  public:
    map<string, bool> customerConfirmation;
    bool movement;
};

class State
{
  public:
    PointOrder currentPosition;
    double currentTime;
    vector<string> unservicedCustomer;
    vector<string> newCustomers;
    State();
    void getAction(int actionNum, Action *a);
    void getPostDecisionState(Action a);
    bool checkActionFeasibility(Action a);
};

class MDP
{
  public:
    Solution solution;
    State currentState;
    void transition(State *S, Action a);
    double reward(State S, Action a);
    void observation(vector<string> *newCustomers);
    void initialize();
};