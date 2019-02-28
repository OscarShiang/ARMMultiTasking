#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>

#define MAX_THREADS 10

struct MonitorConfig {
  bool destroy_on_stack_err;
  bool exit_when_no_threads;
} config;

struct ThreadArgs {
  void* a1;
  void* a2;
  void* a3;
  void* a4;
};

#define make_args(a,b,c,d) \
{ (void*)a, (void*)b, (void*)c, (void*)d }

int add_thread(void (*worker)(void));
int add_named_thread(void (*worker)(void), const char* name);
int add_named_thread_with_args(void (*worker)(), const char* name, struct ThreadArgs args);

bool is_valid_thread(int tid);
int get_thread_id();
const char* get_thread_name();

void yield();
bool yield_next();
bool yield_to(int id);
void __attribute__((noreturn)) start_scheduler();
void log_event(const char* event);
bool get_msg(int* sender, int* message);
bool send_msg(int destination, int message);

#endif /* ifdef THREAD_H */
