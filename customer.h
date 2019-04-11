#pragma once

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
    double startTime, endTime, Priority, weight;
    string id;
}