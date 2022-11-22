#pragma once
// Please write all your code in this file. This is an individual coursework, and solutions must not be shared by students.
#include <iostream>
#include "Parameters.h"
#include "Task.h"
#include <list>
#include <thread>
#include "Processor.h"

// NOTE :: I added these includes and namespace to this file
#include <Queue>
#include <chrono>
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
	atomic<int> completedTasks = 0;
	int queuedTasks = 0;
	queue<Task*> taskQueue;
	mutex queueMutex;
	mutex waitMutex;
	condition_variable waitAlert;

	/*
	 Helper function to handle calling the next queued task on a specified processor
	*/
	void CallNextTask(int processor)
	{
		while (true) {
			// This mutex block slows execution a fair bit
			// It protects against dequeues on an empty taskQueue 
			queueMutex.lock();
			if (taskQueue.size() > 0)
			{
				processors[processor].LaunchTask(*taskQueue.front());
				taskQueue.pop();
				queueMutex.unlock();
				return;
			}
			queueMutex.unlock();
		}
	}

public:
	void ScheduleTasksUntilEnd()  //In this function you will have to schedule all the tasks until completion of all of them.
	{
		// Adds the task list to a queue
		while (queuedTasks < NB_TASKS)
		{
			for (int i = 0; i < NB_TASKS; i++)
			{
				taskQueue.push(&tasks[i]);
				queuedTasks++;
			}
		}
		// Starts Initial Tasks on all processors
		for (int i = 0; i < NB_PROCESSORS; i++)
		{
			CallNextTask(i);
		}

		//Uses a mutex lock and conditional variable to notify the thread to wake from wait state
		unique_lock<mutex> waitLock(waitMutex);
		waitAlert.wait_for(waitLock, 10000000s);


		/*while (completedTasks < NB_TASKS)
		{
			
		}*/
	};


	//The function NotifyEndScheduling is called by the thread representing a processor when done with running a task. This may be of use in some implementations.
	void NotifyEndScheduling(int processor, int taskId, TaskState state)
	{
		switch (state)
		{
		// Requeues the task and executes next task on the queue
		case ready:
			queueMutex.lock();
			taskQueue.push(&tasks[taskId]);
			queueMutex.unlock();
			CallNextTask(processor);
			break;
		// Updates the number of completed tasks and executes the next task
		// If all tasks have been completed, sends a notify_all to resume the waiting ScheduleTasksUntilEnd thread
		case terminated:
			completedTasks.fetch_add(1);
			if (completedTasks == NB_TASKS) {
				waitAlert.notify_all();
			}
			CallNextTask(processor);
			break;
		// Handles requeuing tasks waiting for IO
		case waitingIOCompletion:
			// If there are other tasks for processors to execute
			// the program continues executing tasks, with the IO task returning to a ready state in the background
			if (taskQueue.size() >= NB_PROCESSORS)
			{
				CallNextTask(processor);

				while (!tasks[taskId].IsReady())
				{
				}
				queueMutex.lock();
				taskQueue.push(&tasks[taskId]);
				queueMutex.unlock();
			}
			// If there are no other tasks to execute, the processors wait for the tasks to return to a ready state
			else
			{
				while (!tasks[taskId].IsReady())
				{
				}
				queueMutex.lock();
				taskQueue.push(&tasks[taskId]);
				queueMutex.unlock();
				CallNextTask(processor);
			}
			break;
		case done:
			/*
			This status didn't seem to be used in the provided code.
			I assumed it would be roughly equivalent to a task terminating,
			so handled it the same way.
			*/
			completedTasks.fetch_add(1);
			if (completedTasks == NB_TASKS) {
				waitAlert.notify_all();
			}
			CallNextTask(processor);
			break;
		default:
			// If an unexpected status is recieved, the processor moves to the next task
			CallNextTask(processor);
			break;
		}
	};

	

	//Complete these two functions. The functions should return your student id and your name.
	int StudentID() { return 851182; }
	std::string StudentName() { return "Neil Woodhouse"; }
};


