#ifndef NORMALMODEDISPLAY_H
#define NORMALMODEDISPLAY_H

#include "include.h"
#include "data.h"
#include "cursor.cpp"
#include "common_functions.cpp"

void print_perm(mode_t perm)
{
  cout << ((S_ISDIR(perm)) ? 'd' : '-');
  cout << ((perm & S_IRUSR) ? 'r' : '-');
  cout << ((perm & S_IWUSR) ? 'w' : '-');
  cout << ((perm & S_IXUSR) ? 'x' : '-');
  cout << ((perm & S_IRGRP) ? 'r' : '-');
  cout << ((perm & S_IWGRP) ? 'w' : '-');
  cout << ((perm & S_IXGRP) ? 'x' : '-');
  cout << ((perm & S_IROTH) ? 'r' : '-');
  cout << ((perm & S_IWOTH) ? 'w' : '-');
  cout << ((perm & S_IXOTH) ? 'x' : '-');
}

string h_size(off_t b_size)
{
  int count = 0;
  string sz;
  long double s = static_cast<long double>(b_size);
  while (s > 999)
  {
    s /= 1024;
    count++;
  }
  if (s >= 10)
  {
    s = round(s);
  }
  else
  {
    s = round(s * 10);
    s /= 10;
  }
  sz = to_string(s);
  int digits = 3;
  if (sz[2] == '.')
    digits--;
  sz = sz.substr(0, digits);
  string range = "BKMGT";
  sz += range[count];
  return sz;
}

void display_folder(vector<string> content, string parent, int page, int mode = 1)
{
  erase_normal_mode(); 
  status_bar(mode);
  cursor_position(1, 1);
  int l_size = content.size();
  string path;
  struct stat buff; 
  time_t ct;
  struct winsize size;
  ioctl(0, TIOCGWINSZ, (char *)&size);
  screen_len = size.ws_row;
  normal_screen_len = screen_len - command_mode_len - 2;
  int i = (page - 1) * normal_screen_len;
  if (normal_screen_len < (l_size - (page - 1) * normal_screen_len))
  {
    l_size = normal_screen_len * page;
  }

  for (; i < l_size; i++)
  {
    path = parent;
    path.append("/");
    path.append(content[i]);
    stat(path.c_str(), &buff);
    string mtime = ctime(&buff.st_mtime);
    print_perm(buff.st_mode);
    ssize_t xattr = listxattr(path.c_str(), NULL, 0, XATTR_NOFOLLOW);
    char ext_attr;
    if (xattr < 0)
      xattr = 0;
    if (xattr > 0)
      ext_attr = '@';
    else
      ext_attr = ' ';
    cout << ext_attr;
    struct passwd *uid;
    struct group *gid;
    if (!(uid = getpwuid(buff.st_uid)))
    {
      cout << path << " ";
      cout << "Error in retirieving userid : " << strerror(errno) << endl;
      return;
    }
    if (!(gid = getgrgid(buff.st_gid)))
    {
      cout << path << " ";
      cout << "Error in retirieving groupid : " << strerror(errno) << endl;
      return;
    }
    cout << " " << left << setw(3) << buff.st_nlink << " " << setw(7) << uid->pw_name << " " << setw(7) << gid->gr_name << " " << right << setw(4) << h_size(buff.st_size) << " " << mtime.substr(0, mtime.length() - 1) << " " << content[i] << endl;
  }
  string curs_pos = "\033[";
  curs_pos.append(to_string(screen_len - command_mode_len - 1));
  curs_pos.append(";1H");
  cout << curs_pos;
  for (int i = 0; i < size.ws_col; i++)
  {
    cout << "-";
  }
  if (mode == 1)
    cout << "\033[1;63H";
}

#endif
