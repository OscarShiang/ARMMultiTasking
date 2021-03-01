ENTRY(_Reset)
SECTIONS
{
 . = 0x80000;
 .text : {
    KEEP(*(.text.boot))
    *(.text*)
    *(.rodata*)
    /* align to pointer size (8 for AArch64) */
    . = ALIGN(8);
    _init_array = .;
    *(.init_array*)
    _einit_array = .;
    _etext = . ; }
 .data :
  /* AT ( ADDR (.text) + SIZEOF (.text) ) */
  {
    _data = . ;
    *(.data)
    *(.thread_vars)
    _edata = . ; }
 /* align 16 for AArch64 */
 . = ALIGN(16);
 . = . + 0xC00; /* 3kB of stack for kernel */
 stack_top = .;
 .bss (NOLOAD) : {
    _bstart = . ;
    /* Thread structs go first! */
    *(.thread_structs)
    /* then the important zero init globals */
    *(.thread_vars_bss)
    *(.bss COMMON)
    /* Putting this first minimises waste on Arm/Thumb due to alignment below */
    *(.code_page_backing)
    /* AArch64 PIE requires that programs start on 4k alignemnt for adrp usage.
       We don't need to align for function starts, we get the proper entry addr from the elf.
    */
    . = ALIGN(0x1000);
    /* on the end in case of weird stuff writing beyond */
    *(.code_page)
    _bend = . ;
  }
  /* Don't need unwinding info on Arm */
  /DISCARD/ : { *(.ARM.exidx* ) }
}

_bsize = (_bend - _bstart) >> 3;
