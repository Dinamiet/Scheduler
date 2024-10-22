#include "scheduler.h"

#include <assert.h>

static void newTask(
		Scheduler*                   scheduler,
		SchedulerTask*               task,
		const size_t                 id,
		const SchedulerTaskType      type,
		const Scheduler_TaskFunction func,
		void*                        data,
		const uint32_t               period);

static void newTask(
		Scheduler*                   scheduler,
		SchedulerTask*               task,
		const size_t                 id,
		const SchedulerTaskType      type,
		const Scheduler_TaskFunction func,
		void*                        data,
		const uint32_t               period)
{
	assert(scheduler != NULL);
	assert(scheduler->Time != NULL);
	assert(task != NULL);
	assert(func != NULL);

	LinkedList_AddEnd(&scheduler->Tasks, task);
	task->ID            = id;
	task->Type          = type;
	task->Status        = SCHEDULER_TASK_ACTIVE;
	task->Period        = period;
	task->LastTimestamp = scheduler->Time();
	task->TaskFunc      = func;
	task->Data          = data;

	if (scheduler->NextTask == NULL)
		scheduler->NextTask = task;
}

void Scheduler_Init(Scheduler* scheduler, const Scheduler_Time time)
{
	assert(scheduler != NULL);
	assert(time != NULL);

	LinkedList_Init(&scheduler->Tasks);
	scheduler->Time     = time;
	scheduler->NextTask = NULL;
}

void Scheduler_CreateRecurringTask(Scheduler* scheduler, SchedulerTask* task, const size_t id, const Scheduler_TaskFunction taskFunc, void* data, const uint32_t period)
{
	newTask(scheduler, task, id, SCHEDULER_RECURRING_TASK, taskFunc, data, period);
}

void Scheduler_CreateSingleTask(Scheduler* scheduler, SchedulerTask* task, const size_t id, const Scheduler_TaskFunction taskFunc, void* data, const uint32_t delay)
{
	newTask(scheduler, task, id, SCHEDULER_SINGLE_TASK, taskFunc, data, delay);
}

SchedulerTaskStatus Scheduler_TaskStatus(const SchedulerTask* task)
{
	assert(task != NULL);

	return task->Status;
}

void Scheduler_Activate(SchedulerTask* task)
{
	assert(task != NULL);

	if (task->Status == SCHEDULER_TASK_INACTIVE)
		task->Status = SCHEDULER_TASK_ACTIVE;
}

void Scheduler_Deactivate(SchedulerTask* task)
{
	assert(task != NULL);

	task->Status = SCHEDULER_TASK_INACTIVE;
}

void Scheduler_ChangePeriod(SchedulerTask* task, const uint32_t newPeriod)
{
	assert(task != NULL);

	task->Period = newPeriod;
}

void Scheduler_Refresh(const Scheduler* scheduler, SchedulerTask* task)
{
	assert(scheduler != NULL);
	assert(scheduler->Time != NULL);
	assert(task != NULL);

	task->LastTimestamp = scheduler->Time();
}

void Scheduler_ChangeTaskFunc(SchedulerTask* task, const Scheduler_TaskFunction taskFunc, void* data)
{
	assert(task != NULL);

	if (taskFunc)
		task->TaskFunc = taskFunc;

	task->Data = data;
}

SchedulerTask* Scheduler_FindTask(Scheduler* scheduler, const size_t id)
{
	assert(scheduler != NULL);

	SchedulerTask* task = LinkedList_First(&scheduler->Tasks);
	if (task)
	{
		do {
			if (task->ID == id)
				return task;

			task = LinkedList_Next(task);
		} while (task != LinkedList_First(&scheduler->Tasks));
	}

	return NULL;
}

void Scheduler_Remove(Scheduler* scheduler, SchedulerTask* task)
{
	assert(scheduler != NULL);

	// Queue the next task if task to be removed is next in queue
	if (scheduler->NextTask == task)
		scheduler->NextTask = LinkedList_Next(task);

	// If next queued task is still the task to be removed (only one task in queue), there is no task left - so clear
	if (scheduler->NextTask == task)
		scheduler->NextTask = NULL;

	LinkedList_Remove(&scheduler->Tasks, task);
}

SchedulerTask* Scheduler_NextReady(Scheduler* scheduler)
{
	assert(scheduler != NULL);
	assert(scheduler->Time != NULL);

	SchedulerTask* currentTask = scheduler->NextTask;
	if (!currentTask)
		return NULL;

	uint32_t time = scheduler->Time();
	do {
		int32_t delta = (int32_t)(time - currentTask->LastTimestamp);

		// Task needs to be active and period expired for it to be the next available task
		if (currentTask->Status == SCHEDULER_TASK_ACTIVE && delta >= (int32_t)currentTask->Period)
		{
			currentTask->Status        = SCHEDULER_TASK_READY;
			currentTask->LastTimestamp = time;
			scheduler->NextTask        = LinkedList_Next(currentTask);
			return currentTask;
		}

		currentTask = LinkedList_Next(currentTask);
	} while (currentTask != scheduler->NextTask);

	return NULL;
}

void Scheduler_Execute(SchedulerTask* task)
{
	if (task && task->Status == SCHEDULER_TASK_READY)
	{
		task->Status = SCHEDULER_TASK_RUNNING;
		task->TaskFunc(task->Data);
		task->Status = SCHEDULER_TASK_CLEAN;
	}
}

void Scheduler_Queue(Scheduler* scheduler, SchedulerTask* task)
{
	assert(scheduler != NULL);

	if (task && task->Status == SCHEDULER_TASK_CLEAN)
	{
		if (task->Type == SCHEDULER_SINGLE_TASK)
		{
			task->Status = SCHEDULER_TASK_INACTIVE;
			Scheduler_Remove(scheduler, task);
		}
		else
		{
			task->Status = SCHEDULER_TASK_ACTIVE;
		}
	}
}
