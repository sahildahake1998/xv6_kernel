#include "labs/fiber_scheduler.h"

int pow3 (int a, int b) {
  int ans = 1;
  for (int i=0;i<b;i++) 
    ans = ans*a;
  return ans;
}

bool prime3 (int cand) {
	for (int i = 2; i<cand;i++) {
		if (cand % i == 0) 
			return false;
	}
	return true;
}

int char_to_int3 (char func_arg[]) {
  int number = 0;
  int ans = 0;
  for (int i=0;i<10;i++) {
    if (func_arg[i] != '`') number += 1;
  }
  for (int i=0;i<number;i++) {
    ans = ans + (int)(func_arg[i]-'0')*(pow3(10,(number-i-1)));
    // hoh_debug("ans in loop = "<<ans);
  }
  // hoh_debug("answer === "<<ans);
  return ans;
}

void show_answer(shellstate_t& stateinout){
	// if (*display_answer == 2) {
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
	    int completed_processes = 0;
	    hoh_debug("process_status array which entered show answer :- ");
	    for (int i=0;i<5;i++) {
	    	hoh_debug(" "<<stateinout.process_status[i]);
	    	if (stateinout.process_status[i] == 2) {
	    		completed_processes += 1;
	    		stateinout.process_status[i] = 0;
	    	}
	    }

	    int temp = 0;
	    hoh_debug("exited show answer as :- ");
	    for (int i=0; i<5; i++) {
	    	hoh_debug(" "<<stateinout.process_status[i]);
	    	if (stateinout.process_status[i]==1) 
	    		temp += 1;
	    }
	    if (temp == 0) {
	    	stateinout.computing_answer = 0;
	    }
	// }
}

void long1 (addr_t* pmain_stack, addr_t* pf_stack, int* pret, int* index, int* input, int* stat_arr) {
	hoh_debug("running long1");
	addr_t& main_stack = *pmain_stack; 
    addr_t& f_stack = *pf_stack;
	int& ret = *pret;
	int& n = *input;
	int& rank = *index;
	// int& display = *display_answer;
	int candidate = 2;
    int count = 0;
    for(;count < n;) {
   	   	// hoh_debug("candidate = " << candidate);
    	hoh_debug("count = " << count);
    	if (prime3(candidate)) count += 1;
    	candidate += 1;
    	stack_saverestore(f_stack,main_stack);
	}

   	hoh_debug("just after yield");
	for(;;){
        ret = candidate-1;
      	hoh_debug("long computed answer = " << ret);
        stat_arr[rank] = 2;
        stack_saverestore(f_stack,main_stack);
    }



	// int squares = 0;
	
	// for (int i =0; i<n; i++) {
	// 	squares += i*i;
	// 	stack_saverestore(f_stack,main_stack);
	// 	hoh_debug("crossed save_restore at iteration = "<<i);
	// }
	// squares = squares % n;
	// for(;;) {
	// 	ret = squares;
	// 	// display = 1;
	// 	stat_arr[rank] = 2;
	// 	stack_saverestore(f_stack,main_stack);
	// }
}

// void long2 (addr_t* pmain_stack, addr_t* pf_stack, int* pret, int* running_processes, int* input) {
// 	addr_t& main_stack = *pmain_stack; 
//     addr_t& f_stack = *pf_stack;
// 	int& ret = *pret;
// 	int& n = *input;
// 	// int& display = *display_answer;
// 	int& processes = *running_processes;
// 	int cubes = 0;
	// for (int i =0; i<n; i++) {
	// 	cubes += i*i*i;
	// 	stack_saverestore(f_stack,main_stack);
	// }
// 	cubes = cubes % n;
// 	for(;;) {
// 		ret = cubes;
// 		// display = 1;
// 		processes -= 1;
// 		stack_saverestore(f_stack,main_stack);
// 	}
// }
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
void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size){
	// hoh_debug("stat array in fiber sceduler = ");
	shellstate.fiber_scheduler_counter += 1;

	int running_processes = 0;
	for (int i=0;i<5;i++) {
		if (shellstate.process_status[i] == 1) running_processes += 1;
	}   

	int index = shellstate.fiber_scheduler_counter % 5;	stackptrs[index+5];
	// }


	// int display_answer = 0;
	int* received_long_command = &shellstate.received_long_command;
	int* computing_answer = &shellstate.computing_answer;
    // bool* ans_computed = &shellstate.long_answer_computed;
 	// int ready_to_take_input = 0;
  //   if ((*computing_answer == 5 || *computing_answer == 6)  && (*received_long_command == 3 || *received_long_command == 4)) {
  //   	ready_to_take_input = 1;
  //   }
	// int special = 1;
	// for (int i=0;i<5;i++) {
	// 	if (shellstate.process_status[i] == 1) {

	// 	} 
	// }

    if (*received_long_command == 3 && shellstate.init_process == 0) {
		hoh_debug("received long1 command");
		// int& process = shellstate.running_processes;
		char a[10];
        for(int i=0;i<10;i++){a[i]='`';}
        for(int i=6;shellstate.input[i]!='#' && i<17; i++) {a[i-6] = shellstate.input[i];}
        int n = char_to_int3(a);
    	hoh_debug("input received = "<<n);
		// addr_t main_stack = (addr_t)stackptrs[process];
    	// addr_t f_stack = (addr_t)stackptrs[process+5];
		int f_arraysize = arrays_size/stackptrs_size;
		addr_t f_array = &arrays[running_processes*f_arraysize];
		// hoh_debug("add of main_stack = "<<main_stack);
		// hoh_debug("add of f stack = "<<f_stack);
		// stack_init6(stackptrs[running_processes+5], f_array, f_arraysize, &long1, &stackptrs[running_processes], &stackptrs[running_processes+5], &shellstate.p_ret, &index, &n, shellstate.process_status);
		// stack_init6(stackptrs[running_processes+5], f_array, f_arraysize, &long1, &stackptrs[0], &stackptrs[running_processes+5], &shellstate.p_ret, &index, &n, shellstate.process_status);
    	stack_init6(stackptrs[index+5], f_array, f_arraysize, &long1, &stackptrs[0], &stackptrs[index+5], &shellstate.p_ret, &index, &n, shellstate.process_status);
    	
    	hoh_debug("stack initialised");
    	*computing_answer = 5;
    	shellstate.init_process = 1;
    	// shellstate.process_status[running_processes] = 1; 
    	shellstate.process_status[index] = 1;
    	hoh_debug("processes running before this = " << running_processes);
    	index = running_processes;
    	// hoh_debug("counter = "<<counter);
    }

  //   if (*received_long_command == 4 && *computing_answer == 0) {
	 //   	int& process = shellstate.running_processes;
	 //   	char a[10];
  //       for(int i=0;i<10;i++){a[i]='`';}
  //       for(int i=6;shellstate.input[i]!='#' && i<17; i++) {a[i-6] = shellstate.input[i];}
  //       int n = char_to_int3(a);
	 //   	// addr_t main_stack = stackptrs[process];
  //   	// addr_t f_stack = stackptrs[process+5];
  //   	size_t f_arraysize = arrays_size/stackptrs_size;
		// addr_t f_array = arrays + process*f_arraysize;
	 //   	stack_init5(stackptrs[process+5], f_array, f_arraysize, &long2, &stackptrs[process], &stackptrs[process+5], &shellstate.p_ret, &shellstate.running_processes, &n);
  //   	*computing_answer = 6;
  //   	process += 1;
  //   }

    // if ((*computing_answer == 5 || *computing_answer == 6) && ) {
    	// hoh_debug("enterd computation");
    	// hoh_debug("index in fiber = "<<index);
    	// addr_t main_stack = (addr_t)stackptrs[index];
    	// hoh_debug("current running processes = "<< running_processes);
    	// addr_t f_stack = (addr_t)stackptrs[index+5];
    	// hoh_debug("index = "<< index);
    	// stack_saverestore(stackptrs[index], stackptrs[index+5]);
	if (shellstate.process_status[index] == 1)
	stack_saverestore(stackptrs[0], stackptrs[index+5]);
    // }

    for (int i=0;i<5;i++) {
    	if (shellstate.process_status[i] == 2) {
    		show_answer(shellstate);
    	}
    }
}
