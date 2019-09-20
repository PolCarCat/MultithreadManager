#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{

	while (!exitFlag)
	{
		// TODO 3:
		// - Wait for new tasks to arrive
		// - Retrieve a task from scheduledTasks
		// - Execute it
		// - Insert it into finishedTasks

		std::unique_lock<std::mutex> lock(mtx);
		Task* task = nullptr;
		while (task == nullptr && !exitFlag)
		{
			if (scheduledTasks.empty())
			{
				event.wait(lock); // Wait for a new task to come
			}
			else
			{
				// Get new task
				task = scheduledTasks.front();
				scheduledTasks.pop();

				// Execute it
				task->execute();
				finishedTasks.push(task);
			}
		}
	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())
	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i] = std::thread(&ModuleTaskManager::threadMain, this);
	}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	
	std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty())
	{
		finishedTasks.front()->owner->onTaskFinished(finishedTasks.front());
		finishedTasks.pop();
	}

	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify all threads to finish and join them

	exitFlag = true;
	event.notify_all();

	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i].join();
	}

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
