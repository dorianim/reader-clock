#include "Task.h"

#include <Arduino.h>

void _runTask(void *ptr) {
  Task *task = (Task *)ptr;
  task->setup();

  for (;;) {
    vTaskDelay(10);
    task->loop();
  }

  vTaskDelete(NULL);
}

Task::Task(const char *name) {
  Serial.printf("[I] Taks create: %s\n", name);
  xTaskCreate(_runTask, name, 10000, this, 1, NULL);
}

Task::~Task() {}
