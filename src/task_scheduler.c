#include "task_scheduler.h"

void TaskScheduler_Init(TaskList* taskList, Task* buffer, uint32_t size)
{
	BufferedList_Init(&taskList->Tasks, (Node*)buffer, sizeof(Task), size);
	taskList->Current = NULL;
}

Task* TaskScheduler_CreateRetriggerTask(TaskList* list, void (*callback)(void*), void* data, uint32_t period)
{
	Task* newTask = (Task*)BufferedList_LinkTail(&list->Tasks);
	if (newTask)
	{
		newTask->Name		  = 0;
		newTask->Type		  = RecurringTask;
		newTask->Status		  = ActiveTask;
		newTask->Period		  = period;
		newTask->LastTimestep = 0;
		newTask->Callback	  = callback;
		newTask->Data		  = data;
	}

	if (list->Current == NULL)
		list->Current = newTask;

	return newTask;
}
// TODO: This function is very close to other create - create 'private' function which does the initialization
Task* TaskScheduler_CreateSingleShotTask(TaskList* list, void (*callback)(void*), void* data, uint32_t delay)
{
	Task* newTask = (Task*)BufferedList_LinkTail(&list->Tasks);
	if (newTask)
	{
		newTask->Name		  = 0;
		newTask->Type		  = SingleShotTask;
		newTask->Status		  = ActiveTask;
		newTask->Period		  = delay;
		newTask->LastTimestep = 0;
		newTask->Callback	  = callback;
		newTask->Data		  = data;
	}

	if (list->Current == NULL)
		list->Current = newTask;

	return newTask;
}

void TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status) { task->Status = status; }

void TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period) { task->Period = period; }

void TaskScheduler_ChangeTaskCallback(Task* task, void (*callback)(void*), void* data)
{
	task->Callback = callback;
	task->Data	   = data;
}

void TaskScheduler_RunNextTask()
{
	//TODO: All logic basically goes here...
}
