#pragma once
#include "customer.h"
#include "util.h"
#include "mdp.h"
#define MAXWORKTIME 720
#define UNITTIME 1
#define CAPACITY 10
#define MAXCOST 999999

typedef class Order *PointOrder;
typedef class Order *PointOrder;
typedef class Route *PointRoute;

class Order
{
public:
  double arrivalTime, waitTime, departureTime, currentWeight;
  bool isOrigin;
  Customer *customer;
  Position position;
  PointOrder prior;
  PointOrder next;
  void infoCopy(PointOrder source);
  Order(Customer *customer, bool isOrigin);
};
class Route
{
public:
  Customer depot;
  PointOrder head;
  PointOrder tail;
  double cost;
  PointOrder currentPos;
  void routeUpdate();
  void insertOrder(PointOrder p);
  void removeOrder(PointOrder p);
  void creatPartialRoute(PointOrder currentPosition);
  bool greedyInsertion(Action a);
  bool checkFeasibility();
  void deleteRoute();
  double calcCost();
  Route(bool createPartial);
};
