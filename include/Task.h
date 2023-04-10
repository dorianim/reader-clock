#ifndef TASK_H
#define TASK_H

#include <Arduino.h>

class Task {
protected:
  Task(const char *name);
  virtual ~Task();
  virtual void setup() = 0;
  virtual void loop() = 0;

  bool _constructorDone;

private:
  static void _runTask(void *task);
};

#endif // TASK_H