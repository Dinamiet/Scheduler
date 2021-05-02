#ifndef __TASK_SCHEDULER__
#define __TASK_SCHEDULER__

#include "bufferedlist.h"

#include <stdint.h>

typedef enum
{
	RecurringTask,
	SingleShotTask
} TaskTypes;

typedef enum
{
	InactiveTask,
	ActiveTask
} TaskStatus;

typedef struct
{
	uint32_t   Name;
	TaskTypes  Type;
	TaskStatus Status;
	uint32_t   Period;
	uint32_t   LastTimestep;
	void (*Callback)(void*);
	void* Data;
} Task;

typedef struct
{
	Task*		 Current;
	BufferedList Tasks;
} TaskList;

void  TaskScheduler_Init(TaskList* taskList, Task* buffer, uint32_t size);
Task* TaskScheduler_CreateRetriggerTask(TaskList* list, void (*callback)(void*), void* data, uint32_t period);
Task* TaskScheduler_CreateSingleShotTask(TaskList* list, void (*callback)(void*), void* data, uint32_t delay);
void  TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status);
void  TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period);
void  TaskScheduler_ChangeTaskCallback(Task* task, void (*callback)(void*), void* data);
// TODO: Remove task from the list
void TaskScheduler_RunNextTask();

#endif // __TASK_SCHEDULER__
