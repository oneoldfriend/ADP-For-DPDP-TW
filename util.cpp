#include "util.h"

double Util::standardDeviation(vector<double> sample)
{
    double sum = 0, mean = 0, variance = 0;
    for (auto iter = sample.begin(); iter != sample.end();++iter){
        sum += *iter;
    }
    mean = sum / sample.size();
    for(auto iter = sample.begin(); iter != sample.end();++iter){
        variance += pow(*iter - mean, 2);
    }
    variance = variance / (sample.size() - 1);
    return sqrt(variance);
}

double Util::calcTravelTime(Position a, Position b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
