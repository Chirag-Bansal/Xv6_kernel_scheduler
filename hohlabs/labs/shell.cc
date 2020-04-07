#include "shell.h"
#include "vgatext.h"

//
// initialize shellstate
//
void shell_init(shellstate_t& state){
  state.num_keys = 0;
  state.shell_state = 0x07; //initial
  state.output = "$";
  state.fiber_out = "$";
  state.pnt_buf = 0;
  state.pnt_cmd = 0;
  state.cursor_x = 2;
  state.cursor_y = 0;
  state.done_old = false;
  state.num_npr = 0;
  state.num_nop = 0;
  state.enter = false;
  state.fiber_running = false;
  state.coroutine_running = false;
  state.scheduler_running = false;
  for(int i=0; i<5; i++){
  	state.done[i] = true;
  	state.answer[i] = 0;
  	state.func_type[i] = 0;

  }

  for(int i=0; i<50;i++){
      if(i == 0)
	  		(state.curr_cmd[i]).char_val = 0x24;
	  	else
	  		(state.curr_cmd[i]).char_val = 0x00;
      (state.curr_cmd[i]).x = 0;
      (state.curr_cmd[i]).y = 0;
  }
  state.answer_old = 0;
}

//0x00 - black; 0x07 - white, 0x01 - blue

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//

uint8_t get_ascii(uint8_t scancode){
	uint8_t answer;
	switch(scancode){
		case 0x1e:
			answer = 0x61;
			break;
		case 0x30:
			answer = 0x62;
			break;
		case 0x2e:
			answer = 0x63;
			break;
		case 0x20:
			answer = 0x64;
			break;
		case 0x12:
			answer = 0x65;
			break;
		case 0x21:
			answer = 0x66;
			break;
		case 0x22:
			answer = 0x67;
			break;
		case 0x23:
			answer = 0x68;
			break;
		case 0x17:
			answer = 0x69;
			break;
		case 0x24:
			answer = 0x6a;
			break;
		case 0x25:
			answer = 0x6b;
			break;
		case 0x26:
			answer = 0x6c;
			break;
		case 0x32:
			answer = 0x6d;
			break;
		case 0x31:
			answer = 0x6e;
			break;
		case 0x18:
			answer = 0x6f;
			break;
		case 0x19:
			answer = 0x70;
			break;
		case 0x10:
			answer = 0x71;
			break;
		case 0x13:
			answer = 0x72;
			break;
		case 0x1f:
			answer = 0x73;
			break;
		case 0x14:
			answer = 0x74;
			break;
		case 0x16:
			answer = 0x75;
			break;
		case 0x2f:
			answer = 0x76;
			break;
		case 0x11:
			answer = 0x77;
			break;
		case 0x2d:
			answer = 0x78;
			break;
		case 0x15:
			answer = 0x79;
			break;
		case 0x2c:
			answer = 0x7a;
			break;
		case 0x0b:
			answer = 0x30;
			break;
		case 0x02:
			answer = 0x31;
			break;
		case 0x03:
			answer = 0x32;
			break;
		case 0x04:
			answer = 0x33;
			break;
		case 0x05:
			answer = 0x34;
			break;
		case 0x06:
			answer = 0x35;
			break;
		case 0x07:
			answer = 0x36;
			break;
		case 0x08:
			answer = 0x37;
			break;
		case 0x09:
			answer = 0x38;
			break;
		case 0x0a:
			answer = 0x39;
			break;
		case 0x39:
			answer = 0x20;
			break;
		case 0x00:
			answer = 0x1b;
			break;
		case 0x1c:
			answer = 0x03;
			break;
	}
	return answer;
}
// 0x20 - func2 of schedule, 0x30 - func1 of schedule  , 0x10 - func of fiber, 0x11 - func of coroutine
void shell_update(uint8_t scankey, shellstate_t& stateinout){
  stateinout.done_old = false;

  hoh_debug("Got: "<<unsigned(scankey));

  stateinout.num_keys++;
  stateinout.output = "$";
  stateinout.fiber_out = "$";
  stateinout.pnt_buf = 0;

  if(stateinout.shell_state == 0x11 || stateinout.shell_state == 0x10)		//coroutine or fibre
      return;

  if(stateinout.cursor_y > 19 && stateinout.cursor_x == 2){		//Page end: refresh
	  	stateinout.pnt_buf++;
	  	stateinout.cursor_y = 0;
  }
  if(stateinout.pnt_cmd == 0){		//refresh state buffer of size 50 for new command
	  for(int i=0;i<50;i++){
		  (stateinout.curr_cmd[i]).char_val = 0x00;
	      (stateinout.curr_cmd[i]).x = 0;
	      (stateinout.curr_cmd[i]).y = 0;
	  }
	  stateinout.cursor_x = 2;
	  if(stateinout.shell_state == 0x20 || stateinout.shell_state == 0x30){
	  		// stateinout.cursor_y++;
	  		stateinout.curr_cmd[0].char_val = 0x24;
	  		stateinout.curr_cmd[0].x = 0;
	  		stateinout.curr_cmd[0].y = stateinout.cursor_y;
	  }
  }
  if(scankey != 0x2a && scankey != 0x1d && scankey != 0x0f && scankey != 0x1a && scankey != 0x1b && scankey != 0x27 && scankey != 0x28){
    stateinout.curr_cmd[stateinout.pnt_cmd+1].char_val = get_ascii(scankey);
    stateinout.curr_cmd[stateinout.pnt_cmd+1].x = stateinout.cursor_x;
    stateinout.curr_cmd[stateinout.pnt_cmd+1].y = stateinout.cursor_y;
    stateinout.pnt_cmd++;
    stateinout.cursor_x++;

    if(scankey == 0x1c){  //enter
    	stateinout.enter = true;
      if(stateinout.shell_state != 0x20 && stateinout.shell_state!=0x30)
      	stateinout.shell_state = 0x01;	//ready state

      stateinout.cursor_y++;
      // hoh_debug("Y value is : " << stateinout.cursor_y);
      stateinout.cursor_x = 2;
      stateinout.pnt_cmd = 0;
    }
  }
}

void convert_to_char(long n, char* output){
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

void factorial(int num, char* output){
  long ans = 1;
  for(int i=2; i<=num; i++){
    ans = ans*i;
  }
  long temp = ans;
  int size = 0;
  while(temp !=0){
  	temp = temp/10;
  	size++;
  }
  convert_to_char(ans,output);
}

void nthPrime(int num, char* output)
{
  int count = 0;
  long prime = 2;
  int flag=1;
  while(1){
  	flag = 1;
    for(int i=2; i<=prime/2; i++){
      if(prime%i==0){
        flag = 0;
        break;
      }
    }
    if(flag==1)
      count++;
    if(count==num)
      break;
  	prime++;
  }
  convert_to_char(prime,output);
}

void fibonacci(int num, char* output){
  int ans;
  int num1 = 1;
  int num2 = 1;
  int num3 = 0;
  if(num==1 || num==2)
    num3 = 1;
  else{
    for(int i=3;i<=num;i++){
    	num3 = num1+num2;
    	num1 = num2;
    	num2 = num3;
    }
  }
  convert_to_char((long)num3, output);
}

// int fibonacci_rec(int num){
//   int ans;

//   if(num>2){
//   	ans  = fibonacci_rec(num-1) + fibonacci_rec(num-2);
//   }
//   else if(num<=2 && num>0){
//   	ans = 1;
//   }

//   return ans;
// }


//
// do computation
//
void shell_step(shellstate_t& stateinout){
  if(stateinout.done_old){
  	stateinout.output = "$";
  	stateinout.enter = false;
  }
  stateinout.done_old = false;
if(stateinout.shell_state == 0x07){
	  stateinout.curr_cmd[0].char_val = 0x24;
	  stateinout.curr_cmd[0].x = 0;
	  stateinout.curr_cmd[0].y = stateinout.cursor_y;
  	  stateinout.enter = false;
}
  if(stateinout.enter){
    // hoh_debug("Enter normal ");
	  int i=1;
	  int func_id=0;
	  stateinout.output = "                                                                ";
	  stateinout.fiber_out = "$";
	  while(stateinout.curr_cmd[i].char_val!=0x20){ //while not equal to space
	  	if(stateinout.curr_cmd[i].char_val == 0x03){
	  		if(i!= 1){
	  			stateinout.output = "ERROR: Not a valid function call. Only npr,fac and fib allowed!";
	  			stateinout.enter = false;
	  		}
	  		func_id = 0;
	  		if(i == 1)
	  			stateinout.output = "$";
	        break;
	  	}
	    if(i==1){
	      if(stateinout.curr_cmd[i].char_val!=0x66 && stateinout.curr_cmd[i].char_val!=0x6e && stateinout.curr_cmd[i].char_val!=0x65){  //not equal to 'f'
	        stateinout.output = "ERROR: Not a valid function call. Only npr,fac and fib allowed!";
	    	stateinout.enter = false;
	        break;
	      }
	    }
	    else if(i==2){
	      if(stateinout.curr_cmd[i].char_val!=0x61 && stateinout.curr_cmd[i].char_val!=0x69 && stateinout.curr_cmd[i].char_val!=0x70 && stateinout.curr_cmd[i].char_val!=0x63 && stateinout.curr_cmd[i].char_val!=0x6f){  //not equal to 'a' or 'i'
	        stateinout.output = "ERROR: Not a valid function call. Only npr,fac and fib allowed!";
	    	stateinout.enter = false;
	    	hoh_debug("Galti mili! hu ha ha");
	        break;
	      }
	    }
	    else if(i==3){
	      if(stateinout.curr_cmd[i-1].char_val == 0x61 && stateinout.curr_cmd[i].char_val == 0x63 && stateinout.curr_cmd[i-2].char_val == 0x66)  //its fac
	        func_id = 2;
	   	  else if(stateinout.curr_cmd[i-1].char_val == 0x69 && stateinout.curr_cmd[i].char_val == 0x62 && stateinout.curr_cmd[i-2].char_val == 0x66)   //its fib
	        func_id = 1;
	      else if(stateinout.curr_cmd[i-1].char_val == 0x70 && stateinout.curr_cmd[i].char_val == 0x72 && stateinout.curr_cmd[i-2].char_val == 0x6e)   //its npr
	        func_id = 3;
	      else if(stateinout.curr_cmd[i-1].char_val == 0x63 && stateinout.curr_cmd[i].char_val == 0x68 && stateinout.curr_cmd[i-2].char_val == 0x65 && stateinout.curr_cmd[i+1].char_val == 0x6f)	//echo
	      	func_id = 4;
	      else if(stateinout.curr_cmd[i-1].char_val == 0x6f && stateinout.curr_cmd[i].char_val == 0x70 && stateinout.curr_cmd[i-2].char_val == 0x6e && stateinout.curr_cmd[i+1].char_val == 0x73)	//nops
	      	func_id = 5;
	      else{
	        stateinout.output = "ERROR: Not a valid function call. Only npr,fac and fib allowed!";
	        stateinout.enter = false;
	        break;
	      }
	    }
	    i++;
	  }
    if(i >5){
      stateinout.output = "ERROR: Not a valid function call. Only npr,fac and fib allowed!";
      stateinout.enter = false;
      stateinout.done_old = true;
      stateinout.cursor_y++;
      stateinout.curr_cmd[0].char_val = 0x00;
      if(stateinout.shell_state!=0x20 && stateinout.shell_state!=0x30)
      		stateinout.shell_state = 0x07;  //initials
      return;
    }
	  if(i == 4 || (func_id == 4 && i == 5)){
		  int arg = 0;
		  if(func_id != 0){  //either fi or fa
		    i++;
		    int temp = 0;
		    if(func_id == 4){	//echo instruction
		    	int start = i;
		    	for(int j=0; j<64; j++){
		    		stateinout.output[j] = ' ';
		    	}
		    	while(stateinout.curr_cmd[i].char_val!=0x03){ //enter
		    		stateinout.output[i-start] = (char)(stateinout.curr_cmd[i].char_val);
		    		i++;
		    	}
		    	stateinout.cursor_y++;
		    	if(stateinout.shell_state != 0x20 && stateinout.shell_state!=0x30)
		    		stateinout.shell_state = 0x07;
          		stateinout.done_old = true;
          		stateinout.curr_cmd[0].char_val = 0x00;
		    	return;
		    }
		    while(stateinout.curr_cmd[i].char_val!=0x03){ //enter
		      temp = ((int)(stateinout.curr_cmd[i].char_val) - 48);
		      if(temp<0 || temp>9){
		        stateinout.output = "ERROR: Not a valid argument. Only integers allowed!";
		        stateinout.enter = false;
		        func_id = 0;
		        break;
		      }
		      arg = arg*10 + (temp);
		      i++;
		    }
		  }

		  //now we have both func type and argument
		  if(func_id == 1){
		    fibonacci(arg,stateinout.output);
		  }
		  else if(func_id == 2){
		    factorial(arg,stateinout.output);
		   }
		   else if(func_id == 3){
		   	nthPrime(arg,stateinout.output);
		   }
	  }
	  if(i == 5 && (func_id==3 || func_id==5)){
	  	stateinout.output = "$";
	  }
      else{  //nprc case
    	if(stateinout.shell_state!=0x20 && stateinout.shell_state!=0x30)
      		stateinout.shell_state = 0x07;  //initial
      	stateinout.done_old = true;
      	stateinout.cursor_y++;
        stateinout.curr_cmd[0].char_val = 0x00;
        stateinout.enter = false;
    }
  }
  else{
  	stateinout.output = "$";
  	stateinout.fiber_out = "$";
  }
 stateinout.fiber_out = "$";
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){
  render.output = " ";
  render.fiber_out = " ";
  render.num_keys =  shell.num_keys;
  render.shell_state = shell.shell_state;
  render.output = shell.output;
  render.fiber_out = shell.fiber_out;

  render.clear = 0;
  if(shell.num_keys == 1){
  	render.pnt_buf = 0;
  }
  int i=0;
  if(shell.pnt_buf > 0){
  	for(int i=0; i<256;i++){
  		render.out_buf[i].char_val = 0x00;
  		render.out_buf[i].x = 0;
  		render.out_buf[i].y = 0;
  	}
  	render.pnt_buf = 0;
  	render.clear = 1;
    // if()
  }
  if (shell.shell_state!=0x11 && shell.shell_state!=0x10){ //&& shell.shell_state!=0x20 && shell.shell_state!=0x30){    //that is not in running state
    while(shell.curr_cmd[i].char_val != 0x00 && shell.curr_cmd[i].char_val != 0x03){
    	render.out_buf[render.pnt_buf+i] = shell.curr_cmd[i];
    	i++;
    }
    if(shell.curr_cmd[i].char_val != 0x03)
  	 render.out_buf[render.pnt_buf+i] = shell.curr_cmd[i];
    if(shell.curr_cmd[i].char_val == 0x03){
     	render.pnt_buf = i;
     }
     render.cursor_x = shell.cursor_x;
     render.cursor_y = shell.cursor_y-1;
     // hoh_debug("Output is: " << render.output);
  }
}

//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if((a.num_keys == b.num_keys)&&
     (a.shell_state == b.shell_state)&&
     (a.output==b.output)&&
     (a.out_buf == b.out_buf)&&
     (a.cursor_x == b.cursor_x)&&
     (a.cursor_y == b.cursor_y))
       return true;
  return false;
}


static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){
  drawrect(0,0,80,25,0x00,0x07,80,25,vgatext_base);
  const char* out_str;
  out_str = "Keys Pressed: ";
  drawtext(1,1,out_str,80,0x00,0x07,80,25,vgatext_base);
  drawnumberinhex(15,1,(uint32_t)state.num_keys,80,0x00,0x07,80,25,vgatext_base);
  if(state.clear == 1)
  	fillrect(1,2,79,24,0x00,0x00,80,25,vgatext_base);
  for(int i=0; state.out_buf[i].char_val != 0x00;i++){
    vgatext::writechar((state.out_buf[i].y+3) * 80 + state.out_buf[i].x+2,state.out_buf[i].char_val,0x00,0x07,vgatext_base);
  }
  if((int)state.output[0] != 36 && (int)state.output[0] != 32){
  	// hoh_debug("Kya main idhar aaya? "<<state.output);
  	drawtext(state.cursor_x+2,state.cursor_y+3,(const char*)state.output,80,0x00,0x07,80,25,vgatext_base);
  	// hoh_debug("FIber ka answer is " << state.fiber_out);
  	if((int)state.fiber_out[0] != 36){
  		drawtext(state.cursor_x+2,state.cursor_y+4,(const char*)state.fiber_out,80,0x00,0x07,80,25,vgatext_base);
  	}
  }
  else if((int)state.fiber_out[0] != 36 && (int)state.fiber_out[0] != 32){
  		// hoh_debug("KYa hua tera vaada? " << state.cursor_y);
  		drawtext(state.cursor_x+2,state.cursor_y+3,(const char*)state.fiber_out,80,0x00,0x07,80,25,vgatext_base);
  	}

}


//
//
// helper functions
//
//

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}
