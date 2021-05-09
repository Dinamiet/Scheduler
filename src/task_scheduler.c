#include "task_scheduler.h"

void TaskScheduler_Init(TaskList* taskList, uint32_t (*timeStampFunc)(), Task* buffer, uint32_t size)
{
	BufferedList_Init(&taskList->Tasks, (Node*)buffer, sizeof(Task), size);
	taskList->timeStamp = timeStampFunc;
	taskList->NextTask	= NULL;
}

Task* createNewTask(TaskList* list, TaskTypes type, void (*callback)(void*), void* data, uint32_t period)
{
	Task* newTask = (Task*)BufferedList_LinkTail(&list->Tasks);
	if (newTask)
	{
		newTask->Name		   = 0;
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

Task* TaskScheduler_CreateRetriggerTask(TaskList* list, void (*callback)(void*), void* data, uint32_t period)
{
	return createNewTask(list, RecurringTask, callback, data, period);
}

Task* TaskScheduler_CreateSingleShotTask(TaskList* list, void (*callback)(void*), void* data, uint32_t delay)
{
	return createNewTask(list, SingleShotTask, callback, data, delay);
}

void TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status) { task->Status = status; }

void TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period) { task->Period = period; }

void TaskScheduler_ChangeTaskCallback(Task* task, void (*callback)(void*), void* data)
{
	task->Callback = callback;
	task->Data	   = data;
}

void TaskScheduler_RemoveTask(TaskList* list, Task* task)
{
	if (list->NextTask == task)
		list->NextTask = (Task*)task->List.Next;
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
		uint32_t now = list->timeStamp();
		if (currentTask->Status == ActiveTask && currentTask->Period <= (now - currentTask->LastTimestamp))
		{
			currentTask->Callback(currentTask->Data);
			currentTask->LastTimestamp = now;
			list->NextTask			   = (Task*)currentTask->List.Next;

			if (currentTask->Type == SingleShotTask)
				TaskScheduler_RemoveTask(list, currentTask);

			return;
		}
		currentTask = (Task*)currentTask->List.Next;
	} while (currentTask != list->NextTask);
}
