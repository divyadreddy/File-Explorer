
#ifndef COMMONFUNCS_H
#define COMMONFUNCS_H

#include "include.h"
#include "data.h"
#include "cursor.cpp"

string cwd()
{
  char buff[BUFF_SIZE];
  if (getcwd(buff, BUFF_SIZE) == NULL)
  {
    cout << "Error in getcwd() : " << strerror(errno);
    exit(1);
  }
  string path(buff);
  return path;
}

void disable_nc_mode()
{
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
}

// tcgetattr(STDIN_FILENO, &old_termios);
// atexit(disable_nc_mode);
// struct termios term = old_termios;

void change_mode()
{
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  cout << term.c_lflag << endl;
  // term.c_lflag &= ~(ECHO | ICANON);
  term.c_lflag &= ~ICANON;
  term.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
  cout << "Error in changing mode : " << strerror(errno);
}

vector<string> read_folder(string path)
{
  cout << "inside";
  vector<string> content;
  DIR *dirp = opendir(path.c_str());
  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL)
  {
    content.push_back(dp->d_name);
  }
  closedir(dirp);
  sort(content.begin(), content.end());
  content_size = content.size();
  return content;
}

void update_screen_dim()
{
  ioctl(0, TIOCGWINSZ, (char *)&wsize);
  screen_len = wsize.ws_row;
  screen_width = wsize.ws_col;
  normal_screen_len = screen_len - command_mode_len - 2;
  command_screen_start = screen_len - command_mode_len;
}

void print_status_bar(string msg)
{
  int m_len = msg.length();
  string path = pwd;
  int p_len = path.length();
  cout << "\033[K";
  while (m_len + p_len > screen_width)
  {
    int i = 0;
    while (i < p_len && path[i] != '/')
    {
      i++;
    }
    if (i < p_len)
    {
      path = path.substr(i + 1, p_len - i - 1);
    }
    else
    {
      path = "";
    }
    p_len = path.length();
  }
  cout << msg << path;
}

void status_bar(int mode)
{
  cursor_position(0, screen_len);
  switch (mode)
  {
  case 1:
    print_status_bar("Normal Mode : ");
    cursor_position(1, 1);
    break;
  case 2:
    print_status_bar("Command Mode : ");
    break;
  }
}

void append_history(string path)
{
  if (hist_ind > -1 && history[hist_ind] != path)
  {
    history.resize(hist_ind + 1);
    history.push_back(pwd);
    hist_ind++;
  }
  else if (hist_ind == -1)
  {
    history.push_back(pwd);
    hist_ind++;
  }
}

#endif
