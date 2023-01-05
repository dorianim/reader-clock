#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

class Task {
protected:
    Task();
    virtual ~Task();
    virtual void run() = 0;
    friend void _runTask(void* task);
};

#endif // TASK_H