#include "avi.h"
#include "mdp.h"
#include <math.h>

void AVI::approximation()
{
    int count = 0;
    while (count++ < MAXSIMULATION)
    {
        MDP simulation;
        simulation.initialize();
        while (!simulation.currentState.unservicedCustomer.empty())
        {
            int actionNum = 0, maxActionNum = pow(2, simulation.currentState.newCustomers.size() + 1), bestActionNum = -1;
            double bestActionValue = MAXVALUE;
            while (actionNum < maxActionNum)
            {
                Action a;
                double actionValue = 0;
                simulation.currentState.getAction(actionNum, &a);
                if (simulation.currentState.checkActionFeasibility(a))
                {
                    actionValue = simulation.reward(simulation.currentState, a) + this->lookupTable.lookup(simulation.currentState, a);
                    if (actionValue < bestActionValue)
                    {
                        bestActionNum = actionNum;
                    }
                }
                actionNum++;


                //!!!!!!need to record the partition information for dynamic lookup table


            }
        }
    }
}