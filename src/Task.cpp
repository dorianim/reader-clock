#include "Task.h"

#include <Arduino.h>

void _runTask(void* ptr)
{
    Task* task = (Task*)ptr;
    task->run();
}

Task::Task()
{
    xTaskCreate(_runTask, "Task", 10000, this, 1, NULL);
}

Task::~Task()
{
}

