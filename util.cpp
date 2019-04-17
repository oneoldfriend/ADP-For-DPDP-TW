#include "util.h"

double Util::standardDeviation(vector<double> sample)
{
    return 0;
}

double Util::calcTravelTime(Position a, Position b)
{
    return sqrt(pow(a.x - b.x, 2) - pow(a.y - b.y, 2));
}
