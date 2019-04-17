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

Route::Route(bool creatPartial)
{
    this->cost = 0;
    if (creatPartial)
    {
        this->head = nullptr;
        this->tail = nullptr;
        this->currentPos = nullptr;
    }
    else
    {
        this->head = new Order(&(this->depot), true);
        this->tail = new Order(&(this->depot), true);
        this->head->next = this->tail;
        this->tail->prior = this->head;
        this->currentPos = this->head;
    }
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
    this->currentPos = targetHead;
    this->head = targetHead;
    this->tail = pre;
}

bool Route::findBestPosition(PointOrder origin, PointOrder dest, pair<PointOrder, PointOrder> *bestOriginPos, pair<PointOrder, PointOrder> *bestDestPos, double *bestCost)
{
    double oldCost = this->cost;
    PointOrder originPos = this->currentPos;
    bool feasibilityExist = false;
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
            this->routeUpdate;
            if (this->checkFeasibility())
            {
                feasibilityExist = true;
                if (oldCost - this->cost < *bestCost)
                {
                    *bestCost = oldCost - this->cost;
                    bestOriginPos->first = origin->prior;
                    bestOriginPos->second = origin->next;
                    bestDestPos->first = dest->prior;
                    bestDestPos->second = dest->next;
                }
            }
            this->removeOrder(dest);
            destPos = destPos->next;
        }
        this->removeOrder(origin);
        originPos = originPos->next;
    }
    this->routeUpdate();
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