# GNU Mach GDB debugging script
# Load the kernel symbols
file /home/runner/work/cognu-mach/cognu-mach/test-gnumach-debug

# Connect to QEMU GDB server
target remote localhost:1234

# Set useful breakpoints for kernel debugging
# break startup.c:main if you want to debug early boot
# break kern/printf.c:printf for debugging output

# Display useful information
set disassembly-flavor intel
set print pretty on

# Show registers and next instructions
define debug-info
    info registers
    x/10i $pc
end

echo \n=== GNU Mach debugging session started ===\n
echo Use 'debug-info' to show registers and disassembly\n
echo Use 'continue' to start the kernel\n
echo Use 'break function_name' to set breakpoints\n
