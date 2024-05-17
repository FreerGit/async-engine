#ifndef ASYNC_ENGINE_H
#define ASYNC_ENGINE_H

#include "pt.h"
#include <stdbool.h>
#include <stddef.h>

// returns zero to signal continue and non-zero for exit.
typedef bool (*task_function_t)(void *);

typedef struct {
  task_function_t function;
  void *arg;
} Task;

typedef struct {
  struct pt pt;
  Task *tasks;
  size_t count;
  size_t capacity;
  bool shutdown;
} Engine;

void async_engine_init(Engine *engine, size_t initial_cap);
void async_engine_cleanup(Engine *engine);
int async_engine_add_task(Engine *engine, task_function_t function, void *arg);
PT_THREAD(async_engine_run(Engine *engine));

#endif // ASYNC_ENGINE_H