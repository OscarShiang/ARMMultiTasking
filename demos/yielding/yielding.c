#include "user/thread.h"
#include "user/util.h"
#include "port/uart.h"
#include "common/print.h"
#include "port/port.h"
#include "kernel/thread.h"

#include "common/arm_generic_asm.h"

__attribute__((noreturn)) void thread_worker_1() {
  while (1) {
    for (int i = 0;; ++i) {
      // printf("[User] current: %i\n", i);
      if (i == 2) {
        log_event("working");
        log_event("exiting");
        exit(0);
      }
      yield();
    }
  }
}

void print_something() {
    printf("[Debug] run here hihi\n");
}

void print_elr_el1(size_t x) {
    printf("[Debug] address: 0x%x\n");
}

void print_current_context() {
    print_register_context((RegisterContext *)current_thread->stack_ptr);
}

void invalid_entry()
{
    printf("[Error] Invalid exception occurs\n");
    while(1) {
	// do nothing
    }
}

void main() {
    uart_init();
    uart_puts("test\n");
    while(1) {
	// nothing
    }
}

void setup(void) {
  printf("Doing syscall\n");

  set_kernel_config(KCFG_LOG_SCHEDULER, 0);
  add_thread_from_worker(thread_worker_1);

  printf("[User] Back to setup\n");

  while (1) {
    log_event("working");
    yield();
  }
}
