#include "solver.h"
#include <random>
#include <ctime>

void infoCopy(Customer *target, Customer *source)
{
    target->dest.x = source->dest.x;
    target->dest.y = source->dest.y;
    target->origin.x = source->origin.x;
    target->origin.y = source->origin.y;
    target->startTime = source->startTime;
    target->endTime = source->endTime;
    target->weight = source->weight;
    target->priority = source->priority;
    target->id = source->id;
}

bool sortAscend(const pair<double, Customer *> a, const pair<double, Customer *> b)
{
    return a.first > b.first;
}

void instanceGenenrator(double trainDayNum)
{
    double shopLocation = 25.0, serviceRange = 20.0;
    default_random_engine e(time(0));
    uniform_real_distribution<double> ratio(0.0, 1.0);
    uniform_real_distribution<double> shopPosX(-shopLocation, shopLocation);
    uniform_real_distribution<double> shopPosY(-shopLocation, shopLocation);
    int customerNum = 20;
    double cancellationRatio = 0.1, hurryRatio = 0.4, timeWindowLength = (shopLocation + serviceRange), blankLength = 10.0, maxDemand = 5.0;
    int count = 0;
    while (count++ < trainDayNum)
    {
        list<pair<double, Customer *> > generatedCustomers;
        int customerCount = 0;
        while (customerCount++ < customerNum)
        {
            Customer *customer = new Customer();
            double appearTime = ratio(e) * (MAXWORKTIME - timeWindowLength - blankLength);
            customer->origin.x = shopPosX(e);
            customer->origin.y = shopPosY(e);
            uniform_real_distribution<double> customerPosX(customer->origin.x - serviceRange, customer->origin.x + serviceRange);
            uniform_real_distribution<double> customerPosy(customer->origin.y - serviceRange, customer->origin.y + serviceRange);
            customer->dest.x = customerPosX(e);
            customer->dest.y = customerPosy(e);
            customer->startTime = appearTime + blankLength;
            customer->endTime = customer->startTime + timeWindowLength;
            customer->weight = ratio(e) * maxDemand;
            char idString[] = {char(customerCount / 1000 + 48), char(customerCount % 1000 / 100 + 48),
                            char(customerCount % 100 / 10 + 48), char(customerCount % 10 + 48)};
            customer->id = idString;
            generatedCustomers.push_back(make_pair(appearTime, customer));
            double isCanceled = ratio(e), isHurry = ratio(e);
            if (isCanceled <= cancellationRatio)
            {
                Customer *cancel = new Customer();
                infoCopy(cancel, customer);
                cancel->priority = 0;
                double cancelTime = customer->startTime - blankLength + ratio(e) * blankLength;
                generatedCustomers.push_back(make_pair(cancelTime, cancel));
            }
            if (isHurry <= hurryRatio)
            {
                Customer *hurry = new Customer();
                infoCopy(hurry, customer);
                hurry->priority = 0;
                double hurryTime = customer->startTime + ratio(e) * timeWindowLength;
                generatedCustomers.push_back(make_pair(hurryTime, hurry));
            }
        }
        generatedCustomers.sort(sortAscend);
        char dayNum[] = {char(count / 1000 + 48), char(count % 1000 / 100 + 48),
                      char(count / 100 + 48), char(count % 100 / 10 + 48), char(count % 10 + 48)};
        string fileName = "TrainingData/";
        fileName = fileName + +dayNum + ".txt";
        ofstream outFile(fileName, ios::out);
        for (auto iter = generatedCustomers.begin(); iter != generatedCustomers.end();++iter){
            outFile << iter->first << " ";
            outFile << iter->second->id << " ";
            outFile << iter->second->origin.x << " ";
            outFile << iter->second->origin.y << " ";
            outFile << iter->second->dest.x << " ";
            outFile << iter->second->dest.y << " ";
            outFile << iter->second->startTime << " ";
            outFile << iter->second->endTime << " ";
            outFile << iter->second->weight << endl;
            delete iter->second;
        }
        outFile.close();
    }
}

int main()
{
    Solver solver;
    solver.solve();
}