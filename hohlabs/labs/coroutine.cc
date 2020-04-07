#include "labs/coroutine.h"

void conv_to_char(long n, char* output){
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

void npr_step(coroutine_t* pf_coro, f_t* pf_locals, long* pret, bool* pdone)  //get next step-th number of prime after num (also a prime, found out till now)
{
  coroutine_t& f_coro  = *pf_coro;
  long& ret = *pret;
  bool& done = *pdone;
  int& count = pf_locals->count;
  long& prime = pf_locals->prime;
  int& steps = pf_locals->steps;
  // hoh_debug("Shaloo de do yaar2: " << pf_locals->steps);
  // hoh_debug("Steps are2: " << steps);
  h_begin(f_coro);

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
      // hoh_debug("Value is: " << (int)prime);
      h_yield(f_coro);
    }
  }
  prime--;
  ret = prime;
  // hoh_debug("Value is: " << (int)prime);
  done = true;
  h_end(f_coro);
}
void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){
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
        if(shellstate.curr_cmd[i+1].char_val == 0x70 && shellstate.curr_cmd[i+2].char_val == 0x72 && shellstate.curr_cmd[i].char_val == 0x6e && shellstate.curr_cmd[i+3].char_val == 0x63){
          hoh_debug("Function mil gaya");
          func_id = 1;
        }
        else{
          return;
        }
      }
      i++;
    }
    hoh_debug("Value of i is: " << i);
    if(func_id == 1 && i == 5){
        i++;
        int temp = 0;
        while(shellstate.curr_cmd[i].char_val!=0x03){ //enter
          temp = ((int)(shellstate.curr_cmd[i].char_val) - 48);
          if(temp<0 || temp>9){
            shellstate.output = "ERROR: Not a valid argument. Only integers allowed!";
            shellstate.done_old = true;
            shellstate.curr_cmd[0].char_val = 0x00;
            shellstate.fiber_out = "$";
            shellstate.cursor_y++;
            // if(!chk_fiber_running(shellstate))
            		shellstate.shell_state = 0x07;  //initial
            return;
          }
          arg = arg*10 + (temp);
          i++;
        }
    }
    coroutine_reset(f_coro);
    shellstate.output = "$";
    shellstate.fiber_out = "$";
    shellstate.shell_state = 0x11;  //running
    f_locals.steps = arg;
    f_locals.count = 0;
    f_locals.prime = 2;
    shellstate.coroutine_running = true;
  }

  if(shellstate.shell_state == 0x11 || shellstate.coroutine_running){
    shellstate.enter = false;
    npr_step(&f_coro, &f_locals, &shellstate.answer_old, &shellstate.done_old);
    if(shellstate.done_old == true){
      shellstate.coroutine_running = false;
       shellstate.curr_cmd[0].char_val = 0x00;
       shellstate.output = "                                                                ";
       shellstate.fiber_out = "$";
       conv_to_char(shellstate.answer_old, shellstate.output);
      //  hoh_debug("Coroutine output written!");
       shellstate.cursor_y++;
       if(shellstate.cursor_y > 19){
        shellstate.cursor_y = 0;
        shellstate.pnt_buf++;
      }
       // if(!chk_fiber_running(shellstate))
       		shellstate.shell_state = 0x07;  //initial
      hoh_debug("Done value is: " << (int) shellstate.done[0]);
    }
  }
}
