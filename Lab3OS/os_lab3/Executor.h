#pragma once
#include <list>
#include <vector>
#include <fstream>

struct Task
{
	size_t weight;
	size_t awaitTime;
	size_t timeLeft;
	size_t timeCreated;
	size_t timeFinished;
	size_t queueNum;
	Task(size_t weight = 0, size_t timeCreated = 0)
	{
		this->weight = weight;
		this->timeCreated = timeCreated;
		awaitTime = 0;
		timeLeft = weight;
		timeFinished = 0;
		queueNum = 0;
	}
};

class Executor
{
public:
	Executor(size_t queuesAm, size_t weightStart, size_t weightEnd, size_t intervalStart, size_t intervalEnd);
	~Executor();
	void processTick();
	void saveData();
	void saveData2();
	void reset(size_t intervalEnd);
	size_t currentTick;
private:
	std::vector<std::list<Task>> queues;
	std::vector<Task> doneTasks;
	void addTask();
	void processTask();
	void updateAwaitingTime();
	size_t queueAmount;
	size_t weightStart;
	size_t weightEnd;
	size_t intervalStart;
	size_t intervalEnd;
	size_t tickToAddTask;
	size_t taskAmount;
	size_t sleepAmount;
	bool processingTask;
	Task currentTask;
	size_t timeToProcessTask;
	std::ofstream file;
	std::ofstream file2;
};

