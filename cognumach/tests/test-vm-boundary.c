/* Simple VM boundary test: allocate, touch, deallocate */

#include <testlib.h>
#include <mach/mach.h>

int main(int argc, char *argv[], int envc, char *envp[])
{
  kern_return_t kr;
  vm_address_t addr = 0;
  vm_size_t size = 1 << 20; /* 1 MiB */

  kr = vm_allocate(mach_task_self(), &addr, size, 1);
  ASSERT_RET(kr, "vm_allocate failed");

  /* Write something to the first and last page */
  volatile char *p = (volatile char*)addr;
  p[0] = 42;
  p[size - 1] = 7;

  kr = vm_deallocate(mach_task_self(), addr, size);
  ASSERT_RET(kr, "vm_deallocate failed");
  return 0;
}


