#pragma once
#include "util/config.h"
#include "util/debug.h"

struct shellstate_t {
	int keys_pressed;
	char input[50];
	char answer[50];
	int process_status[5];
	int offset;
	int p_ret;
	int fiber_scheduler_counter;
	// int long_computation_n;
	// int running_processes;
	int computing_answer;
	int received_long_command;
	int init_process;
	bool flag;
	bool long_answer_computed;
	
	// bool fiber_command;
    // char *input_command (char *dest, const char *src)
};

struct renderstate_t {
	int offset;
	int command_length;
	int keys_pressed;
	char output;
	char answer[100];
	bool flag;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);
bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

