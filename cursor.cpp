#ifndef CURSOR_H
#define CURSOR_H

#include "include.h"
#include "data.h"

void cursor_position(int x, int y)
{
  string curs_pos = "\033[";
  curs_pos.append(to_string(y));
  curs_pos.append(";");
  curs_pos.append(to_string(x));
  curs_pos.append("H");
  cout << curs_pos;
}

void clear_screen(){
  cout << "\033[2J";
}

void erase_normal_mode()
{
  cursor_position(0, normal_screen_len+1);
  cout<<"\033[1J";
}

void erase_screen_below(){
  cout<<"\033[0J";
}


void init_cursor()
{
  cursor = 0;
  cursor_position(63, 1);
}

void cursor_left_erase(){
  cout<<"\033[1D";
  cout<<"\033[0K";
}

#endif
