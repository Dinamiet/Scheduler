#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/**
 * \file
 * Task Scheduler
 *
 * Best effort scheduling of tasks
 */

#include "linkedlist.h"

#include <stdint.h>

/**
 * Task types
 */
typedef enum _SchedulerTaskType_
{
	SCHEDULER_RECURRING_TASK, /** Recurring/repeating task */
	SCHEDULER_SINGLE_TASK,    /** Execute/run task only once */
} SchedulerTaskType;

/**
 * Task status flags
 */
typedef enum _SchedulerTaskStatus_
{
	SCHEDULER_TASK_INACTIVE, /** Task is disabled, will not be queued for execution */
	SCHEDULER_TASK_ACTIVE,   /** Task is active, will be queued for execution as requested */
	SCHEDULER_TASK_READY,    /** Task is ready to be executed, and removed from the waiting queue */
	SCHEDULER_TASK_RUNNING,  /** Task is currently executing */
	SCHEDULER_TASK_CLEAN,    /** Task is done with execution, marked for cleanup and placed back into the waiting queue */
} SchedulerTaskStatus;

/**
 * The function which a task should execute
 * \param data The data required by the task to work on
 */
typedef void (*Scheduler_TaskFunction)(void* data);

/**
 * Scheduler time function template.
 * It is used by the scheduler to check when tasks are ready.
 * \return Current system time/counter
 */
typedef uint32_t (*Scheduler_Time)();

/**
 * Task information
 */
typedef struct _SchedulerTask_
{
	Node                   Node;
	size_t                 ID;
	SchedulerTaskType      Type;
	SchedulerTaskStatus    Status;
	uint32_t               Period;
	uint32_t               LastTimestamp;
	Scheduler_TaskFunction TaskFunc;
	void*                  Data;
} SchedulerTask;

/**
 * Scheduler information
 */
typedef struct _Scheduler_
{
	SchedulerTask* NextTask;
	LinkedList     Tasks;
	Scheduler_Time Time;
} Scheduler;

/**
 * Scheduler initialization
 * \param scheduler Scheduler structure to initialize
 * \param time The time function linked to the scheduler
 */
void Scheduler_Init(Scheduler* scheduler, const Scheduler_Time time);

/**
 * Creates a new active recurring task in the scheduler.
 * \param scheduler The scheduler to which task should be added
 * \param task The task to be added to the scheduler
 * \param id Identification assigned to the task
 * \param taskFunc The function to be executed when the task is run
 * \param data The data passed to the task when executed
 * \param period How often the task should be executed
 */
void Scheduler_CreateRecurringTask(
		Scheduler*                   scheduler,
		SchedulerTask*               task,
		const size_t                 id,
		const Scheduler_TaskFunction taskFunc,
		void*                        data,
		const uint32_t               period);

/**
 * Creates a new active single execution task in the scheduler.
 * \param scheduler The scheduler to which task should be added
 * \param task The task to be added to the scheduler
 * \param id Identification assigned to the task
 * \param taskFunc The function to be executed when the task is run
 * \param data The data passed to the task when executed
 * \param delay How long to wait before the task is executed
 */
void Scheduler_CreateSingleTask(Scheduler* scheduler, SchedulerTask* task, const size_t id, const Scheduler_TaskFunction taskFunc, void* data, const uint32_t delay);

/**
 * Retreives the current status of a task
 * \param task The task of interest
 * \return The tasks current status
 */
SchedulerTaskStatus Scheduler_TaskStatus(const SchedulerTask* task);

/**
 * Changes a tasks status to active.
 * Active tasks will be executed when required.
 * Only inactive task's status can be updated/changed.
 * \param task The task's status to change
 */
void Scheduler_Activate(SchedulerTask* task);

/**
 * Changes a tasks status to inactive.
 * Inactive tasks will not be executed when required.
 * Only active task's status can be updated/changed.
 * \param task The task's status to change
 */
void Scheduler_Deactivate(SchedulerTask* task);

/**
 * Changes the delay or period of a task
 * \param task Task's period/delay to change
 * \param newPeriod The new period/delay to assigned to the task
 */
void Scheduler_ChangePeriod(SchedulerTask* task, const uint32_t newPeriod);

/**
 * Changes the function of a task.
 * \param task Task's function to change
 * \param taskFunc The new function to assign to the task. Set to NULL to only change data.
 * \param data New function data for the task.
 */
void Scheduler_ChangeTaskFunc(SchedulerTask* task, const Scheduler_TaskFunction taskFunc, void* data);

/**
 * Find a task added to scheduler
 * \param scheduler The scheduler to search through for the task
 * \param id Task identifier to search for
 * \return First task with a matching ID, NULL if id could not be found in scheduler
 */
SchedulerTask* Scheduler_FindTask(Scheduler* scheduler, const size_t id);

/**
 * Remove a task from the scheduler
 * \param scheduler The scheduler from which the task will be removed
 * \param task The task to remove
 */
void Scheduler_Remove(Scheduler* scheduler, SchedulerTask* task);

/**
 * Retreives the next task to be executed from the scheduler
 * \param scheduler The scheduler to search through for the next ready task
 * \return Next task ready for execution
 */
SchedulerTask* Scheduler_NextReady(Scheduler* scheduler);

/**
 * Executes a task marked as ready for execution
 * \param task The task to execute
 */
void Scheduler_Execute(SchedulerTask* task);

/**
 * Queues and readies a task after execution
 * \param scheduler The scheduler to which the task belongs
 * \param task The task to be queued again after execution
 */
void Scheduler_Queue(Scheduler* scheduler, SchedulerTask* task);

#endif
