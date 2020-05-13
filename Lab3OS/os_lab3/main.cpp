#include <iostream>
#include "Executor.h"

using namespace std;

int main()
{
    size_t queues = 3;
    size_t weightStart = 5;
    size_t weightEnd = 80;
    size_t intervalStart = 10;
    size_t intervalEnd = 100;
    Executor executor(queues, weightStart, weightEnd, intervalStart, intervalEnd);

    for (size_t interval = intervalEnd; interval >= intervalStart; interval -= 2)
    {
        executor.reset(interval);

        for (size_t i = 0; i < weightEnd*100; i++)
        {
            executor.processTick();
            if (interval == 30)
            {
                executor.saveData2();
            }
        }

        executor.saveData();
    }
}
