#include "labs/fiber.h"

void con_to_char(long n, char* output){
  long temp = n;
    int size = 0;
  while(temp!=0){
    temp = temp/10;
      size++;
  }
    int size_temp = size;
    while(n != 0){
      output[size-1] = (n%10) + '0';
      n = n/10;
      size--;
    }
    for(int j=size_temp; j<64;j++){
      output[j] = ' ';
    }
}

void npr_step(addr_t* pmain_stack, addr_t* pf_stack, long* pret, bool* pdone, int* arg)  //get next step-th number of prime after num (also a prime, found out till now)
{
  addr_t& main_stack  = *pmain_stack;
  addr_t& f_stack  = *pf_stack;
  long& ret = *pret;
  bool& done = *pdone;
  int count = 0;
  long prime = 2;
  int steps =  *arg;
  while(1){
    int flag = 1;
    for(int i=2; i<=prime/2; i++){
      if(prime%i==0){
        flag = 0;
        break;
      }
    }
    prime++;
    if(flag==1){
      count++;
      if(count==steps){
        break;
      }
      done = false;
      ret = prime-1;
      stack_saverestore(f_stack, main_stack);
    }
  }
  prime--;
  for(;;){
      ret = prime;
      done = true;
      stack_saverestore(f_stack, main_stack);
  }
}
void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize, dev_lapic_t lapic){
  //insert your code here
  if(shellstate.shell_state == 0x07 && !shellstate.done_old){
    shellstate.output = "$";
    shellstate.fiber_out = "$";
    return;
  }
  int arg = 0;
  if(shellstate.enter){
    int i=1;
    int func_id=0;
    while(shellstate.curr_cmd[i].char_val!=0x20){ //while not equal to space
      if(i==1){
        if(shellstate.curr_cmd[i+1].char_val == 0x70 && shellstate.curr_cmd[i+2].char_val == 0x72 && shellstate.curr_cmd[i].char_val == 0x6e && shellstate.curr_cmd[i+3].char_val == 0x66){
          func_id = 1;
        }
        else{
          return;
        }
      }
      i++;
    }

    if(func_id == 1 && i == 5){
        i++;
        int temp = 0;
        while(shellstate.curr_cmd[i].char_val!=0x03){ //enter
          temp = ((int)(shellstate.curr_cmd[i].char_val) - 48);
          if(temp<0 || temp>9){
            shellstate.output = "ERROR: Not a valid argument. Only integers allowed!";
            shellstate.done_old = true;
            shellstate.fiber_out = "$";
            shellstate.cursor_y++;
            shellstate.curr_cmd[0].char_val = 0x00;
            // if(!check_fiber_running(shellstate))
                shellstate.shell_state = 0x07;  //initial
            return;
          }
          arg = arg*10 + (temp);
          i++;
        }
    }
    stack_init5(f_stack, f_array, f_arraysize, &npr_step, &main_stack, &f_stack, &shellstate.answer_old, &shellstate.done_old, &arg);
    shellstate.output = "$";
    shellstate.fiber_out = "$";
    shellstate.shell_state = 0x10;  //running
    shellstate.fiber_running = true;
  }
  if(shellstate.shell_state == 0x10 || shellstate.fiber_running){
    shellstate.enter = false;
    stack_saverestore(main_stack, f_stack);
    if(shellstate.done_old == true){
      shellstate.fiber_running = false;
       shellstate.output = "                                                                ";
       shellstate.fiber_out = "$";
       con_to_char(shellstate.answer_old, shellstate.output);
       shellstate.cursor_y++;
       if(shellstate.cursor_y > 19){
        shellstate.cursor_y = 0;
        shellstate.pnt_buf++;
      }
       shellstate.curr_cmd[0].char_val = 0x00;
       // if(!check_fiber_running(shellstate))
          shellstate.shell_state = 0x07;  //initial
    }
  }
}
