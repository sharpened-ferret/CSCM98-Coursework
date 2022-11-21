#pragma once
// Please write all your code in this file. This is an individual coursework, and solutions must not be shared by students.
#include <iostream>
#include "Parameters.h"
#include "Task.h"
#include <list>
#include <thread>
#include "Processor.h"
#include <Queue>
using namespace std;

//The scheduler class should implement scheduling operations required to allow smooth scheduling of all the tasks
class Task;
class Processor;
class Scheduler
{
private:
	Task tasks[NB_TASKS]; //Storing a list of tasks
	Processor processors[NB_PROCESSORS]; //Simulating running processors
private:
	void CreateEnvironment();  //This function should not be modified. Definition is put inside the equivalent cpp file.
public:
	Scheduler(); //This function should not be modified. Definition is put inside the equivalent cpp file.
	~Scheduler();  //This function should not be modified. Definition is put inside the equivalent cpp file.

// own code from here
private:
	int currentTask = 0;
	atomic<int> completedTasks = 0;
	int queuedTasks = 0;
	queue<Task*> taskQueue;
	mutex queueMutex;
	mutex completedMutex;

public:
	void ScheduleTasksUntilEnd()  //In this function you will have to schedule all the tasks until completion of all of them.
	{
		while (queuedTasks < NB_TASKS)
		{
			for (int i = 0; i < NB_TASKS; i++)
			{
				taskQueue.push(&tasks[i]);
				queuedTasks++;
			}
		}
		/*while (taskQueue.size() > 0)
		{
			cout << taskQueue.front()->IsReady();
			taskQueue.pop();
		}*/
		for (int i = 0; i < NB_PROCESSORS; i++)
		{
			queueMutex.lock();
			processors[i].LaunchTask(*taskQueue.front());
			taskQueue.pop();
			queueMutex.unlock();
		}
		while (completedTasks < NB_TASKS)
		{
			
		}
		// loops until all tasks have been completed
		//while (completedTasks < NB_TASKS) 
		//{
		//	for (int i = 0; i < NB_PROCESSORS; i++) 
		//	{
		//		if (!processors[i].IsBusy()) 
		//		{
		//			// assigns task to the processor if it is in a ready state
		//			// this causes issues in some instances, since IsReady is not thread safe
		//			if (tasks[currentTask].IsReady())
		//			{
		//				processors[i].LaunchTask(tasks[currentTask]);
		//			}
		//		} 
		//		currentTask = (currentTask + 1) % NB_TASKS;
		//	}
		//}
	};


	//The function NotifyEndScheduling is called by the thread representing a processor when done with running a task. This may be of use in some implementations.
	void NotifyEndScheduling(int processor, int taskId, TaskState state)
	{
		
		// once a task terminates, it is added to the count of completed processes. 
		/*if (state == 2) {
			completedTasks += 1;
		}*/
		switch (state)
		{
		case notStarted:
			cout << "not started";
			break;
		case ready:
			queueMutex.lock();
			taskQueue.push(&tasks[taskId]);
			processors[processor].LaunchTask(*taskQueue.front());
			taskQueue.pop();
			queueMutex.unlock();
			break;
		case terminated:
			completedTasks.fetch_add(1);
			cout << "\nTask Completed! Total: " << completedTasks;
			queueMutex.lock();
			processors[processor].LaunchTask(*taskQueue.front());
			taskQueue.pop();
			queueMutex.unlock();
			break;
		case running:
			cout << "Running";
			break;
		case waitingIOCompletion:
			cout << "\nWaiting IO";
			queueMutex.lock();
			processors[processor].LaunchTask(*taskQueue.front());
			taskQueue.pop();
			queueMutex.unlock();
			while (!tasks[taskId].IsReady())
			{
			}
			queueMutex.lock();
			taskQueue.push(&tasks[taskId]);
			queueMutex.unlock();
			break;
		case done:
			cout << "done";
			break;
		default:
			break;
		}
	};

	void CallNextTask(int processor)
	{
		while (true) {
			if (taskQueue.size() > 0)
			{
				queueMutex.lock();
				processors[processor].LaunchTask(*taskQueue.front());
				taskQueue.pop();
				queueMutex.unlock();
				return;
			}
		}
	}

	//Complete these two functions. The functions should return your student id and your name.
	int StudentID() { return 851182; }
	std::string StudentName() { return "Neil Woodhouse"; }
};


