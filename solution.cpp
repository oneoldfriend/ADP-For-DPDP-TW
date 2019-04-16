#include"solution.h"


Solution::Solution(){
    for (int i = 0; i < MAXVEHICLE;i++){
        Route route = Route(false);
        this->routes.push_back(route);
    }
    this->cost = 0;
}