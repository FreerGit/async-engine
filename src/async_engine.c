#include "async_engine.h"
#include <stdio.h>
#include <stdlib.h>

void async_engine_init(Engine *engine, size_t initial_cap) {
  engine->tasks = (Task *)malloc(initial_cap * sizeof(Task));
  if (!engine->tasks) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  engine->count = 0;
  engine->capacity = initial_cap;
  engine->shutdown = 0;

  PT_INIT(&engine->pt);
}

void async_engine_cleanup(Engine *engine) {
  free(engine->tasks);
}

int async_engine_add_task(Engine *engine, task_function_t function, void *arg) {
  if (engine->count == engine->capacity) {
    size_t new_capacity = engine->capacity * 2;
    Task *new_tasks = (Task *)realloc(engine->tasks, new_capacity * sizeof(Task));
    if (!new_tasks) {
      perror("realloc");
      return -1;
    }
    engine->tasks = new_tasks;
    engine->capacity = new_capacity;
  }

  engine->tasks[engine->count++] = (Task){.function = function, .arg = arg};
  return 0;
}

PT_THREAD(async_engine_run(Engine *engine)) {
  static size_t i;

  PT_BEGIN(&engine->pt);

  static bool done = true;
  while (!engine->shutdown) {
    for (i = 0; i < engine->count; i++) {
      if (!engine->tasks[i].is_done) {
        bool ret = engine->tasks[i].function(engine->tasks[i].arg);
        engine->tasks[i].is_done = ret;
        done = false;
      }
    }

    if (done) {
      engine->shutdown = true;
    }
    done = true;
    PT_YIELD(&engine->pt);
  }

  PT_END(&engine->pt);
}