#pragma once
#include "util/config.h"
#include "util/debug.h"

struct character{
	uint8_t char_val;
	int x;
	int y;
};
struct shellstate_t{
	int num_keys;
	uint8_t shell_state;
	int pnt_buf;
	struct character curr_cmd[50];		//current command
	int pnt_cmd;
	char* fiber_out;
	char* output;
	int cursor_x;
	int cursor_y;
	bool done_old;
	bool done[5];
	long answer[5];
	long answer_old;
	int fiber_index;
	int num_npr;
	int num_nop;
	bool enter;
	int func_type[5];
	bool coroutine_running;
	bool fiber_running;
	bool scheduler_running;
};

struct renderstate_t{
	int num_keys;
	uint8_t shell_state;
	struct character out_buf[256];
	int pnt_buf;
	char* output;
	char* fiber_out;
	int cursor_x;
	int cursor_y;
	int clear;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);
