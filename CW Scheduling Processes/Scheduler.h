#pragma once
// Please write all your code in this file. This is an individual coursework, and solutions must not be shared by students.
#include <iostream>
#include "Parameters.h"
#include "Task.h"
#include <list>
#include <thread>
#include "Processor.h"
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
	int completedTasks = 0;

public:
	void ScheduleTasksUntilEnd()  //In this function you will have to schedule all the tasks until completion of all of them.
	{
		// loops until all tasks have been completed
		while (completedTasks < NB_TASKS) 
		{
			for (int i = 0; i < NB_PROCESSORS; i++) 
			{
				if (!processors[i].IsBusy()) 
				{
					// assigns task to the processor if it is in a ready state
					// this causes issues in some instances, since IsReady is not thread safe
					if (tasks[currentTask].IsReady())
					{
						processors[i].LaunchTask(tasks[currentTask]);
					}
				} 
				currentTask = (currentTask + 1) % NB_TASKS;
			}
		}
	};


	//The function NotifyEndScheduling is called by the thread representing a processor when done with running a task. This may be of use in some implementations.
	void NotifyEndScheduling(int processor, int taskId, TaskState state)
	{
		// once a task terminates, it is added to the count of completed processes. 
		if (state == 2) {
			completedTasks += 1;
		}
	};

	//Complete these two functions. The functions should return your student id and your name.
	int StudentID() { return 851182; }
	std::string StudentName() { return "Neil Woodhouse"; }
};


