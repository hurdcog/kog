/* VM negative tests: exercise failure paths without crashing */

#include <testlib.h>
#include <mach/mach.h>

int main(int argc, char *argv[], int envc, char *envp[])
{
  kern_return_t kr;
  vm_address_t addr = 0;
  vm_size_t size = 1 << 20; /* 1 MiB */

  /* 1) Deallocating an unmapped region should fail */
  kr = vm_deallocate(mach_task_self(), 0x400000, size);
  if (kr == KERN_SUCCESS) {
    ASSERT_RET(KERN_INVALID_ADDRESS, "vm_deallocate unexpectedly succeeded on unmapped region");
  }

  /* 2) Map a region, then deallocate a misaligned partial subrange -> expect error */
  addr = 0;
  kr = vm_allocate(mach_task_self(), &addr, size, 1);
  ASSERT_RET(kr, "vm_allocate failed");

  /* Use a misaligned subrange to trigger KERN_INVALID_ARGUMENT */
  vm_address_t bad = addr + 12345;
  kr = vm_deallocate(mach_task_self(), bad, 4096);
  if (kr == KERN_SUCCESS) {
    /* Clean up then fail */
    (void) vm_deallocate(mach_task_self(), addr, size);
    ASSERT_RET(KERN_INVALID_ARGUMENT, "vm_deallocate unexpectedly succeeded on misaligned subrange");
  }

  /* Clean up full region */
  kr = vm_deallocate(mach_task_self(), addr, size);
  ASSERT_RET(kr, "vm_deallocate cleanup failed");

  return 0;
}


