#include"solver.h"
#include"avi.h"




void Solver::solve(){
    ValueFunction valueFunction;

    //offline approximation

    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);

    //online solving
}