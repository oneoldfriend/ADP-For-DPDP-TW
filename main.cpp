#include "solver.h"
#include "generator.h"


int main()
{
    Generator::instanceGenenrator(MAX_TRAINING_INSTANCE);
    return 0;
    Solver solver;
    solver.solve();
}