/*
 *  Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <testlib.h>

int main(int argc, char *argv[], int envc, char *envp[])
{
  /* Emit a few lines so that the log contains multiple timestamped entries */
  printf("timestamp test line 1\n");
  msleep(100);
  printf("timestamp test line 2\n");
  msleep(100);
  printf("timestamp test line 3\n");
  return 0;
}


