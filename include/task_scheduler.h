#ifndef __TASK_SCHEDULER__
#define __TASK_SCHEDULER__

#include "bufferedlist.h"

#include <stdint.h>
#include <stddef.h>

typedef enum
{
	RecurringTask,
	SingleShotTask
} TaskTypes;

typedef enum
{
	InactiveTask,
	ActiveTask,
	RunningTask
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
	BufferedList	  Tasks;
	TimeStampCallback timeStamp;
} TaskList;

void  TaskScheduler_Init(TaskList* taskList, TimeStampCallback timeStampFunc, Task* buffer, size_t size);
Task* TaskScheduler_CreateRetriggerTask(TaskList* list, char* name, TaskCallback callback, void* data, uint32_t period);
Task* TaskScheduler_CreateSingleShotTask(TaskList* list, char* name, TaskCallback callback, void* data, uint32_t delay);
void  TaskScheduler_ChangeTaskStatus(Task* task, TaskStatus status);
void  TaskScheduler_ChangeTaskPeriod(Task* task, uint32_t period);
void  TaskScheduler_ChangeTaskCallback(Task* task, TaskCallback callback, void* data);
Task* TaskScheduler_FindTask(TaskList* list, char* name);
void  TaskScheduler_RemoveTask(TaskList* list, Task* task);
void  TaskScheduler_RunNextTask(TaskList* list);

#endif
