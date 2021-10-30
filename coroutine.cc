#include "labs/coroutine.h"
// #include "labs/shell.cc"

bool prime1 (int cand) {
	for (int i = 2; i<cand;i++) {
		if (cand % i == 0) 
			return false;
	}
	return true;
}
 
int pow1 (int a, int b) {
  int ans = 1;
  for (int i=0;i<b;i++) 
    ans = ans*a;
  return ans;
}

int char_to_int1 (char func_arg[]) {
  int number = 0;
  int ans = 0;
  for (int i=0;i<10;i++) {
    if (func_arg[i] != '`') number += 1;
  }
  for (int i=0;i<number;i++) {
    ans = ans + (int)(func_arg[i]-'0')*(pow1(10,(number-i-1)));
    // hoh_debug("ans in loop = "<<ans);
  }
  // hoh_debug("answer === "<<ans);
  return ans;
}

void is_done1(shellstate_t& stateinout) {
  if (stateinout.computing_answer == 4) {
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
    // stateinout.long_computation_n = 0;
    // stateinout.long_answer_computed = false;
    stateinout.computing_answer = 0;
    hoh_debug("the answer array is = " << stateinout.answer);
    // hoh_debug("FLag val is: " << (int)stateinout.flag);
  }
}

void mprime(coroutine_t* pf_coro, f_t* pf_locals, int* p_ret, int* computing_answer, bool* ans_computed)
{
    coroutine_t& f_coro = *pf_coro;
    int& candidate = pf_locals->candidate;
    int& count = pf_locals->count;
    int& n = pf_locals->n; 
    h_begin(f_coro);
    hoh_debug("passed begin");
    for(;count < n;)
    {
   	   	hoh_debug("candidate = " << candidate);
    	hoh_debug("count = " << count);
    	if (prime1(candidate)) {
    		count += 1;
    	}
    	candidate += 1;
    	hoh_debug("just before yield");
    	h_yield(f_coro);
    	hoh_debug("just after yield");
	}
	*computing_answer = 4;
	*ans_computed = true;
	int answer = candidate - 1;
	*p_ret = answer;
	hoh_debug("long computed answer = " << *p_ret);
	h_end(f_coro);
}

void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){
    // hoh_debug("n in coroutine = "<<shellstate.long_computation_n);
    int* computing_answer = &shellstate.computing_answer;
    bool* ans_computed = &shellstate.long_answer_computed;
    int* received_long_command = &shellstate.received_long_command;
	int* p_ret = &shellstate.p_ret;
	f_t* pf_locals = &f_locals;
	coroutine_t *pf_coro = &f_coro;
    if (*received_long_command==1 && *computing_answer == 0 && !*ans_computed) {
    	coroutine_reset(f_coro);
        char a[10];
        int i;
                // hoh_debug("a before accepting n = "<<a);
        for(int i=0;i<10;i++){a[i]='`';}
        for(i=7;shellstate.input[i]!='#' && i<17; i++) {a[i-7] = shellstate.input[i];}
                // hoh_debug("a after accepting n = "<<a);
        int n = char_to_int1(a);
                // hoh_debug("n in shell = "<<n);
                // hoh_debug("computing answer flag = "<<stateinout.computing_answer);
                // hoh_debug("long_answer_computed flag = "<<stateinout.long_answer_computed);
        // shellstate.long_computation_n = n;
        pf_locals-> candidate = 2;
	    pf_locals-> count = 0;
        pf_locals -> n = n;
        hoh_debug("initializing n to "<<n);
    	*computing_answer = 3;
    }
    if (*computing_answer == 3 && !*ans_computed) {
    	mprime(pf_coro,pf_locals,p_ret,computing_answer,ans_computed);
    	hoh_debug("FLag value is: "<< *computing_answer);
    }
    is_done1(shellstate);
}


