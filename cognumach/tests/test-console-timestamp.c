/*
 *  Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the program ; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <testlib.h>

int main(int argc, char *argv[], int envc, char *envp[])
{
  printf("=== Enhanced Console Timestamp Feature Test ===\n");
  
  /* Test that timestamps are enabled by default */
  ASSERT(console_timestamp_is_enabled(), "Timestamps should be enabled by default");
  
  /* Test default format */
  ASSERT(console_timestamp_get_format() == TIMESTAMP_FORMAT_RELATIVE, 
         "Default format should be TIMESTAMP_FORMAT_RELATIVE");
  
  /* Test basic printf with timestamps */
  printf("Testing basic message output\n");
  printf("Multiple line output:\n");
  printf("Line 1\n");
  printf("Line 2\n");
  printf("Line 3\n");
  
  /* Test different timestamp formats */
  printf("Testing TIMESTAMP_FORMAT_SIMPLE...\n");
  console_timestamp_set_format(TIMESTAMP_FORMAT_SIMPLE);
  ASSERT(console_timestamp_get_format() == TIMESTAMP_FORMAT_SIMPLE,
         "Format should be set to SIMPLE");
  printf("Message with simple timestamp format\n");
  
  printf("Testing TIMESTAMP_FORMAT_PRECISE...\n");
  console_timestamp_set_format(TIMESTAMP_FORMAT_PRECISE);
  ASSERT(console_timestamp_get_format() == TIMESTAMP_FORMAT_PRECISE,
         "Format should be set to PRECISE");
  printf("Message with precise timestamp format (microseconds)\n");
  
  printf("Testing TIMESTAMP_FORMAT_UPTIME...\n");
  console_timestamp_set_format(TIMESTAMP_FORMAT_UPTIME);
  ASSERT(console_timestamp_get_format() == TIMESTAMP_FORMAT_UPTIME,
         "Format should be set to UPTIME");
  printf("Message with absolute uptime format\n");
  
  /* Reset to default format */
  console_timestamp_set_format(TIMESTAMP_FORMAT_RELATIVE);
  printf("Reset to default format\n");
  
  /* Test timestamp disable/enable functionality */
  printf("Disabling timestamps...\n");
  console_timestamp_enable(FALSE);
  ASSERT(!console_timestamp_is_enabled(), "Timestamps should be disabled");
  
  printf("This message should have no timestamp\n");
  printf("Neither should this one\n");
  
  printf("Re-enabling timestamps...\n");
  console_timestamp_enable(TRUE);
  ASSERT(console_timestamp_is_enabled(), "Timestamps should be re-enabled");
  
  printf("This message should have timestamps again\n");
  printf("And so should this one\n");
  
  /* Test boot time retrieval */
  time_value64_t boot_time;
  console_timestamp_get_boot_time(&boot_time);
  printf("Boot time recorded: %d.%09d seconds\n", 
         (int)boot_time.seconds, (int)boot_time.nanoseconds);
  
  /* Test mixed output */
  printf("Testing mixed content: ");
  printf("same line continuation\n");
  
  /* Note about boot parameters (for documentation) */
  printf("Boot parameters supported:\n");
  printf("  notimestamps - disable completely\n");  
  printf("  console_timestamps=off/on - explicit control\n");
  printf("  timestamp_format=simple/precise/uptime - format selection\n");
  
  printf("%s: %s\n", TEST_SUCCESS_MARKER, "Enhanced console timestamp test completed successfully");
  
  return 0;
}