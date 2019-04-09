#include"customer.h"
#include<map>
#include<string>
#include<vector>

using namespace std;


class State
{
    Position currentPosition;
    double currentTime;
    map<string, double> customerPriority;
    vector<Customers> newCustomers;
    State();
};

class Action{
    map<string, bool> customerConfirm;
    bool movement;
};

class MDP {
    State currentState;
    State transition(State S, Action a);
    double reward(State S, Action a);
}