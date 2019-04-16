#pragma once
#include "customer.h"
#include "solution.h"
#include <map>
#include <list>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Action
{
public:
  map<Customer *, bool> customerConfirmation;
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
  list<pair<double, Customer *> > sequenceData;
  map<string, Customer*> customers;
  void transition(Action a); //!!!
  double reward(State S, Action a);
  void observation(double lastDecisionTime); //!!!
  MDP(int trainDayNum);
};