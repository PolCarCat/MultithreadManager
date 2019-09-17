#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{
	Task* task = nullptr;

	while (true)
	{
		// TODO 3:
		// - Wait for new tasks to arrive
		// - Retrieve a task from scheduledTasks
		// - Execute it
		// - Insert it into finishedTasks

		std::unique_lock<std::mutex> lock(mtx);

		while (task == nullptr)
		{
			if (scheduledTasks.empty())
			{
				event.wait(lock);
			}
			else
			{
				task = scheduledTasks.front();
				scheduledTasks.pop();
			}
		}

		task->execute();
		finishedTasks.push(task);
	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())
	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i] = std::thread(&ModuleTaskManager::threadMain, this); // What is wrong with this?
	}

	//for (int i = 0; i < MAX_THREADS; ++i) // Not sure if necessary
	//{
	//	threads[i].join();
	//}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	
	std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty())
	{
		Module::onTaskFinished(finishedTasks.front());
		finishedTasks.pop();
	}

	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify all threads to finish and join them

	exitFlag = true;
	event.notify_all();

	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	std::unique_lock<std::mutex> lock(mtx);
	scheduledTasks.push(task);
	event.notify_one();
}
