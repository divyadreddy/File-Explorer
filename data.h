#ifndef DATA_H
#define DATA_H

#include "include.h"

#define BUFF_SIZE 1024

string HOME, pwd;
vector<string> content;
vector<string> history;

struct winsize wsize;
struct termios old_termios;

int cursor = 0; 
int hist_ind = -1;
int content_size = 0;

const int command_mode_len = 8;
int normal_screen_len, screen_width, screen_len;
int command_screen_start;

#endif
