#include"vfa.h"





double LookupTable::lookup(Position postDecisionState){

}

double ValueFunction::getValue(State S, Action a, double reward){
    Position postDecisionState;
    this->aggregation(S, a, &postDecisionState);
}