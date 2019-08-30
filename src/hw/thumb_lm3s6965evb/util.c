void exit(int status) {
  asm volatile (
      "ldr r0, =0x18\n\t"    // angel_SWIreason_ReportException
      "ldr r1, =0x20026\n\t" // ADP_Stopped_ApplicationExit
      "svc 0xAB\n\t"         // have monitor make semihosting call
  );
}

// GDB helper to get current Cortex-M privilege level
// Yes, 1 means unprivileged. I know, weird right?
typedef enum { privileged, unprivileged } plevel;
plevel pl(void) {
  plevel level;
  asm volatile(
      "mrs r0, control\n\t"
      "mov r1, #1\n\t"
      "and r0, r0, r1\n\t"
      "mov %0, r0\n\t"
  : "=r"(level)
  );
  return level;
}