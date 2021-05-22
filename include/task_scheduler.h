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
	Node	   List;
	uint32_t   Name;
	TaskTypes  Type;
	TaskStatus Status;
	uint32_t   Period;
	uint32_t   LastTimestamp;
	void*	   Data;
	void (*Callback)(void*);
} Task;

typedef struct
{
	Task*		 NextTask;
	BufferedList Tasks;
	uint32_t (*timeStamp)();
} TaskList;

void  TaskScheduler_Init(TaskList* taskList, uint32_t (*timeStampFunc)(), Task* buffer, uint32_t size);
Task* TaskScheduler_CreateRetriggerTask(TaskList* list, char* name, void (*callback)(void*), void* data, uint32_t period);
Task* TaskScheduler_CreateSingleShotTask(TaskList* list, char* name, void (*callback)(void*), void* data, uint32_t delay);
void  TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status);
void  TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period);
void  TaskScheduler_ChangeTaskCallback(Task* task, void (*callback)(void*), void* data);
Task* TaskScheduler_FindTask(TaskList* list, char *name);
void  TaskScheduler_RemoveTask(TaskList* list, Task* task);
void  TaskScheduler_RunNextTask(TaskList* list);

#endif // __TASK_SCHEDULER__
