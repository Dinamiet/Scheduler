#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "linkedlist.h"

#include <stdint.h>

typedef enum _SchedulerTaskType_
{
	SCHEDULER_TASK_RECURRING,
	SCHEDULER_TASK_SINGLE,
} SchedulerTaskType;

typedef enum _SchedulerTaskStatus_
{
	SCHEDULER_TASK_INACTIVE,
	SCHEDULER_TASK_ACTIVE,
	SCHEDULER_TASK_READY,
	SCHEDULER_TASK_RUNNING,
	SCHEDULER_TASK_CLEAN,
} SchedulerTaskStatus;

typedef void (*Scheduler_TaskFunction)(void* data);
typedef uint32_t (*Scheduler_Time)();

typedef struct _SchedulerTask_
{
	Node Node;
	size_t ID;
	SchedulerTaskType Type;
	SchedulerTaskStatus Status;
	uint32_t Period;
	uint32_t LastTimestamp;
	Scheduler_TaskFunction TaskFunc;
	void* Data;
} SchedulerTask;

typedef struct _Scheduler_
{
	SchedulerTask* NextTask;
	LinkedList Tasks;
	Scheduler_Time Time;
} Scheduler;

void Scheduler_Init(Scheduler* scheduler, Scheduler_Time time);
void Scheduler_RecurringTask(Scheduler* scheduler, SchedulerTask* task, size_t id, Scheduler_TaskFunction taskFunc, void* data, uint32_t period);
void Scheduler_SingleTask(Scheduler* scheduler, SchedulerTask* task, size_t id, Scheduler_TaskFunction taskFunc, void* data, uint32_t delay);
void Scheduler_Activate(SchedulerTask* task);
void Scheduler_Deactivate(SchedulerTask* task);
void Scheduler_ChangePeriod(SchedulerTask* task, uint32_t newPeriod);
void Scheduler_ChangeTaskFunc(SchedulerTask* task, Scheduler_TaskFunction taskFunc, void* data);
SchedulerTask* Scheduler_FindTask(Scheduler* scheduler, size_t id);
void Scheduler_Remove(Scheduler* scheduler, SchedulerTask* task);
SchedulerTask* Scheduler_NextReady(Scheduler* scheduler);
void Scheduler_Execute(SchedulerTask* task);
void Scheduler_Queue(Scheduler* scheduler, SchedulerTask* task);

#endif
