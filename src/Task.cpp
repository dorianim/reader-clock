#include "Task.h"

#include <Arduino.h>

void Task::_runTask(void *ptr) {
  Task *task = (Task *)ptr;

  while (!task->_constructorDone) {
    vTaskDelay(10);
  }

  task->setup();

  for (;;) {
    vTaskDelay(10);
    task->loop();
  }

  vTaskDelete(NULL);
}

Task::Task(const char *name) {
  Serial.printf("[I] Taks create: %s\n", name);
  this->_constructorDone = false;
  xTaskCreate(Task::_runTask, name, 10000, this, 1, NULL);
}

Task::~Task() {}
