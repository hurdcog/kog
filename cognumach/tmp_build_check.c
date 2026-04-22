#include "i386/i386/vm_param.h"

int kernel_map_base_nonzero = (KERNEL_MAP_BASE != 0);
unsigned long long vm_min_kernel_address_copy = (unsigned long long) VM_MIN_KERNEL_ADDRESS;

int main(void) {
    return (kernel_map_base_nonzero && (vm_min_kernel_address_copy != 0)) ? 0 : 1;
}