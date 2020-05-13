#include <ctime>
#include "Executor.h"
#include <iostream>

using namespace std;

Executor::Executor(size_t queueAmount, size_t weightStart, size_t weightEnd, size_t intervalStart, size_t intervalEnd)
{
	tickToAddTask = currentTick = 1;
	this->queueAmount = queueAmount;
	this->weightStart = weightStart;
	this->weightEnd = weightEnd;
	this->intervalStart = intervalStart;
	this->intervalEnd = intervalEnd;
	processingTask = false;
	taskAmount = sleepAmount = 0;
	queues = vector<list<Task>>(queueAmount);
	srand(time(0));
	file.open("data.csv", ios::out | ios::trunc);
	file2.open("taskAwaiting.csv", ios::out | ios::trunc);
}

Executor::~Executor()
{
	file.close();
	file2.close();
}

void Executor::reset(size_t interval)
{
	this->intervalEnd = this->intervalStart = interval;
	tickToAddTask = currentTick = 1;
	processingTask = false;
	taskAmount = sleepAmount = 0;
	queues = vector<list<Task>>(queueAmount);
	doneTasks = vector<Task>();
}

void Executor::processTick()
{
	if (tickToAddTask == currentTick)
	{
		addTask();
		tickToAddTask = currentTick + rand() % (intervalEnd - intervalStart + 1) + intervalStart;
	}

	if (processingTask)
	{
		processTask();
	}
	else
	{
		timeToProcessTask = -1;

		for (size_t i = 0; i < queues.size(); i++)
		{
			if (queues[i].size())
			{
				currentTask = queues[i].front();
				queues[i].pop_front();

				if (queues.size() - 1 == i || currentTask.timeLeft <= i + 1)
				{
					timeToProcessTask = currentTask.timeLeft;
				}
				else
				{
					timeToProcessTask = i + 1;
				}
				break;
			}
		}

		if (timeToProcessTask != -1)
		{
			processingTask = true;
			processTask();
		}
		else
		{
			sleepAmount++;
		}
	}
	
	currentTick++;
}

void Executor::addTask()
{
	size_t weight = rand() % (weightEnd - weightStart + 1) + weightStart;
	Task task(weight, currentTick);
	queues[0].push_back(task);
	taskAmount++;
}

void Executor::processTask()
{
	currentTask.timeLeft--;
	timeToProcessTask--;
	updateAwaitingTime();
	if (!currentTask.timeLeft)
	{
		timeToProcessTask = 0;
		processingTask = false;
		currentTask.timeFinished = currentTick;
		doneTasks.push_back(currentTask);
		taskAmount--;
	}
	else if (!timeToProcessTask)
	{
		processingTask = false;
		if (currentTask.queueNum != queues.size() - 1)
		{
			currentTask.queueNum++;
			queues[currentTask.queueNum].push_back(currentTask);
		}
	}
}

void Executor::updateAwaitingTime()
{
	for (size_t i = 0; i < queues.size(); i++)
	{
		for (Task &task : queues[i])
		{
			task.awaitTime++;
		}
	}
}

void Executor::saveData()
{
	double avAwTime = 0;
	for (size_t i = 0; i < doneTasks.size(); i++)
	{
		avAwTime += doneTasks[i].awaitTime;
	}
	for (size_t i = 0; i < queues.size(); i++)
	{
		for (Task task : queues[i])
		{
			avAwTime += task.awaitTime;
		}
	}

	avAwTime = avAwTime / (taskAmount + doneTasks.size());
	file << intervalStart << ';' << (long)avAwTime << ';' << (double)sleepAmount / currentTick * 100 << endl;
}

void Executor::saveData2()
{
	double avAwTime = 0;
	for (size_t i = 0; i < doneTasks.size(); i++)
	{
		avAwTime += doneTasks[i].awaitTime;
	}
	for (size_t i = 0; i < queues.size(); i++)
	{
		for (Task task : queues[i])
		{
			avAwTime += task.awaitTime;
		}
	}

	avAwTime = avAwTime / (taskAmount + doneTasks.size());
	file2 << taskAmount + doneTasks.size() << ';' << (long)avAwTime << endl;
}

//void Executor::saveData2()
//{
//	for (size_t i = 0; i < doneTasks.size(); i++)
//	{
//		file2 << doneTasks[i].awaitTime << '\n';
//	}
//	for (size_t i = 0; i < queues.size(); i++)
//	{
//		for (Task task : queues[i])
//		{
//			file2 << task.awaitTime << '\n';
//		}
//	}
//
//	file2.flush();
//}