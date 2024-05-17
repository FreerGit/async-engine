#include "async_engine.h"
#include <stdio.h>
#include <unistd.h>

typedef struct {
  int counter;
} ArbitraryData;

bool increment_task(void *arg) {
  ArbitraryData *data = (ArbitraryData *)arg;
  printf("Incrementing counter: %d\n", ++data->counter);
  return data->counter >= 8;
}

bool decrement_task(void *arg) {
  ArbitraryData *data = (ArbitraryData *)arg;
  printf("Decrementing counter: %d\n", --data->counter);
  return data->counter <= -5;
}

int main() {
  Engine engine;
  async_engine_init(&engine, 8);
  ArbitraryData data1 = {0};
  ArbitraryData data2 = {0};

  // Add tasks
  async_engine_add_task(&engine, increment_task, &data1);
  async_engine_add_task(&engine, decrement_task, &data2);

  while (PT_SCHEDULE(async_engine_run(&engine))) {
    // engine.shutdown = 1;
    // Run the async engine until all tasks are processed
    // async_engine_add_task(&engine, decrement_task, &data2);
  }

  async_engine_cleanup(&engine);
  return 0;
}