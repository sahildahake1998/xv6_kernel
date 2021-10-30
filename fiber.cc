#include "labs/fiber.h"


bool prime2 (int cand) {
	for (int i = 2; i<cand;i++) {
		if (cand % i == 0) 
			return false;
	}
	return true;
}

int pow2 (int a, int b) {
  int ans = 1;
  for (int i=0;i<b;i++) 
    ans = ans*a;
  return ans;
}

int char_to_int2 (char func_arg[]) {
  int number = 0;
  int ans = 0;
  for (int i=0;i<10;i++) {
    if (func_arg[i] != '`') number += 1;
  }
  for (int i=0;i<number;i++) {
    ans = ans + (int)(func_arg[i]-'0')*(pow2(10,(number-i-1)));
    // hoh_debug("ans in loop = "<<ans);
  }
  // hoh_debug("answer === "<<ans);
  return ans;
}


void is_done(shellstate_t& stateinout) {
  if (stateinout.computing_answer==2) {
    hoh_debug("entered is done of fibres");
    int ans = stateinout.p_ret;
    int count = 0;
    char ans_arr[10];
    for(int i=0;i<10;i++){ans_arr[i]='`';}
    while (ans) {
      int temp = ans % 10;
      ans_arr[count] = temp+'0';
      ans = ans / 10;
      count += 1;
    }
    // hoh_debug("in shell = " << ans_arr);
    for(int i=0;i<count;i++)
      stateinout.answer[i] = ans_arr[count-i-1];
    stateinout.flag = true;
    stateinout.computing_answer = 0;
    // stateinout.long_computation_n = 0;
    // stateinout.long_answer_computed = false;
    hoh_debug("the answer array is = " << stateinout.answer);
    // hoh_debug("FLag val is: " << (int)stateinout.flag);
  }
}

void fprime(addr_t* pmain_stack, addr_t* pf_stack, int* pret, bool* pdone, int* cdone, int* long_computation_n)
{
	// hoh_debug("entered fprime");
    addr_t& main_stack = *pmain_stack; 
    addr_t& f_stack = *pf_stack;
    int& ret = *pret;
    int& n = *long_computation_n;
    bool& done = *pdone;
    int& computing_answer = *cdone; 
    int candidate = 2;
    int count = 0;
    for(;count < n;)
    {
   	   	hoh_debug("candidate = " << candidate);
    	hoh_debug("count = " << count);
    	if (prime2(candidate)) {
    		count += 1;
    	}
    	candidate += 1;
    	// hoh_debug("just before yield");
    	// h_yield(f_coro);
    	stack_saverestore(f_stack,main_stack);
    	// hoh_debug("just after yield");
	}
	candidate--;
	// h_end(f_coro);
	for(;;){
        ret = candidate;
      	hoh_debug("long computed answer = " << ret);
        done = true; 
        computing_answer = 2;
        stack_saverestore(f_stack,main_stack);
    }
}

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize){
	int* computing_answer = &shellstate.computing_answer;
   	bool* ans_computed = &shellstate.long_answer_computed;
    int* received_long_command = &shellstate.received_long_command;

	// execute only once
	// hoh_debug("into shell step fiber");
	if (*received_long_command == 2 && *computing_answer==0 && !*ans_computed) {
		hoh_debug("initialising n in fiber");
		char a[10];
        int i;
                // hoh_debug("a before accepting n = "<<a);
        for(int i=0;i<10;i++){a[i]='`';}
        for(i=7;shellstate.input[i]!='#' && i<17; i++) {a[i-7] = shellstate.input[i];}
                // hoh_debug("a after accepting n = "<<a);
        int n = char_to_int2(a);
    	*computing_answer = 1;
    	hoh_debug("stack to be initialised");
		stack_init6(f_stack, &f_array, f_arraysize, &fprime, &main_stack, &f_stack, &shellstate.p_ret, &shellstate.long_answer_computed, &shellstate.computing_answer, &n);
	}
	if (*computing_answer == 1 && !*ans_computed) {
		hoh_debug("computing using fibres");
		stack_saverestore(main_stack,f_stack);	
	}
    is_done(shellstate);
  //insert your code here

}

