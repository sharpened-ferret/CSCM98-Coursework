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
			// This mutex block protects against dequeues on an empty taskQueue 
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

	/*
	 Thread function to wait for a task's IO operation to complete then requeue it
	*/
	void WaitOnIO(int taskId) 
	{
		while (!tasks[taskId].IsReady()) {}
		queueMutex.lock();
		taskQueue.push(&tasks[taskId]);
		queueMutex.unlock();
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

		/*
		This waits the thread until it recieves a signal (when all tasks have completed), or it times out (after one hour).
		Ideally, we would use a sleep function with interrupts to achieve this, to reduce CPU usage and improve power efficiency. 

		However, this seems to require platform-specific functionality, e.g. using sleep from <Windows.h> for Windows systems, or usleep from "unistd.h"
		for POSIX-based systems like Linux. 
		I've used wait here to maintain platform indepent functionality.
		*/
		unique_lock<mutex> waitLock(waitMutex);
		waitAlert.wait_for(waitLock, 3600s);
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
		// If all tasks have been completed, sends a notify_all signal to resume the waiting ScheduleTasksUntilEnd thread
		case terminated:
			completedTasks.fetch_add(1);
			if (completedTasks == NB_TASKS) {
				waitAlert.notify_all();
			}
			CallNextTask(processor);
			break;
		// Handles requeuing tasks waiting for IO
		case waitingIOCompletion:
			// Creates an independent thread to requeue the task once IO is complete
			// Starts the next task on the current processor
			{
				thread IoUpdate(&Scheduler::WaitOnIO, this, taskId);
				IoUpdate.detach();
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
			// If an unexpected status is recieved, an error message is displayed and the processor moves to the next task
			cout << "\nUnexpected Task Status: Task " << taskId << " skipped.\n";
			completedTasks.fetch_add(1);
			if (completedTasks == NB_TASKS) {
				waitAlert.notify_all();
			}
			CallNextTask(processor);
			break;
		}
	};

	

	//Complete these two functions. The functions should return your student id and your name.
	int StudentID() { return 851182; }
	std::string StudentName() { return "Neil Woodhouse"; }
};


