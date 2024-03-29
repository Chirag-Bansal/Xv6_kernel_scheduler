#pragma once

#include "labs/shell.h"
#include "util/coroutine.h"

// state for your coroutine implementation:
struct f_t{
    //insert your code here
    long prime;
    int count;
    int steps;
	// Data local to coroutine between successive calls
};

//PC value stored in coroutine_t

void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals);
