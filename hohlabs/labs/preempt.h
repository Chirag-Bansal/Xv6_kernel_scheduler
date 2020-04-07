// #pragma once

// #include "util/config.h"
// #include "devices/lapic.h"



// //
// // preempt_t : State for your timer/preemption handler
// //
// // Note:
// //  We've one instance of core_t per each core.
// //  To access this instance, you need to use %gs:0
// //  (The entire kernel doesn't have any global/static variables)
// //
// // %gs:core_offset_preempt will point to start of preempt_t instance
// //
// // for example: 
// // %gs:0 will return pointer to core_t
// // %gs:core_offset_mainstack will return core_t::main_stack
// // %gs:core_offset_preempt+0 will return core_t::saved_stack
// //
// // etc.
// //
// // See Definition of core_t in x86/main.h
// //

// struct preempt_t{
//   // your data structure, if any
//   addr_t saved_stack; //feel free to change it - provided as an example
//   uint32_t timer_state;   //state of the timer = 0 (not present), = 1 (active), = 2 (ran out)
//   uint32_t yeild_state;     //exited via yield or directly through preempt
//   uint32_t timer_count;
// };


// // //
// // // 
// // // This macro is being called from x86/except.cc
// // //
// // //
// // // _name: label name
// // // _f   : C function to be called 
// // //        ex: we may have to do send EOI to LAPIC or PIC etc.
// // //
// //
// // if thread is already inside yield,
// //   jmp iret_toring0

// // save the CPU state to current stack (fiber's stack)
// // save the current stack pointer to core_t.preempt.saved_stack
// // switch context and stack using stack_saverestore()
// // ... (control will not reach here immediately, it will reach here only on the next context switch back to this fiber)
// // restore CPU state from the current stack

// // jmp iret_toring0
// // Assumping sizeof(addr_t is 4);
// #  define  _ring0_preempt(_name,_f)            \
//   asm volatile(                                       \
//       "  .text                            \n\t"\
//       " " STR(_name) ":                   \n\t"\
//       "  pushl %edx                       \n\t"\
//       "  pushl %ecx                       \n\t"\
//       "  pushl %eax                       \n\t"\
//       "  call " STR(_f) "                 \n\t"\
//       "  popl  %eax                       \n\t"\
//       "  popl  %ecx                       \n\t"\
//       "  popl  %edx                       \n\t"\
//       "                                   \n\t"\
//       "  sti                              \n\t"\
//       "  cmp  $0x1, %gs:" STR(core_offset_preempt+8) " \n\t"\
//       "  je  iret_toring0                 \n\t"\
//       "  movl  $0x2, %gs:" STR(core_offset_preempt+4) " \n\t"\
//       "                                   \n\t"\
//       "  pushl %ebp                       \n\t"\
//       "  movl %esp, %ebp                  \n\t"\
//       "  subl  $0x200, %esp               \n\t"\
//       "  andl  $0xfffffff0, %esp          \n\t"\
//       "  fxsave (%esp)                    \n\t"\
//       "                                   \n\t"\
//       "  pushl %edi                       \n\t"\
//       "  pushl %esi                       \n\t"\
//       "  pushl %eax                       \n\t"\
//       "  pushl %ebx                       \n\t"\
//       "  pushl %ecx                       \n\t"\
//       "  pushl %edx                       \n\t"\
//       "  pushl %ebp                       \n\t"\
//       "  pushl $1f                        \n\t"\
//       "                                   \n\t"\
//       "  movl  %esp, %gs:" STR(core_offset_preempt+0) " \n\t"\
//       "  movl  %gs:" STR(core_offset_mainstack) ", %esp \n\t"\
//       "                                   \n\t"\
//       "  ret                              \n\t"\
//       "                                   \n\t"\
//       "1:                                 \n\t"\
//       "  popl %ebp                       \n\t"\
//       "  popl %edx                       \n\t"\
//       "  popl %ecx                       \n\t"\
//       "  popl %ebx                       \n\t"\
//       "  popl %eax                       \n\t"\
//       "  popl %esi                       \n\t"\
//       "  popl %edi                       \n\t"\
//       "  fxrstor (%esp)                   \n\t"\
//       "  movl %ebp, %esp                  \n\t"\
//       "  popl  %ebp                       \n\t"\
//       "                                   \n\t"\
//       "  jmp iret_toring0                 \n\t"\
//       );                                       \

// // Do _f things 
// // First save the ebp
// // Then put esp into ebp
// // subl allocates 512 bytes for registers.
// // andl makes the esp word aligned (anding with all bits except last 4 set)
// // Use fxsave to save all the registers into stack


// // Use fxrstor to pop all registers back from stack
// // Restore esp from ebp
// // Restore ebp from stack
// // // "                                   \n\t"
// // :                                        
// //       :"a" (&from_stack), "c"  (&to_stack)     
// //       :_ALL_REGISTERS, "memory"                


#pragma once

#include "util/config.h"
#include "devices/lapic.h"



//
// preempt_t : State for your timer/preemption handler
//
// Note:
//  We've one instance of core_t per each core.
//  To access this instance, you need to use %gs:0
//  (The entire kernel doesn't have any global/static variables)
//
// %gs:core_offset_preempt will point to start of preempt_t instance
//
// for example: 
// %gs:0 will return pointer to core_t
// %gs:core_offset_mainstack will return core_t::main_stack
// %gs:core_offset_preempt+0 will return core_t::saved_stack
//
// etc.
//
// See Definition of core_t in x86/main.h
//

struct preempt_t{
  addr_t saved_stack;
  uint32_t timer_state; // 0 if Timer is absent 1 if Timer is Active and 2 if Timer has run out. 
  uint32_t yeild_state; // 0 if Not yielded and 1 if Yielded // Used in case manual yield points are also present.
  uint32_t timer_count; // 1us  
};


//
// 
// This macro is being called from x86/except.cc
//
//
// _name: label name
// _f   : C function to be called 
//        ex: we may have to do send EOI to LAPIC or PIC etc.
//
// if thread is already inside yield,
//   jmp iret_toring0

// save the CPU state to current stack (fiber's stack)
// save the current stack pointer to core_t.preempt.saved_stack
// switch context and stack using stack_saverestore()
// ... (control will not reach here immediately, it will reach here only on the next context switch back to this fiber)
// restore CPU state from the current stack

// jmp iret_toring0
// Assumping sizeof(addr_t is 4);
#  define  _ring0_preempt(_name,_f)            \
  asm volatile(                                       \
      "  .text                            \n\t"\
      " " STR(_name) ":                   \n\t"\
      "  pushl %edx                       \n\t"\
      "  pushl %ecx                       \n\t"\
      "  pushl %eax                       \n\t"\
      "  call " STR(_f) "                 \n\t"\
      "  popl  %eax                       \n\t"\
      "  popl  %ecx                       \n\t"\
      "  popl  %edx                       \n\t"\
      "                                   \n\t"\
      "  cmp  $0x1, %gs:" STR(core_offset_preempt+8) " \n\t"\
      "  je  iret_toring0                 \n\t"\
      "  movl  $0x2, %gs:" STR(core_offset_preempt+4) " \n\t"\
      "  sti                              \n\t"\
      "                                   \n\t"\
      "  pushl %ebp                       \n\t"\
      "  movl %esp, %ebp                  \n\t"\
      "  subl  $0x200, %esp               \n\t"\
      "  andl  $0xfffffff0, %esp          \n\t"\
      "  fxsave (%esp)                    \n\t"\
      "                                   \n\t"\
      "  pushl %eax                       \n\t"\
      "  pushl %ebx                       \n\t"\
      "  pushl %ecx                       \n\t"\
      "  pushl %edx                       \n\t"\
      "  pushl %esi                       \n\t"\
      "  pushl %edi                       \n\t"\
      "  pushl %ebp                       \n\t"\
      "  pushl $1f                        \n\t"\
      "                                   \n\t"\
      "  movl  %esp, %gs:" STR(core_offset_preempt+0) " \n\t"\
      "  movl  %gs:" STR(core_offset_mainstack) ", %esp \n\t"\
      "                                   \n\t"\
      "  ret                              \n\t"\
      "                                   \n\t"\
      "1:                                 \n\t"\
      "  popl  %ebp                       \n\t"\
      "  popl  %edi                       \n\t"\
      "  popl  %esi                       \n\t"\
      "  popl  %edx                       \n\t"\
      "  popl  %ecx                       \n\t"\
      "  popl  %ebx                       \n\t"\
      "  popl  %eax                       \n\t"\
      "  fxrstor (%esp)                   \n\t"\
      "  movl %ebp, %esp                  \n\t"\
      "  popl  %ebp                       \n\t"\
      "                                   \n\t"\
      "  jmp iret_toring0                 \n\t"\
      );                                       \

// Do _f things 
// First save the ebp
// Then put esp into ebp
// subl allocates 512 bytes for registers.
// andl makes the esp word aligned (anding with all bits except last 4 set)
// Use fxsave to save all the registers into stack


// Use fxrstor to pop all registers back from stack
// Restore esp from ebp
// Restore ebp from stack
// // "                                   \n\t"
// :                                        
//       :"a" (&from_stack), "c"  (&to_stack)     
//       :_ALL_REGISTERS, "memory"                
