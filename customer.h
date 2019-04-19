#pragma once
#include <string>
#define MAXWORKTIME 720.0
#define UNITTIME 1
#define CAPACITY 10
#define PENALTYFACTOR 5
#define MAXCOST 999999.0
#define MAXEDGE 100.0
#define CUSTOMERNUMBER 10

using namespace std;

class Position
{
public:
  double x, y;
  Position()
  {
    x = 0;
    y = 0;
  };
};

class Customer
{
public:
  Position origin, dest;
  double startTime, endTime, priority, weight;
  string id;
  Customer();
};