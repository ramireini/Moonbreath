#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <stdio.h>
#include <stdlib.h>

// [path] [path to the file]
// [mode] [access mode] 
// 
// [returns a malloc'd char array, REMEMBER TO FREE]
char* io_read_file(const char *path, const char *mode)
{
  // open file
  FILE *file = fopen(path, mode);

  if(!file)
  {
    printf("Could not read file %s\n", path);
    return NULL;
  }

  // get file length
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  // allocate memory for file data
  char *buff = malloc(size + 1);

  // copy file contents into buffer
  fread(buff, size, 1, file);

  // null-terminate buffer
  buff[size] = '\0';

  // close file
  fclose(file);

  return buff;
}

#endif // UTIL_IO_H