#include"util.h"




double Util::calcTravelTime(Position a, Position b){
    return sqrt(pow(a.x - b.x, 2) - pow(a.y - b.y, 2));
}
