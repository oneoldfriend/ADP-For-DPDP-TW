#pragma once
#include "customer.h"
#include "solution.h"
#include <map>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#define TIMEUNIT 10

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
  Solution *pointSolution;
  double currentTime;
  vector<string> notServicedCustomer;
  vector<string> newCustomers;
  State();
};

class MDP
{
public:
  Solution solution;
  State currentState;
  list<pair<double, Customer *> > sequenceData;
  map<string, Customer*> customers;
  bool checkActionFeasibility(Action a);
  void getAction(int actionNum, State S, Action *a);
  void transition(Action a);
  double reward(State S, Action a);
  double rejectionReward(Action a);
  void observation(double lastDecisionTime);
  MDP(string fileName);
};