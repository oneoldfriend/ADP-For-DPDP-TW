#pragma once
#include "customer.h"
#define MAXWORKTIME 720

typedef class Order *PointOrder;
typedef class Order *PointOrder;

class Order
{
public:
  double arrivalTime, waitTime, departureTime, currentWeight;
  bool isOrigin;
  Customer *customer;
  Position position;
  PointOrder prior;
  PointOrder next;
};
class Route
{
public:
  PointOrder head;
  PointOrder tail;
  double cost;
  void creatPartialRoute(PointOrder currentPosition);
  static void greedyInsertion(PointOrder currentPosition, map<string, bool> customerConfirmation);
};
