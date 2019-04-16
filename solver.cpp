#include "solver.h"

void Solver::solve()
{
    ValueFunction valueFunction;

    //offline approximation
    AVI approximateValueIterate;
    approximateValueIterate.approximation(&valueFunction);

    //online solving
    ifstream testFile("testData.txt", ios::in);

    return;
}