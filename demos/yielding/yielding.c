#include "user/thread.h"
#include "user/util.h"
#include "port/uart.h"

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

void main(void) {
  uart_init();
  uart_puts("Hello World");

  char c;
  while((c = uart_getc())) {
      uart_send(c);
  }
}

void setup(void) {
  set_kernel_config(KCFG_LOG_SCHEDULER, 0);

  add_thread_from_worker(thread_worker_1);

  while (1) {
    log_event("working");
    yield();
  }
}
