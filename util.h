#pragma once
#include"route.h"
#include <math.h>
#include <vector>

using namespace std;

class Util
{
  public:
    static double standardDeviation(vector<double> sample);//!!!
    static double calcTravelTime(Position a, Position b);
};