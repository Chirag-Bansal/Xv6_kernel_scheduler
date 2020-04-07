#include "labs/fiber_scheduler.h"

//
// stackptrs:      Type: addr_t[stackptrs_size].  array of stack pointers (generalizing: main_stack and f_stack)
// stackptrs_size: number of elements in 'stacks'.
//
// arrays:      Type: uint8_t [arrays_size]. array of memory region for stacks (generalizing: f_array)
// arrays_size: size of 'arrays'. equal to stackptrs_size*STACK_SIZE.
//
// Tip: divide arrays into stackptrs_size parts.
// Tip: you may implement a circular buffer using arrays inside shellstate_t
//      if you choose linked lists, create linked linked using arrays in
//      shellstate_t. (use array indexes as next pointers)
// Note: malloc/new is not available at this point.
//

void co_to_char(long n, char* output){
	long temp = n;
  	int size = 0;
  	if(n == 0){
  		output[0] = '0';
  		size = 1;
  	}
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

void npr_fiber(addr_t* pmain_stack, addr_t* pf_stack, long* pret, bool* pdone, int* arg)  //get next step-th number of prime after num (also a prime, found out till now)
{
  addr_t& main_stack  = *pmain_stack;
  addr_t& f_stack  = *pf_stack;
  long& ret = *pret;
  bool& done = *pdone;
  int count = 0;
  int iter = 1;
  long prime = 2;
  int steps =  *arg;
  // hoh_debug("Steps are: " << steps);
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
      iter ++;
      if(iter%1000==0){
      	stack_saverestore(f_stack, main_stack);
      }
    }
  }
  prime--;
  for(;;){
      ret = prime;
      done = true;
      hoh_debug("done");
      stack_saverestore(f_stack, main_stack);
  }
}
 // nopf 10 => Number of prime factors/fibres
void nop_fiber(addr_t* pmain_stack, addr_t* pf_stack, long* pret, bool* pdone, int* arg){
  addr_t& main_stack  = *pmain_stack;
  addr_t& f_stack  = *pf_stack;
  long& ret = *pret;
  bool& done = *pdone;
  int count = 0;
  int iter = 1;
  long prime = 2;
  int steps =  *arg;
  // hoh_debug("Steps are: " << steps);
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
      iter ++;
      // if(iter%1000==0){
      // 	stack_saverestore(f_stack, main_stack);
      // }
    }
  }
  prime--;
  for(;;){
  	  hoh_debug("Answer done! "<< (int)prime);
      ret = prime % 6;
      done = true;
      // hoh_debug("done");
      // stack_saverestore(f_stack, main_stack);
  }
}

bool check_done(shellstate_t& shellstate){
	for(int i=0; i<5;i++){
		if(shellstate.done[i] == false)
			return false;
	}
	return true;
}

void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t lapic){

	size_t stack_size = arrays_size/stackptrs_size;
  //insert your code here
  if(shellstate.shell_state == 0x07 && !shellstate.done_old && check_done(shellstate)){
    shellstate.output = "$";
    return;
  }

  preempt.timer_count = 20 *  1000000; // 1 us 
  preempt.timer_state=0; // By Default Timer is never set.
  int arg = 0;
  if(shellstate.enter) {

  	// Creating Main Stack
  	  if(check_done(shellstate)){
	    shellstate.fiber_index = 0;
  	  }
	  int i=1;
	  int func_id=0;	//func_id = 1 (npr), func_id = 2 (nop)
	  while(shellstate.curr_cmd[i].char_val!=0x20){ //while not equal to space
	  	 if(i==3){
	      if(shellstate.curr_cmd[i-1].char_val == 0x70 && shellstate.curr_cmd[i].char_val == 0x72 && shellstate.curr_cmd[i-2].char_val == 0x6e && shellstate.curr_cmd[i+1].char_val == 0x73)
	        func_id = 1;
	      else if(shellstate.curr_cmd[i-1].char_val == 0x6f && shellstate.curr_cmd[i].char_val == 0x70 && shellstate.curr_cmd[i-2].char_val == 0x6e && shellstate.curr_cmd[i+1].char_val == 0x73)
	        func_id = 2;
	      else{
	        return;
	      }
	    }
	    i++;
	  }
	  if((func_id == 1 || func_id == 2) && i == 5){		//npr or nop
	    i++;
	    int temp = 0;
	    while(shellstate.curr_cmd[i].char_val!=0x03){ //enter
	      temp = ((int)(shellstate.curr_cmd[i].char_val) - 48);
	      if(temp<0 || temp>9){
					shellstate.fiber_out = "ERROR: Not a valid argument. Only integers allowed!";
					shellstate.done_old = true;
					shellstate.curr_cmd[0].char_val = 0x00;
					shellstate.fiber_out = "$";
					shellstate.cursor_y++;
					shellstate.shell_state = 0x07;  //initial
					return;
	      }
	      arg = arg*10 + (temp);
	      i++;
	    }
	  }

	  int free_ind = -1;

	  //Compute next free pointer in array...................
	  for(int i=0; i<5;i++){
	  	if(shellstate.done[i] == true){
	  		free_ind = i;
	  		break;
	  	}
	  }

	  if(free_ind == -1){
	  	shellstate.fiber_out = "ERROR: YOu cannot have more than 5 computations!";
	  	shellstate.cursor_y++;
		shellstate.shell_state = 0x07;
	  	return;
	  }

	  if(func_id==1){
	  	if(shellstate.num_npr == 3){	//exceeding the limit
			shellstate.fiber_out = "ERROR: You cannot run more than 3 long computation functions at a time!";
			shellstate.cursor_y++;
			shellstate.shell_state = 0x07;
			return;
		}
		else{
			shellstate.num_npr += 1;
		}
		addr_t f_stack = addr_t(0xfacebaad);
	    stack_init5(stackptrs[free_ind], (arrays + (free_ind * stack_size)), stack_size, &npr_fiber, &main_stack, &stackptrs[free_ind], &shellstate.answer[free_ind], &shellstate.done[free_ind], &arg);
        hoh_debug("Timer set");
  		hoh_debug("Free index is: " << free_ind);
	    shellstate.func_type[free_ind] = 1;
		shellstate.fiber_out = "$";
		shellstate.done[free_ind] = false;
		shellstate.answer[free_ind] = 0;
    	shellstate.shell_state = 0x20;  //running
    	shellstate.enter = false;
	  }

	  else if (func_id==2){
	  	if(shellstate.num_nop == 3){	//exceeding the limit
			shellstate.fiber_out = "ERROR: You cannot run more than 3 long computation functions at a time!";
			shellstate.cursor_y++;
			shellstate.shell_state = 0x07;
			return;
		}
		else{
			shellstate.num_nop += 1;
		}
		addr_t f_stack = addr_t(0xfacebaad);
	    stack_init5(stackptrs[free_ind], (arrays + (free_ind * stack_size)), stack_size, &nop_fiber, &main_stack, &stackptrs[free_ind], &shellstate.answer[free_ind], &shellstate.done[free_ind], &arg);
  		
        hoh_debug("Timer set");
  		hoh_debug("Free index is: " << free_ind);
	    shellstate.func_type[free_ind] = 2;

		shellstate.fiber_out = "$";
		shellstate.done[free_ind] = false;
		shellstate.answer[free_ind] = 0;
    	shellstate.shell_state = 0x30;  //running
    	shellstate.enter = false;
	  }

  }

  if(shellstate.shell_state == 0x20 || shellstate.shell_state == 0x30 || !check_done(shellstate)){
  	preempt.timer_state=1;
  	shellstate.fiber_index = (shellstate.fiber_index+1)%5;
  	while(shellstate.done[shellstate.fiber_index]){
  		shellstate.fiber_index = (shellstate.fiber_index+1)%5;
  	}
  	if(shellstate.func_type[shellstate.fiber_index]==1)
  		shellstate.shell_state == 0x20;
  	else
  		shellstate.shell_state == 0x30;

    preempt.yeild_state=0;	
  	preempt.saved_stack=stackptrs[shellstate.fiber_index];
  	///////////////////
	lapic.reset_timer_count(preempt.timer_count);
    stack_saverestore(main_stack, preempt.saved_stack);
	//////////////////
	// hoh_debug("Normally yeilded");
    preempt.yeild_state=1;    
	// hoh_debug_if(preempt.timer_state==2,"End Timer");
	if(preempt.timer_state==2)
        stackptrs[shellstate.fiber_index]=preempt.saved_stack;  // Always save the stack again using new address if the timer ran out.
     // hoh_debug_if(preempt.timer_state==1,"Timer running");
    if(shellstate.done[shellstate.fiber_index] == true){
    	if(shellstate.shell_state == 0x30)
    		shellstate.num_nop--;
    	else
    		shellstate.num_npr--;
		shellstate.fiber_out = "                                                                ";
		shellstate.output = "$";
		shellstate.curr_cmd[0].char_val = 0x00;
        co_to_char(shellstate.answer[shellstate.fiber_index], shellstate.fiber_out);
	    shellstate.cursor_y++;
	    if(shellstate.cursor_y > 19){
	    	shellstate.cursor_y = 0;
	    	shellstate.pnt_buf++;
	    }
	    if(check_done(shellstate)){
	    	shellstate.shell_state = 0x07;  //done
			// hoh_debug("Exited with "<<preempt.timer_state);
	    }
    }
  }
}
