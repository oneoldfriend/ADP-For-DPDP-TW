#include "solver.h"
#include "generator.h"


int main()
{
    Generator::instanceGenenrator(1);
    Solver solver;
    solver.solve();
}