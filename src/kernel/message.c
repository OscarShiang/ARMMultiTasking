#include "common/errno.h"
#include "kernel/thread.h"

static void inc_msg_pointer(Thread* thr, Message** ptr) {
  ++(*ptr);
  // Wrap around from the end to the start
  if (*ptr == &(thr->messages[THREAD_MSG_QUEUE_SIZE])) {
    *ptr = &(thr->messages[0]);
  }
}

bool k_get_msg(int* sender, int* message) {
  if (!sender || !message) {
    user_thread_info.err_no = E_INVALID_ARGS;
    return false;
  }

  // If message box is not empty, or it is full
  if (current_thread->next_msg != current_thread->end_msgs ||
      current_thread->msgs_full) {
    *sender = current_thread->next_msg->src;
    *message = current_thread->next_msg->content;

    inc_msg_pointer(current_thread, &current_thread->next_msg);
    current_thread->msgs_full = false;

    return true;
  }

  return false;
}

bool k_send_msg(int destination, int message) {
  if (
      // Invalid destination
      destination >= MAX_THREADS || destination < 0 ||
      all_threads[destination].id == INVALID_THREAD ||
      // Buffer is full
      all_threads[destination].msgs_full) {
    return false;
  }

  Thread* dest = &all_threads[destination];
  Message* our_msg = dest->end_msgs;
  our_msg->src = k_get_thread_id();
  our_msg->content = message;
  inc_msg_pointer(dest, &(dest->end_msgs));
  dest->msgs_full = dest->next_msg == dest->end_msgs;

  return true;
}
