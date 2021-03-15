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
      if (i == 2) {
        log_event("working");
        log_event("exiting");
        exit(0);
      }
      yield();
    }
  }
}

void print_x0_x1(int x0, int x1) {
    printf("current x0: %i, x1: %i\n", x0, x1);
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
  printf("Entering Hello\n");
#if 0
  printf("stack address: 0x%x\n", current_thread->stack_ptr);

  register size_t reg0 __asm(RCHR "0") = 1;
  register size_t reg1 __asm(RCHR "1") = 2;
  register size_t reg2 __asm(RCHR "2") = 3;
  register size_t reg3 __asm(RCHR "3") = 4;

  asm volatile("bl save_thread");
  print_current_context();
  
  printf("stack address: 0x%x\n", current_thread->stack_ptr);

  printf("test end %i %i %i %i\n", reg0, reg1, reg2, reg3);

  while(1) {};
#endif
  set_kernel_config(KCFG_LOG_SCHEDULER, 0);

  printf("Doing syscall\n");
  add_thread_from_worker(thread_worker_1);

  while (1) {
    log_event("working");
    yield();
  }
}
