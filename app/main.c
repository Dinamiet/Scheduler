#include "task_scheduler.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TASK_BUFF_SIZE	 4
#define TEST_BUFFER_SIZE 50

uint32_t counter   = 0;
uint32_t startTime = 0;
uint32_t resultBuffer[TEST_BUFFER_SIZE];

uint32_t getCurrentTime()
{
	clock_t t		= clock();
	double	seconds = (double)t / (double)CLOCKS_PER_SEC;
	return seconds * 1000;
}

void IncTask()
{
	uint32_t diff			= getCurrentTime() - startTime;
	resultBuffer[counter++] = diff;
	printf("Inc on: %d\n", diff);
}

void WrongInc()
{
	uint32_t diff			= getCurrentTime() - startTime;
	resultBuffer[counter++] = diff << 1;
	printf("WrongInc on: %d\n", diff);
}

int main()
{
	TaskList tasklist;
	tasklist.Current = NULL;
	if (tasklist.Current != NULL)
	{
		return 1;
	}
	startTime = getCurrentTime();
	Task taskBuffer[TASK_BUFF_SIZE];
	TaskScheduler_Init(&tasklist, &getCurrentTime, taskBuffer, TASK_BUFF_SIZE);

	TaskScheduler_CreateSingleShotTask(&tasklist, &IncTask, NULL, 5);
	Task* changingTask = TaskScheduler_CreateRetriggerTask(&tasklist, &IncTask, NULL, 7);
	TaskScheduler_CreateSingleShotTask(&tasklist, &IncTask, NULL, 2);
	Task* removeTask = TaskScheduler_CreateRetriggerTask(&tasklist, &IncTask, NULL, 10);

	Task* testTask = TaskScheduler_CreateRetriggerTask(&tasklist, &IncTask, NULL, 10);
	if (testTask != NULL)
	{
		return TEST_BUFFER_SIZE + 1;
	}

	testTask = TaskScheduler_CreateSingleShotTask(&tasklist, &IncTask, NULL, 10);
	if (testTask != NULL)
	{
		return TEST_BUFFER_SIZE + 1;
	}

	uint32_t testBuffer[TEST_BUFFER_SIZE] = {2, 5, 7, 10, 14, 20, 21, 28, 30, 40, 50, 51, 58, 60, 61, 64, 67, 70, 70, 73, 76, 79};
	uint32_t testIndex					  = 0;

	while (getCurrentTime() - startTime < 31) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 31)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	TaskScheduler_ChangeTaskStatus(changingTask, InactiveTask);
	while (getCurrentTime() - startTime < 51) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 51)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	TaskScheduler_ChangeTaskStatus(changingTask, ActiveTask);
	while (getCurrentTime() - startTime < 61) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 61)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	TaskScheduler_ChangeTaskPeriod(changingTask, 3);
	while (getCurrentTime() - startTime < 71) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 71)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	TaskScheduler_RemoveTask(&tasklist, removeTask);
	while (getCurrentTime() - startTime < 81) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 81)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	testTask = TaskScheduler_CreateRetriggerTask(&tasklist, &WrongInc, NULL, 1);
	if (testTask == NULL)
	{
		return TEST_BUFFER_SIZE + 2;
	}
	TaskScheduler_ChangeTaskCallback(testTask, &IncTask, NULL);
	while (getCurrentTime() - startTime < 86) { TaskScheduler_RunNextTask(&tasklist); }
	while (testBuffer[testIndex] < 86)
	{
		if (testBuffer[testIndex] != resultBuffer[testIndex])
		{
			return testIndex;
		}
		testIndex++;
	}

	return 0;
}
