#include "include.h"
#include "data.h"
#include "common_functions.cpp"
#include "cursor.cpp"
#include "normal_mode_display.cpp"
#include "normal_mode.cpp"
#include "command_mode.cpp"

int main(){
  pwd = cwd();
  HOME = pwd;
  change_mode();
  while(true){
    normal_mode();
    command_mode();
  }
  return 0;
}
