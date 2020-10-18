#ifndef CMODE_H
#define CMODE_H

#include "include.h"
#include "data.h"
#include "common_functions.cpp"
#include "cursor.cpp"
#include "normal_mode_display.cpp"

void print_command(vector<string> command)
{
  int size = command.size();
  for (int i = 0; i < size; i++)
  {
    cout << command[i] << " ";
  }
}

string transform_path(string location)
{
  int i = 0, j = 0;
  int l = location.length();
  char path[1024];
  while (i < l)
  {
    if (location[i] == '\\') // to handle spaces in the path 
    {
      if (location[i + 1] == ' ')
      {
        path[j] = ' ';
        i++;
      }
      else
      {
        path[j] = location[i];
      }
      j++;
    }
    else if (location[i] == '~') // to handle home path
    {
      int k = 0;
      int hl = HOME.length();
      if (j == 0)
      {
        while (k < hl)
        {
          path[j++] = HOME[k++];
        }
      }
      else
      {
        while (k < hl)
        {
          path[j++] = HOME[k++];
        }
      }
    }
    else
    {
      path[j] = location[i];
      j++;
    }
    i++;
  }
  if (j > 0 && path[j - 1] == '/')
  {
    j--;
  }
  path[j] = '\0';
  return string(path);
}

bool path_exists(string path)
{
  struct stat info;
  if (stat(path.c_str(), &info) != 0)
  {
    return false;
  }
  return true;
}

bool is_dir(string path)
{
  struct stat info;
  stat(path.c_str(), &info);
  return S_ISDIR(info.st_mode);
}

bool delete_everything(string path)
{
  bool flag = true;
  if (is_dir(path))
  {
    vector<string> content = read_folder(path);
    int l = content.size();
    for (int i = 2; i < l; i++)
    {
      string subpath = path;
      subpath.append("/");
      subpath.append(content[i]);
      flag &= delete_everything(subpath);
    }
    if (rmdir(path.c_str()) != 0)
    {
      cout << "Unable to delete directory : " << path;
      flag &= false;
    }
  }
  else
  {
    if (remove(path.c_str()) != 0)
    {
      cout << "Unable to delete file : " << path;
      flag &= false;
    }
  }
  return flag;
}

bool same(string first, string second)
{
  struct stat info1;
  struct stat info2;
  if (stat(first.c_str(), &info1) != -1)
  {
    if (stat(second.c_str(), &info2) != -1)
    {
      if (info1.st_ino != info2.st_ino)
      {
        return false;
      }
      else
      {
        return true;
      }
    }
  }
  cout << "Error in checking if file/dir exists : " << first << " : " << strerror(errno);
  return true;
}

void file_exists_overwrite(string file)
{
  if (path_exists(file))
  {
    cout << "File already exists : " << file << " Do you want to override? ";
    char res;
    cin >> res;
    if (res == 'Y' || res == 'y')
    {
      if (remove(file.c_str()) != 0)
      {
        cout << "Unable to overwrite : " << file;
      }
    }
  }
}

bool copy_file(string source, string dest, bool check = false)
{
  bool identical = false;
  if (path_exists(dest))
    identical = same(source, dest);
  if (check && !identical)
  {
    file_exists_overwrite(dest);
  }
  else if (identical)
  {
    cout << "File to be copied exists in the same directory : " << dest;
  }
  struct stat info;
  if (stat(source.c_str(), &info) != 0)
  {
    cout << "Error in copying file : " << source << " : " << strerror(errno);
    return false;
  }
  int src_fd = open(source.c_str(), O_RDONLY);
  int dst_fd = open(dest.c_str(), O_CREAT | O_WRONLY, info.st_mode);
  char buff[4096];
  if (src_fd < 0 || dst_fd < 0)
  {
    cout << "Error in copying file(while opening) : " << source << " : " << strerror(errno);
    close(src_fd);
    close(dst_fd);
    return false;
  }
  int flag = true;
  while (1)
  {
    ssize_t size = read(src_fd, buff, 4096);
    if (size == -1)
    {
      cout << "Error in copying file(while reading) : " << source << " : " << strerror(errno);
      flag = false;
      break;
    }
    else if (size == 0)
      break;
    size = write(dst_fd, buff, size);
    if (size == -1)
    {
      cout << "Error in copying file(while writing) : " << source << " : " << strerror(errno);
      flag = false;
      break;
    }
  }
  close(src_fd);
  close(dst_fd);
  if (chown(dest.c_str(), info.st_uid, info.st_gid) == -1)
  {
    cout << "Error in copying file(while changing uid and gid) : " << source << " : " << strerror(errno);
    // flag= false;
  }
  return flag | true;
}

bool copy_dir(string source, string dest)
{
  bool flag = true;
  if (is_dir(source))
  {
    struct stat info;
    if (stat(source.c_str(), &info) != 0)
    {
      cout << "Error in copying directory : " << source << " : " << strerror(errno);
      return false;
    }
    if (mkdir(dest.c_str(), info.st_mode) == -1)
    {
      cout << "Error in creating directory : " << dest;
      return false;
    }
    vector<string> content = read_folder(source);
    int l = content.size();
    for (int i = 2; i < l; i++)
    {
      string subsrc = source;
      subsrc.append("/");
      subsrc.append(content[i]);
      string subdest = dest;
      subdest.append("/");
      subdest.append(content[i]);
      flag &= copy_dir(subsrc, subdest);
    }
    if (chown(dest.c_str(), info.st_uid, info.st_gid) == -1)
    {
      cout << "Error in copying directory(while changing uid and gid) : " << source << " : " << strerror(errno);
      // flag= false;
    }
  }
  else
  {
    flag &= copy_file(source, dest);
  }
  return flag;
}

void dir_exists_overwrite(string dir)
{
  if (path_exists(dir))
  {
    cout << "Directory already exists : " << dir << " Do you want to override? ";
    char res;
    cin >> res;
    if (res == 'Y' || res == 'y')
    {
      if (delete_everything(dir) != true)
      {
        cout << "Unable to overwrite : " << dir;
      }
    }
  }
}

void copy(vector<string> command)
{
  int l = command.size() - 1;
  string path = transform_path(command[l]);
  if (path_exists(path))
  {
    while (l > 1)
    {
      l--;
      string subpath;
      string suffix = transform_path(command[l]);
      if (suffix[0] != '/' && command[l][0] != '/')
      {
        subpath = pwd;
        subpath.append("/");
        subpath.append(suffix);
      }
      else
      {
        subpath = suffix;
      }
      int i = subpath.length();
      while (i > 0 && subpath[i] != '/')
      {
        i--;
      }
      if (i == 0)
      {
        i = subpath.length();
      }
      suffix = subpath.substr(i + 1, subpath.length() - i - 1);
      string subdest = path;
      subdest.append("/");
      subdest.append(suffix);
      if (path_exists(subpath))
      {
        if (is_dir(subpath))
        {
          bool identical = false;
          if (path_exists(subdest))
            identical = same(subpath, subdest);
          if (!identical)
          {
            dir_exists_overwrite(subdest);
            copy_dir(subpath, subdest);
          }
          else
          {
            cout << "Directory to be copied exists in the same directory : " << subdest;
          }
        }
        else
        {
          copy_file(subpath, subdest, true);
        }
      }
      else
      {
        cout << "Error in accessing(path does not exist) : " << subpath;
      }
    }
  }
  else
  {
    cout << "Error in accessing(path does not exist) : " << path;
  }
}

void move(vector<string> command)
{
  int l = command.size() - 1;
  string path = transform_path(command[l]);
  if (path_exists(path))
  {
    while (l > 1)
    {
      l--;
      string subpath;
      string suffix = transform_path(command[l]);
      if (suffix[0] != '/' && command[l][0] != '/')
      {
        subpath = pwd;
        subpath.append("/");
        subpath.append(suffix);
      }
      else
      {
        subpath = suffix;
      }
      int i = subpath.length();
      while (i > 0 && subpath[i] != '/')
      {
        i--;
      }
      if (i == 0)
      {
        i = subpath.length();
      }
      suffix = subpath.substr(i + 1, subpath.length() - i - 1);
      string subdest = path;
      subdest.append("/");
      subdest.append(suffix);
      if (path_exists(subpath))
      {
        if (is_dir(subpath))
        {
          bool identical = false;
          if (path_exists(subdest))
            identical = same(subpath, subdest);
          if (!identical)
          {
            dir_exists_overwrite(subdest);
            if (copy_dir(subpath, subdest))
            {
              delete_everything(subpath);
            }
            else
            {
              cout << "Incomplete move operation";
            }
          }
          else if (identical)
          {
            cout << "Directory to be copied exists in the same directory" << endl;
          }
        }
        else
        {
          if (copy_file(subpath, subdest))
          {
            delete_everything(subpath);
          }
          else
          {
            cout << "Incomplete move operation";
          }
        }
      }
      else
      {
        cout << "Error in accessing(path does not exist) : " << subpath;
      }
    }
  }
  else
  {
    cout << "Error in accessing(path does not exist) : " << path;
  }
}

void rename(vector<string> command)
{
  string old_name = transform_path(command[1]);
  string new_name = transform_path(command[2]);
  if (path_exists(old_name))
  {
    if (rename(old_name.c_str(), new_name.c_str()) == -1)
    {
      cout << "Error in renaming : " << old_name;
    }
  }
  else
  {
    cout << "Error in accessing(path does not exist) : " << old_name;
  }
}

void create_file(vector<string> command)
{
  int l = command.size() - 1;
  string prefix = transform_path(command[l]);
  // cout << prefix << endl;
  if (path_exists(prefix))
  {
    while (l > 1)
    {
      l--;

      string filename = command[l];
      string path = prefix;
      path.append("/");
      path.append(filename);
      path = transform_path(path);
      int fd = open(path.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (fd < 0)
      {
        cout << "Error in creating file : " << path;
      }
      else
      {
        close(fd);
      }
    }
  }
  else
  {
    cout << "Error in accessing : " << prefix;
  }
}

void create_dir(vector<string> command)
{
  int l = command.size() - 1;
  string prefix = transform_path(command[l]);
  if (path_exists(prefix))
  {
    while (l > 1)
    {
      l--;
      string path = prefix;
      string directory = command[l];
      path.append("/");
      path.append(directory);
      path = transform_path(path);
      if(path_exists(path)){
        cout << "Error in creating directory : Directory already exists " << path;
      } else if (mkdir(path.c_str(), 0755) == -1)
      {
        cout << "Error in creating directory : " << path;
      }
    }
  }
  else
  {
    cout << "Error in accessing(path does not exist) : " << prefix;
  }
}

void delete_file(vector<string> command)
{
  int l = command.size();
  while (l > 1)
  {
    l--;
    string path = transform_path(command[l]);
    if (path_exists(path))
    {
      if (!is_dir(path))
      {
        if (remove(path.c_str()) != 0)
        {
          cout << "Unable to delete : " << path;
        }
      }
      else
      {
        cout << "Error in deleting(not a file) : " << path;
      }
    }
    else
    {
      cout << "Error in accessing(path does not exist) : " << path;
    }
  }
}

void delete_dir(vector<string> command)
{
  int l = command.size();
  while (l > 1)
  {
    l--;
    string path = transform_path(command[l]);
    if (path_exists(path))
    {
      if (is_dir(path))
      {
        delete_everything(path);
      }
      else
      {
        cout << "Error in deleting(not a directory) : " << path;
      }
    }
    else
    {
      cout << "Error in accessing(path does not exist) : " << path;
    }
  }
}

void gotop(vector<string> command)
{
  string path = command[1];
  if (command[1][0] == '/')
  {
    path = transform_path(command[1]);
    if ((chdir(path.c_str())) != 0)
    {
      cout << "Couldn't change present working directory to : " << path;
    }
    else
    {
      pwd = path;
      content = read_folder(pwd);
      append_history(pwd);
    }
  }
  else
  {
    cout << "Enter an absolute path";
  }
}

bool find(string path, string name)
{
  bool flag = false;
  if (is_dir(path))
  {
    vector<string> content = read_folder(path);
    int l = content.size();
    for (int i = 2; i < l; i++)
    {
      if (content[i] == name)
        return true;
      string subpath = path;
      subpath.append("/");
      subpath.append(content[i]);
      flag |= find(subpath, name);
    }
  }
  return flag;
}

void search(vector<string> command)
{
  string path = cwd();
  string name = transform_path(command[1]);
  if (find(path, name))
  {
    cout << "True";
  }
  else
  {
    cout << "False";
  }
}

bool min_arg_count(int size, int req, bool equal = false)
{
  if ((equal && size == req) || (size >= req))
  {
    return true;
  }
  else
  {
    cout << "Incorrect number of arguments";
    return false;
  }
}

void execute(vector<string> command)
{
  if (command[0] == "copy")
  {
    if (min_arg_count(command.size(), 3))
      copy(command);
  }
  else if (command[0] == "move")
  {
    if (min_arg_count(command.size(), 3))
      move(command);
  }
  else if (command[0] == "rename")
  {
    if (min_arg_count(command.size(), 3))
      rename(command);
  }
  else if (command[0] == "create_file")
  {
    if (min_arg_count(command.size(), 3))
      create_file(command);
  }
  else if (command[0] == "create_dir")
  {
    if (min_arg_count(command.size(), 3))
      create_dir(command);
  }
  else if (command[0] == "delete_file")
  {
    if (min_arg_count(command.size(), 2))
      delete_file(command);
  }
  else if (command[0] == "delete_dir")
  {
    if (min_arg_count(command.size(), 2))
      delete_dir(command);
  }
  else if (command[0] == "goto")
  {
    if (min_arg_count(command.size(), 2, true))
      gotop(command);
  }
  else if (command[0] == "search")
  {
    if (min_arg_count(command.size(), 2, true))
      search(command);
  }
  else
  {
    cout << "Invalid command : " << command[0];
  }
}

void screen_resize() // to handle screen re-size
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
    clear_screen();
    cursor_position(1, 1);
    display_folder(content, pwd, cursor / normal_screen_len + 1, 2);
    // status_bar(2);
    cursor_position(0, command_screen_start);
  }
}


int command_mode() // starting of command mode
{
  if ((chdir(pwd.c_str())) != 0)
  {
    cout << "Couldn't change present working directory to : " << pwd;
  }
  update_screen_dim();
  cursor_position(0, command_screen_start);
  string input;
  char inp[1000];
  int cursor_y = 0;
  while (true)
  {
    screen_resize();
    if (command_mode_len <= cursor_y + 1)
    {
      cursor_position(0, command_screen_start);
      erase_screen_below();
      cursor_position(0, screen_len);
      status_bar(2);
      cursor_position(0, command_screen_start);
      cursor_y = 0;
    }
    char ch = getchar();
    if(screen_width>50)
    cursor_y += 2;
    else
    {
      cursor_y+=3;
    }
    
    int ind = 0;
    while (ch != '\n')
    {
      if (ch == '\e')
      {
        return 1;
      }
      if (ch == 127 && ind > 0)
      {
        ind--;
        cursor_left_erase();
      }
      else
      {
        inp[ind++] = ch;
      }
      cout << ch;
      ch = getchar();
    }
    if (ch == '\e')
    {
      return 1;
    }
    cout << ch;
    inp[ind] = '\0';
    string input(inp);

    if (input != "")
    {
      vector<string> command;
      int i = 0, l = input.length();
      while (i < l)
      {
        while (input[i] == ' ')
          i++;
        int begin = i;
        while (i < l)
        {
          if (input[i] == ' ' && (input[i - 1] != '\\'))
          {
            break;
          }
          i++;
        }
        int end = i;
        while (input[i] == ' ')
          i++;
        string comm = input.substr(begin, end - begin);
        if (comm != "")
          command.push_back(comm);
      }
      execute(command);
      cout << endl;
      vector<string> content = read_folder(pwd);
      display_folder(content, pwd, 1, 2);
      cursor_position(0, command_screen_start + cursor_y);
    }
  }
  return 1;
}

#endif
