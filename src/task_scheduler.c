#include "task_scheduler.h"

// #include "fnv.h"
// #define HASH_FUNC FNV_Hash

#include "sdbm.h"
#define HASH_FUNC SDBM_Hash

#include <string.h>

void TaskScheduler_Init(TaskList* taskList, TimeStampCallback timeStampFunc)
{
	LinkedList_Init(&taskList->Tasks);
	taskList->timeStamp = timeStampFunc;
	taskList->NextTask	= NULL;
}

static void createNewTask(TaskList* list, Task* task, char* taskName, TaskTypes type, TaskCallback callback, void* data, uint32_t period)
{
	if (task)
	{
		LinkedList_LinkTail(&list->Tasks, &task->List);
		task->Name			= taskName ? HASH_FUNC(taskName, strlen(taskName)) : 0;
		task->Type			= type;
		task->Status		= ActiveTask;
		task->Period		= period;
		task->LastTimestamp = list->timeStamp() + period;
		task->Callback		= callback;
		task->Data			= data;
	}

	if (list->NextTask == NULL)
		list->NextTask = task;
}

void TaskScheduler_CreateRetriggerTask(TaskList* list, Task* task, char* name, TaskCallback callback, void* data, uint32_t period)
{
	createNewTask(list, task, name, RecurringTask, callback, data, period);
}

void TaskScheduler_CreateSingleShotTask(TaskList* list, Task* task, char* name, TaskCallback callback, void* data, uint32_t delay)
{
	createNewTask(list, task, name, SingleShotTask, callback, data, delay);
}

void TaskScheduler_ActivateTask(Task* task) { task->Status = ActiveTask; }

void TaskScheduler_DeactivateTask(Task* task) { task->Status = InactiveTask; }

void TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period) { task->Period = period; }

void TaskScheduler_ChangeTaskCallback(Task* task, TaskCallback callback, void* data)
{
	task->Callback = callback;
	task->Data	   = data;
}

Task* TaskScheduler_FindTask(TaskList* list, char* name)
{
	Task*	 task	  = (Task*)list->Tasks.Head;
	uint32_t taskName = name ? HASH_FUNC(name, strlen(name)) : 0;
	if (task == NULL)
		return NULL;

	do {
		if (task->Name == taskName)
			return task;
		task = (Task*)task->List.Next;
	} while (task != (Task*)list->Tasks.Head);

	// Task could not be found
	return NULL;
}

void TaskScheduler_RemoveTask(TaskList* list, Task* task)
{
	// The task being removed cannot be the next task to be checked.
	if (list->NextTask == task)
		list->NextTask = (Task*)task->List.Next;

	// If the task being removed is still the next task in line (the only task queued), clear the next task.
	if (list->NextTask == task)
		list->NextTask = NULL;

	LinkedList_UnlinkNode(&list->Tasks, &task->List);
}

Task* TaskScheduler_NextTask(TaskList* list)
{
	Task* currentTask = list->NextTask;
	if (currentTask == NULL)
	{
		return NULL;
	}
	uint32_t time = list->timeStamp();
	do {
		int32_t timeDelta = (int32_t)(time - currentTask->LastTimestamp);

		// Task needs to be active and period expired for it to be run.
		if (currentTask->Status == ActiveTask && timeDelta >= (int32_t)currentTask->Period)
		{
			currentTask->Status		   = ReadyTask;
			currentTask->LastTimestamp = time;
			list->NextTask			   = (Task*)currentTask->List.Next;
			return currentTask;
		}
		currentTask = (Task*)currentTask->List.Next;
	} while (currentTask != list->NextTask);

	return NULL;
}

void TaskScheduler_ExecuteTask(Task* task)
{
	if (task && task->Status == ReadyTask)
	{
		task->Status = RunningTask;
		task->Callback(task->Data);
		task->Status = CleanTask;
	}
}

void TaskScheduler_QueueTask(TaskList* list, Task* task)
{
	if (task && task->Status == CleanTask)
	{
		if (task->Type == SingleShotTask)
			TaskScheduler_RemoveTask(list, task);
		else
			task->Status = ActiveTask;
	}
}
