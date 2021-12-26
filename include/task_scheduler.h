#ifndef __TASK_SCHEDULER__
#define __TASK_SCHEDULER__

#include "linkedlist.h"

#include <stddef.h>
#include <stdint.h>

typedef enum
{
	RecurringTask,
	SingleShotTask
} TaskTypes;

typedef enum
{
	InactiveTask,
	ActiveTask,
	ReadyTask,
	RunningTask,
	CleanTask
} TaskStatus;

typedef void (*TaskCallback)(void*);
typedef uint32_t (*TimeStampCallback)();

typedef struct
{
	Node		 List;
	uint32_t	 Name;
	TaskTypes	 Type;
	TaskStatus	 Status;
	uint32_t	 Period;
	uint32_t	 LastTimestamp;
	void*		 Data;
	TaskCallback Callback;
} Task;

typedef struct
{
	Task*			  NextTask;
	LinkedList		  Tasks;
	TimeStampCallback timeStamp;
} TaskList;

void TaskScheduler_Init(TaskList* taskList, TimeStampCallback timeStampFunc);

void TaskScheduler_CreateRetriggerTask(TaskList* list, Task* task, char* name, TaskCallback callback, void* data, uint32_t period);
void TaskScheduler_CreateSingleShotTask(TaskList* list, Task* task, char* name, TaskCallback callback, void* data, uint32_t delay);

void TaskScheduler_ActivateTask(Task* task);
void TaskScheduler_DeactivateTask(Task* task);
void TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period);
void TaskScheduler_ChangeTaskCallback(Task* task, TaskCallback callback, void* data);

Task* TaskScheduler_FindTask(TaskList* list, char* name);
void  TaskScheduler_RemoveTask(TaskList* list, Task* task);

Task* TaskScheduler_NextTask(TaskList* list);
void  TaskScheduler_ExecuteTask(Task* task);
void  TaskScheduler_QueueTask(TaskList* taskList, Task* task);

#endif
