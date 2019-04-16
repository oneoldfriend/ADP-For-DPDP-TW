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
    map<Customer*, bool> customerConfirmation;
    bool movement;
};

class State
{
  public:
    PointRoute currentRoute;
    double currentTime;
    vector<string> notServicedCustomer;
    vector<string> newCustomers;
    State();
    void getAction(int actionNum, Action *a);
    bool checkActionFeasibility(Action a);
};

class MDP
{
  public:
    Solution solution;
    State currentState;
    map<string, Customer> customers;
    void transition(Action a);//!!!
    double reward(State S, Action a);
    void observation();//!!!
    MDP();
};