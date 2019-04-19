#include "route.h"

Order::Order(Customer *customer, bool isOrigin)
{
    this->arrivalTime = 0;
    this->currentWeight = 0;
    this->customer = customer;
    this->departureTime = 0;
    this->isOrigin = isOrigin;
    this->prior = nullptr;
    this->next = nullptr;
    if (this->isOrigin)
    {
        this->position = customer->origin;
    }
    else
    {
        this->position = customer->dest;
    }
    this->waitTime = 0;
}

void Order::infoCopy(PointOrder source)
{
    this->arrivalTime = source->arrivalTime;
    this->currentWeight = source->currentWeight;
    this->customer = source->customer;
    this->departureTime = source->departureTime;
    this->isOrigin = source->isOrigin;
    this->position = source->position;
    this->waitTime = source->waitTime;
}

Route::Route()
{
    this->depot = new Customer();
    this->cost = 0;
    this->head = new Order(this->depot, true);
    this->tail = new Order(this->depot, true);
    this->head->next = this->tail;
    this->tail->prior = this->head;
    this->currentPos = this->head;
}

void Route::routeUpdate()
{
    PointOrder p = this->currentPos->next;
    while (p != nullptr)
    {
        p->arrivalTime = p->prior->departureTime + Util::calcTravelTime(p->prior->position, p->position);
        if (p->arrivalTime < p->customer->startTime)
        {
            p->waitTime = p->customer->startTime - p->arrivalTime;
            p->departureTime = p->customer->startTime;
        }
        else
        {
            p->waitTime = 0;
            p->departureTime = p->arrivalTime;
        }
        if (p->isOrigin)
        {
            p->currentWeight = p->prior->currentWeight + p->customer->weight;
        }
        else
        {
            p->currentWeight = p->prior->currentWeight - p->customer->weight;
        }
        p = p->next;
    }
    this->cost = this->calcCost();
}

void Route::removeOrder(PointOrder p)
{
    if (p->prior != nullptr)
    {
        p->prior->next = p->next;
    }
    else
    {
        this->head = p->next;
    }
    if (p->next != nullptr)
    {
        p->next->prior = p->prior;
    }
    else
    {
        this->tail = p->prior;
    }
}

void Route::insertOrder(PointOrder p)
{
    if (p->prior != nullptr)
    {
        p->prior->next = p;
    }
    else
    {
        this->head = p;
    }
    if (p->next != nullptr)
    {
        p->next->prior = p;
    }
    else
    {
        this->tail = p;
    }
}

void Route::creatPartialRoute(PointOrder currentPosition)
{
    PointOrder p = currentPosition;
    PointOrder targetHead = nullptr, pre = nullptr;
    while (p != nullptr)
    {
        PointOrder order = new Order(p->customer, p->isOrigin);
        order->infoCopy(p);
        if (targetHead == nullptr)
            targetHead = order;
        else
            pre->next = order;
        order->prior = pre;
        pre = order;
        p = p->next;
    }
    this->currentPos = targetHead;
    this->head = targetHead;
    this->tail = pre;
}

void Route::routeCopy(Route source)
{
    delete this->head;
    delete this->tail;
    this->head = nullptr;
    this->tail = nullptr;
    PointOrder p = source.head;
    PointOrder targetHead = nullptr, pre = nullptr;
    while (p != nullptr)
    {
        PointOrder order = new Order(p->customer, p->isOrigin);
        order->infoCopy(p);
        if (targetHead == nullptr)
            targetHead = order;
        else
            pre->next = order;
        order->prior = pre;
        pre = order;
        p = p->next;
        if (source.currentPos == p)
        {
            this->currentPos = order;
        }
    }
    this->head = targetHead;
    this->tail = pre;
    this->cost = source.cost;
}

bool Route::findBestPosition(PointOrder origin, PointOrder dest, double *bestCost)
{
    double oldCost = this->cost;
    PointOrder originPos = this->currentPos;
    bool feasibilityExist = false;
    pair<PointOrder, PointOrder> bestOriginPos, bestDestPos;
    while (originPos != tail)
    {
        origin->prior = originPos;
        origin->next = originPos->next;
        this->insertOrder(origin);
        PointOrder destPos = origin;
        while (destPos != tail)
        {
            dest->prior = destPos;
            dest->next = destPos->next;
            this->insertOrder(dest);
            this->routeUpdate();
            if (this->checkFeasibility())
            {
                feasibilityExist = true;
                if (this->cost - oldCost < *bestCost)
                {
                    *bestCost = this->cost - oldCost;
                    bestOriginPos.first = origin->prior;
                    bestOriginPos.second = origin->next;
                    bestDestPos.first = dest->prior;
                    bestDestPos.second = dest->next;
                }
            }
            this->removeOrder(dest);
            destPos = destPos->next;
        }
        this->removeOrder(origin);
        originPos = originPos->next;
    }
    this->routeUpdate();
    origin->prior = bestOriginPos.first;
    origin->next = bestOriginPos.second;
    dest->prior = bestDestPos.first;
    dest->next = bestDestPos.second;
    return feasibilityExist;
}

bool Route::checkFeasibility()
{
    if (this->tail->departureTime > MAXWORKTIME)
    {
        return false;
    }
    PointOrder p = this->currentPos;
    while (p != nullptr)
    {
        if (p->currentWeight > CAPACITY)
        {
            return false;
        }
        p = p->next;
    }
    return true;
}

void Route::deleteRoute()
{
    PointOrder p = this->head;
    if (p == nullptr)
    {
        return;
    }
    while (p != nullptr)
    {
        PointOrder temp = p->next;
        delete p;
        p = temp;
    }
    delete this->depot;
}

double Route::calcCost()
{
    double penalty = 0, travelTime = 0, waitTime = 0;
    PointOrder p = this->head;
    while (p != this->tail)
    {
        if (!p->isOrigin)
        {
            if (p->arrivalTime > p->customer->endTime)
            {
                penalty += p->customer->priority * PENALTYFACTOR * (p->arrivalTime - p->customer->endTime);
            }
            else
            {
                waitTime += p->waitTime;
            }
        }
        if (p->next != nullptr)
        {
            travelTime += Util::calcTravelTime(p->position, p->next->position);
        }
        p = p->next;
    }
    return penalty + travelTime + waitTime;
}