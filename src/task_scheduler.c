#include "task_scheduler.h"

// #include "fnv.h"
// #define HASH_FUNC FNV_Hash

#include "sdbm.h"
#define HASH_FUNC SDBM_Hash

#include <string.h>

void TaskScheduler_Init(TaskList* taskList, TimeStampCallback timeStampFunc, Task* buffer, size_t size)
{
	BufferedList_Init(&taskList->Tasks, (Node*)buffer, sizeof(Task), size);
	taskList->timeStamp = timeStampFunc;
	taskList->NextTask	= NULL;
}

static Task* createNewTask(TaskList* list, char* taskName, TaskTypes type, TaskCallback callback, void* data, uint32_t period)
{
	Task* newTask = (Task*)BufferedList_LinkTail(&list->Tasks);
	if (newTask)
	{
		newTask->Name		   = HASH_FUNC(taskName, strlen(taskName));
		newTask->Type		   = type;
		newTask->Status		   = ActiveTask;
		newTask->Period		   = period;
		newTask->LastTimestamp = list->timeStamp();
		newTask->Callback	   = callback;
		newTask->Data		   = data;
	}

	if (list->NextTask == NULL)
		list->NextTask = newTask;

	return newTask;
}

Task* TaskScheduler_CreateRetriggerTask(TaskList* list, char* name, TaskCallback callback, void* data, uint32_t period)
{
	return createNewTask(list, name, RecurringTask, callback, data, period);
}

Task* TaskScheduler_CreateSingleShotTask(TaskList* list, char* name, TaskCallback callback, void* data, uint32_t delay)
{
	return createNewTask(list, name, SingleShotTask, callback, data, delay);
}

void TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status) { task->Status = status; }

void TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period) { task->Period = period; }

void TaskScheduler_ChangeTaskCallback(Task* task, TaskCallback callback, void* data)
{
	task->Callback = callback;
	task->Data	   = data;
}

Task* TaskScheduler_FindTask(TaskList* list, char* name)
{
	Task*	 task	  = (Task*)list->Tasks.Used.Head;
	uint32_t taskName = HASH_FUNC(name, strlen(name));
	if (task == NULL)
		return NULL;

	do {
		if (task->Name == taskName)
			return task;
		task = (Task*)task->List.Next;
	} while (task != (Task*)list->Tasks.Used.Head);

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

	BufferedList_UnlinkNode(&list->Tasks, &task->List);
}

void TaskScheduler_RunNextTask(TaskList* list)
{
	Task* currentTask = list->NextTask;
	if (currentTask == NULL)
	{
		return;
	}
	do {
		uint32_t timeDelta = list->timeStamp() - currentTask->LastTimestamp;

		// Task needs to be active and period expired for it to be run.
		if (currentTask->Status == ActiveTask && currentTask->Period <= timeDelta)
		{
			currentTask->Status = RunningTask;
			currentTask->Callback(currentTask->Data);
			currentTask->Status		   = ActiveTask;
			currentTask->LastTimestamp = list->timeStamp();
			list->NextTask			   = (Task*)currentTask->List.Next;

			if (currentTask->Type == SingleShotTask)
				TaskScheduler_RemoveTask(list, currentTask);

			return;
		}
		currentTask = (Task*)currentTask->List.Next;
	} while (currentTask != list->NextTask);
}
