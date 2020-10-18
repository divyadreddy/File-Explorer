#ifndef NM_H
#define NM_H
#include "include.h"
#include "data.h"
#include "common_functions.cpp"
#include "cursor.cpp"
#include "normal_mode_display.cpp"

int normal_mode()
{
  update_screen_dim();
  clear_screen();
  cursor_position(1, 1);
  content = read_folder(pwd);
  init_cursor();
  display_folder(content, pwd, 1);
  append_history(pwd);
  string prefix;
  char c;
  while (true)
  {
    struct winsize new_size;
    fflush(stdout);
    fflush(stdin);
    tcdrain(STDOUT_FILENO);
    tcdrain(STDIN_FILENO);
    ioctl(0, TIOCGWINSZ, (char *)&new_size);
    int new_screen_len = new_size.ws_row;
    int new_screen_width = new_size.ws_col;
    if (new_screen_len != screen_len || new_screen_width != screen_width)
    {
      update_screen_dim();
      display_folder(content, pwd, cursor / normal_screen_len + 1);
    }
    c = getchar(); // reading character
    if (c == 27)
    {
      //
      c = getchar();
      c = getchar();
      switch (c)
      {
      case 'A': // cursor up
        if (cursor % normal_screen_len == 0 && cursor != 0)
        {
          display_folder(content, pwd, cursor / normal_screen_len);
          string curs_pos = "\033[";
          curs_pos.append(to_string(normal_screen_len - 1));
          curs_pos.append("B");
          cout << curs_pos;
          cursor--;
        }
        else if (cursor != 0)
        {
          cout << "\033[1A";
          cursor--;
        }
        break;
      case 'B': // cursor down
        if ((cursor + 1) % normal_screen_len == 0 && cursor != content_size - 1)
        {
          display_folder(content, pwd, (cursor + 1) / normal_screen_len + 1);
          string curs_pos = "\033[";
          curs_pos.append(to_string(normal_screen_len));
          curs_pos.append("A");
          cout << curs_pos;
          cursor++;
        }
        else if (cursor != content_size - 1)
        {
          cout << "\033[1B";
          cursor++;
        }
        break;
      case 'D': // left - previously visited directory
        if (hist_ind > 0)
        {
          pwd = history[--hist_ind];
          init_cursor();
          content = read_folder(pwd);
          display_folder(content, pwd, 1);
        }
        break;
      case 'C': // right - next visited directory
        if (hist_ind + 1 < history.size())
        {
          pwd = history[++hist_ind];
          init_cursor();
          content = read_folder(pwd);
          display_folder(content, pwd, 1);
        }
        break;
      }
    }
    else if (c == 'k' || c == 'K') //page up
    {
      if (cursor >= normal_screen_len)
      {
        display_folder(content, pwd, (cursor / normal_screen_len));
        string curs_pos = "\033[";
        curs_pos.append(to_string(normal_screen_len - 2));
        curs_pos.append("A");
        cout << curs_pos;
        cursor = (cursor / normal_screen_len - 1) * normal_screen_len;
      }
    }
    else if (c == 'l' || c == 'L') // page down
    {
      if ((content_size - (cursor / normal_screen_len) * normal_screen_len) >= normal_screen_len)
      {
        display_folder(content, pwd, (cursor / normal_screen_len) + 2);
        string curs_pos = "\033[";
        curs_pos.append(to_string(normal_screen_len - 2));
        curs_pos.append("A");
        cout << curs_pos;
        cursor = (cursor / normal_screen_len + 1) * normal_screen_len;
      }
    }
    else if (c == 'h' || c == 'H')
    {
      cout << "\033[2J\033[1;1H";
      pwd = HOME;
      append_history(pwd);
      init_cursor();
      content = read_folder(pwd);
      display_folder(content, pwd, 1);
    }
    else if (c == 0x7f) //backspace
    {
      prefix = pwd;
      int i = prefix.length() - 1;
      while (i > -1 && prefix[i] != '/')
      {
        i--;
      }
      if (i > 0)
      {
        prefix = prefix.substr(0, i);
        pwd = prefix;
        content = read_folder(pwd);
        append_history(pwd);
      }
      cout << "\033[2J\033[1;1H";
      init_cursor();
      display_folder(content, pwd, 1);
    }
    else if (c == '\n')
    {
      
      if(cursor!=1){
        pwd.append("/");
        pwd.append(content[cursor]);
      }
      else{
        prefix = pwd;
        int i = prefix.length() - 1;
        while (i > -1 && prefix[i] != '/')
        {
          i--;
        }
        if(i > 0)
        {
          prefix = prefix.substr(0, i);
          pwd = prefix;
        }
      }
      struct stat buff;
      stat(pwd.c_str(), &buff);
      if (S_ISDIR(buff.st_mode))
      {
        append_history(pwd);
        content = read_folder(pwd);
        cout << "\033[2J\033[1;1H";
        init_cursor();
        display_folder(content, pwd, 1);
      }
      else
      {
        pid_t pid;
        if ((pid = fork()) == 0)
        {
          // if (S_ISREG(buff.st_mode))
          // {
          //   execl("/usr/bin/open", "open", pwd.c_str(), NULL);
          // }
          execl("/usr/bin/vi", "vi", pwd.c_str(), NULL);
        }
        else if (pid > 0)
        {
          waitpid(pid, NULL, 0);
          // To remove the file name from cwd
          int i = pwd.length();
          while (i > 0 && pwd[i] != '/')
          {
            i--;
          }
          pwd = pwd.substr(0, i);
        }
      }
    }
    else if (c == 58) // to change to command mode
    {
      return 1;
    }
    else
    {
      // cout << c << endl;
    }
  }
}

#endif
