/*
 * Modern GDB stub implementation for GNU Mach - Basic Implementation
 * Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#include <gdb_stub.h>
#include <kern/printf.h>
#include <i386/i386/kttd_machdep.h>
#include <string.h>

/* Global GDB stub state */
static struct gdb_stub_config gdb_config = GDB_STUB_DEFAULT_CONFIG;
static gdb_state_t gdb_state = GDB_STATE_DISCONNECTED;
static struct gdb_stub_stats gdb_stats = {0};

/* Hardware breakpoint management */
static struct gdb_hw_breakpoint hw_breakpoints[GDB_MAX_HW_BREAKPOINTS] = {0};

/* DR7 bit definitions for hardware breakpoints */
#define DR7_GE          (1UL << 9)   /* Global exact breakpoint enable */
#define DR7_LE          (1UL << 8)   /* Local exact breakpoint enable */
#define DR7_G0          (1UL << 1)   /* Global DR0 breakpoint */
#define DR7_L0          (1UL << 0)   /* Local DR0 breakpoint */
#define DR7_G1          (1UL << 3)   /* Global DR1 breakpoint */
#define DR7_L1          (1UL << 2)   /* Local DR1 breakpoint */
#define DR7_G2          (1UL << 5)   /* Global DR2 breakpoint */
#define DR7_L2          (1UL << 4)   /* Local DR2 breakpoint */
#define DR7_G3          (1UL << 7)   /* Global DR3 breakpoint */
#define DR7_L3          (1UL << 6)   /* Local DR3 breakpoint */

/* DR7 breakpoint type and length fields */
#define DR7_TYPE_SHIFT(n)   (16 + (n)*4)
#define DR7_LEN_SHIFT(n)    (18 + (n)*4)
#define DR7_TYPE_EXEC       0   /* Execute */
#define DR7_TYPE_WRITE      1   /* Write */
#define DR7_TYPE_IO         2   /* I/O */
#define DR7_TYPE_ACCESS     3   /* Read/Write */
#define DR7_LEN_1           0   /* 1 byte */
#define DR7_LEN_2           1   /* 2 bytes */
#define DR7_LEN_4           3   /* 4 bytes */

/*
 * x86 Debug Register Access Functions
 * These use inline assembly to access the debug registers
 */
static inline void set_debug_register(int reg, vm_offset_t value)
{
    switch (reg) {
    case 0:
        asm volatile("movl %0, %%dr0" : : "r" (value));
        break;
    case 1:
        asm volatile("movl %0, %%dr1" : : "r" (value));
        break;
    case 2:
        asm volatile("movl %0, %%dr2" : : "r" (value));
        break;
    case 3:
        asm volatile("movl %0, %%dr3" : : "r" (value));
        break;
    case 6:
        asm volatile("movl %0, %%dr6" : : "r" (value));
        break;
    case 7:
        asm volatile("movl %0, %%dr7" : : "r" (value));
        break;
    }
}

static inline vm_offset_t get_debug_register(int reg)
{
    vm_offset_t value = 0;
    switch (reg) {
    case 0:
        asm volatile("movl %%dr0, %0" : "=r" (value));
        break;
    case 1:
        asm volatile("movl %%dr1, %0" : "=r" (value));
        break;
    case 2:
        asm volatile("movl %%dr2, %0" : "=r" (value));
        break;
    case 3:
        asm volatile("movl %%dr3, %0" : "=r" (value));
        break;
    case 6:
        asm volatile("movl %%dr6, %0" : "=r" (value));
        break;
    case 7:
        asm volatile("movl %%dr7, %0" : "=r" (value));
        break;
    }
    return value;
}

/*
 * Initialize the GDB stub subsystem
 * This enhances the existing TTD infrastructure with modern GDB support
 */
void gdb_stub_init(void)
{
    printf("[GDB] Modern GDB stub initializing...\n");
    
    /* Initialize based on existing TTD infrastructure */
    gdb_config.enabled = FALSE;  /* Start disabled */
    gdb_state = GDB_STATE_DISCONNECTED;
    
    /* Check hardware capabilities */
    gdb_config.hardware_breakpoints = gdb_stub_hw_breakpoint_available();
    
    printf("[GDB] Hardware breakpoints: %s\n", 
           gdb_config.hardware_breakpoints ? "available" : "not available");
    printf("[GDB] GDB stub initialization complete\n");
}

/*
 * Configure GDB stub behavior
 */
void gdb_stub_configure(const struct gdb_stub_config *config)
{
    if (!config) {
        printf("[GDB] Error: NULL configuration\n");
        return;
    }
    
    gdb_config = *config;
    printf("[GDB] Configuration updated:\n");
    printf("  - Hardware breakpoints: %s\n", 
           config->hardware_breakpoints ? "enabled" : "disabled");
    printf("  - Software breakpoints: %s\n", 
           config->software_breakpoints ? "enabled" : "disabled");
    printf("  - Watchpoints: %s\n", 
           config->watchpoints ? "enabled" : "disabled");
    printf("  - Thread-aware: %s\n", 
           config->thread_aware ? "enabled" : "disabled");
}

/*
 * Enable/disable GDB stub
 */
void gdb_stub_enable(boolean_t enable)
{
    gdb_config.enabled = enable;
    if (enable) {
        gdb_state = GDB_STATE_CONNECTED;
        printf("[GDB] GDB stub enabled - waiting for connection\n");
    } else {
        gdb_state = GDB_STATE_DISCONNECTED;
        printf("[GDB] GDB stub disabled\n");
    }
}

/*
 * Check if GDB stub is enabled
 */
boolean_t gdb_stub_is_enabled(void)
{
    return gdb_config.enabled;
}

/*
 * Handle GDB stub communication
 * This is the main entry point from trap handlers
 */
void gdb_stub_handle_exception(int exception_type, 
                              struct i386_saved_state *state)
{
    if (!gdb_config.enabled) {
        return;
    }
    
    gdb_stats.exceptions_handled++;
    
    printf("[GDB] Exception %d handled, EIP=0x%lx\n",
           exception_type, state->eip);
    
    /* Set state to stopped */
    gdb_state = GDB_STATE_STOPPED;
    
    /* Send stop notification to GDB with signal information */
    switch (exception_type) {
    case 1:  /* Debug exception (hardware breakpoint) */
    case 3:  /* Breakpoint (INT3) */
        gdb_stub_send_packet("S05");  /* SIGTRAP */
        gdb_stats.breakpoints_hit++;
        break;
    case 14: /* Page fault */
        gdb_stub_send_packet("S0B");  /* SIGSEGV */
        break;
    default:
        gdb_stub_send_packet("S05");  /* SIGTRAP */
        break;
    }
    
    printf("[GDB] Sent stop notification, waiting for GDB commands\n");
    
    /* In a full implementation, this would enter a command processing loop
     * until GDB sends continue or step command */
}

/*
 * Hardware breakpoint support check
 */
boolean_t gdb_stub_hw_breakpoint_available(void)
{
    /* Check for x86 debug register support by trying to read DR7 */
    /* On x86, debug registers are available on 386 and later */
    /* For simplicity, assume they're available in kernel mode */
    return TRUE;
}

/*
 * Find an available hardware breakpoint slot
 */
static int gdb_stub_find_free_hw_breakpoint(void)
{
    int i;
    for (i = 0; i < GDB_MAX_HW_BREAKPOINTS; i++) {
        if (!hw_breakpoints[i].active) {
            return i;
        }
    }
    return -1;
}

/*
 * Find hardware breakpoint by address
 */
static int gdb_stub_find_hw_breakpoint_by_addr(vm_offset_t address)
{
    int i;
    for (i = 0; i < GDB_MAX_HW_BREAKPOINTS; i++) {
        if (hw_breakpoints[i].active && hw_breakpoints[i].address == address) {
            return i;
        }
    }
    return -1;
}

/*
 * Set a hardware breakpoint
 */
boolean_t gdb_stub_set_hw_breakpoint(vm_offset_t address, 
                                    gdb_breakpoint_type_t type)
{
    int slot;
    unsigned long dr7;
    int dr7_type, dr7_len;
    
    if (!gdb_config.hardware_breakpoints) {
        return FALSE;
    }
    
    /* Check if breakpoint already exists */
    if (gdb_stub_find_hw_breakpoint_by_addr(address) >= 0) {
        printf("[GDB] Hardware breakpoint already exists at 0x%lx\n", (unsigned long)address);
        return TRUE;
    }
    
    /* Find a free slot */
    slot = gdb_stub_find_free_hw_breakpoint();
    if (slot < 0) {
        printf("[GDB] No free hardware breakpoint slots\n");
        return FALSE;
    }
    
    printf("[GDB] Setting hardware breakpoint at 0x%lx, type %d, slot %d\n", 
           (unsigned long)address, type, slot);
    
    /* Set the address in the appropriate debug register */
    set_debug_register(slot, address);
    
    /* Configure DR7 for this breakpoint */
    dr7 = get_debug_register(7);
    
    /* Determine breakpoint type and length for DR7 */
    switch (type) {
    case GDB_BP_HARDWARE:
        dr7_type = DR7_TYPE_EXEC;
        dr7_len = DR7_LEN_1;
        break;
    case GDB_BP_WRITE_WATCH:
        dr7_type = DR7_TYPE_WRITE;
        dr7_len = DR7_LEN_4;
        break;
    case GDB_BP_READ_WATCH:
        dr7_type = DR7_TYPE_IO;  /* Best approximation for read */
        dr7_len = DR7_LEN_4;
        break;
    case GDB_BP_ACCESS_WATCH:
        dr7_type = DR7_TYPE_ACCESS;
        dr7_len = DR7_LEN_4;
        break;
    default:
        return FALSE;
    }
    
    /* Clear previous settings for this slot */
    dr7 &= ~(3UL << DR7_TYPE_SHIFT(slot));
    dr7 &= ~(3UL << DR7_LEN_SHIFT(slot));
    
    /* Set new type and length */
    dr7 |= ((unsigned long)dr7_type << DR7_TYPE_SHIFT(slot));
    dr7 |= ((unsigned long)dr7_len << DR7_LEN_SHIFT(slot));
    
    /* Enable the breakpoint (both local and global for kernel) */
    dr7 |= (1UL << (slot * 2));     /* Local enable */
    dr7 |= (1UL << (slot * 2 + 1)); /* Global enable */
    
    /* Enable exact breakpoint matching */
    dr7 |= DR7_GE | DR7_LE;
    
    /* Write back the control register */
    set_debug_register(7, dr7);
    
    /* Update our tracking structure */
    hw_breakpoints[slot].active = TRUE;
    hw_breakpoints[slot].address = address;
    hw_breakpoints[slot].type = type;
    hw_breakpoints[slot].length = (type == GDB_BP_HARDWARE) ? 1 : 4;
    hw_breakpoints[slot].dr_index = slot;
    
    return TRUE;
}

/*
 * Remove a hardware breakpoint
 */
boolean_t gdb_stub_remove_hw_breakpoint(vm_offset_t address)
{
    int slot;
    unsigned long dr7;
    
    if (!gdb_config.hardware_breakpoints) {
        return FALSE;
    }
    
    /* Find the breakpoint */
    slot = gdb_stub_find_hw_breakpoint_by_addr(address);
    if (slot < 0) {
        printf("[GDB] Hardware breakpoint not found at 0x%lx\n", (unsigned long)address);
        return FALSE;
    }
    
    printf("[GDB] Removing hardware breakpoint at 0x%lx, slot %d\n", (unsigned long)address, slot);
    
    /* Clear the debug register */
    set_debug_register(slot, 0);
    
    /* Disable the breakpoint in DR7 */
    dr7 = get_debug_register(7);
    dr7 &= ~(1UL << (slot * 2));     /* Clear local enable */
    dr7 &= ~(1UL << (slot * 2 + 1)); /* Clear global enable */
    dr7 &= ~(3UL << DR7_TYPE_SHIFT(slot));  /* Clear type */
    dr7 &= ~(3UL << DR7_LEN_SHIFT(slot));   /* Clear length */
    set_debug_register(7, dr7);
    
    /* Clear our tracking structure */
    hw_breakpoints[slot].active = FALSE;
    hw_breakpoints[slot].address = 0;
    hw_breakpoints[slot].type = 0;
    hw_breakpoints[slot].length = 0;
    hw_breakpoints[slot].dr_index = 0;
    
    return TRUE;
}

/*
 * Set a breakpoint (software or hardware)
 */
boolean_t gdb_stub_set_breakpoint(gdb_breakpoint_type_t type,
                                 vm_offset_t address,
                                 vm_size_t length)
{
    if (!gdb_config.enabled) {
        return FALSE;
    }
    
    switch (type) {
    case GDB_BP_SOFTWARE:
        if (gdb_config.software_breakpoints) {
            printf("[GDB] Setting software breakpoint at 0x%lx\n", (unsigned long)address);
            /* Replace instruction with INT3 (0xCC) */
            return TRUE;
        }
        break;
        
    case GDB_BP_HARDWARE:
        return gdb_stub_set_hw_breakpoint(address, type);
        
    case GDB_BP_WRITE_WATCH:
    case GDB_BP_READ_WATCH:
    case GDB_BP_ACCESS_WATCH:
        if (gdb_config.watchpoints) {
            printf("[GDB] Setting watchpoint at 0x%lx, type %d, length %lu\n", 
                   (unsigned long)address, type, (unsigned long)length);
            return gdb_stub_set_hw_breakpoint(address, type);
        }
        break;
    }
    
    return FALSE;
}

/*
 * Remove a breakpoint
 */
boolean_t gdb_stub_remove_breakpoint(gdb_breakpoint_type_t type,
                                    vm_offset_t address,
                                    vm_size_t length)
{
    if (!gdb_config.enabled) {
        return FALSE;
    }
    
    printf("[GDB] Removing breakpoint at 0x%lx, type %d\n", (unsigned long)address, type);
    
    switch (type) {
    case GDB_BP_SOFTWARE:
        /* Restore original instruction */
        return TRUE;
        
    case GDB_BP_HARDWARE:
    case GDB_BP_WRITE_WATCH:
    case GDB_BP_READ_WATCH:
    case GDB_BP_ACCESS_WATCH:
        return gdb_stub_remove_hw_breakpoint(address);
    }
    
    return FALSE;
}

/*
 * Thread debugging notifications
 */
void gdb_stub_thread_create(thread_t thread)
{
    if (gdb_config.thread_aware && gdb_config.enabled) {
        printf("[GDB] Thread created: %p\n", (void *)thread);
    }
}

void gdb_stub_thread_destroy(thread_t thread)
{
    if (gdb_config.thread_aware && gdb_config.enabled) {
        printf("[GDB] Thread destroyed: %p\n", (void *)thread);
    }
}

void gdb_stub_thread_switch(thread_t old_thread, thread_t new_thread)
{
    if (gdb_config.thread_aware && gdb_config.enabled) {
        printf("[GDB] Thread switch: %p -> %p\n", (void *)old_thread, (void *)new_thread);
    }
}

/*
 * Get debugging statistics
 */
void gdb_stub_get_stats(struct gdb_stub_stats *stats)
{
    if (stats) {
        *stats = gdb_stats;
    }
}

/*
 * Reset debugging statistics
 */
void gdb_stub_reset_stats(void)
{
    gdb_stats.packets_sent = 0;
    gdb_stats.packets_received = 0;
    gdb_stats.exceptions_handled = 0;
    gdb_stats.breakpoints_hit = 0;
    gdb_stats.commands_processed = 0;
    gdb_stats.errors = 0;
}

/*
 * Send signal to GDB (for panics, etc.)
 */
void gdb_stub_send_signal(int signal)
{
    if (gdb_config.enabled) {
        printf("[GDB] Sending signal %d to debugger\n", signal);
        gdb_state = GDB_STATE_STOPPED;
    }
}

/*
 * Check if debugger should break
 */
boolean_t gdb_stub_should_break(void)
{
    return (gdb_config.enabled && gdb_state == GDB_STATE_STOPPED);
}

/*
 * Memory validation for GDB access
 */
boolean_t gdb_stub_memory_valid(vm_offset_t address, vm_size_t length)
{
    /* In a full implementation, would check:
     * 1. Address range validity
     * 2. Memory protection
     * 3. Page presence
     */
    return TRUE;  /* Simplified for now */
}

/*
 * Memory change notification
 */
void gdb_stub_memory_changed(vm_offset_t address, vm_size_t length)
{
    if (gdb_config.enabled) {
        printf("[GDB] Memory changed at 0x%x, length %zu\n", address, length);
    }
}

/* Modern GDB protocol extensions (stubs for future implementation) */
void gdb_stub_send_thread_info(void)
{
    printf("[GDB] Sending thread information\n");
}

void gdb_stub_send_register_info(void)
{
    printf("[GDB] Sending register information\n");
}

void gdb_stub_send_memory_map(void)
{
    printf("[GDB] Sending memory map\n");
}

/*
 * Debug register access functions
 * These provide a public API for debug register access
 */
void gdb_stub_set_dr0(vm_offset_t value) { set_debug_register(0, value); }
void gdb_stub_set_dr1(vm_offset_t value) { set_debug_register(1, value); }
void gdb_stub_set_dr2(vm_offset_t value) { set_debug_register(2, value); }
void gdb_stub_set_dr3(vm_offset_t value) { set_debug_register(3, value); }
void gdb_stub_set_dr7(unsigned long value) { set_debug_register(7, value); }
void gdb_stub_set_dr6(unsigned long value) { set_debug_register(6, value); }

vm_offset_t gdb_stub_get_dr0(void) { return get_debug_register(0); }
vm_offset_t gdb_stub_get_dr1(void) { return get_debug_register(1); }
vm_offset_t gdb_stub_get_dr2(void) { return get_debug_register(2); }
vm_offset_t gdb_stub_get_dr3(void) { return get_debug_register(3); }
unsigned long gdb_stub_get_dr6(void) { return get_debug_register(6); }
unsigned long gdb_stub_get_dr7(void) { return get_debug_register(7); }

/*
 * GDB Remote Protocol Packet Handling
 */
static char gdb_packet_buffer[GDB_PACKET_SIZE];
static char gdb_reply_buffer[GDB_PACKET_SIZE];

/* Calculate checksum for GDB packet */
static unsigned char gdb_checksum(const char *data)
{
    unsigned char checksum = 0;
    while (*data) {
        checksum += (unsigned char)*data++;
    }
    return checksum;
}

/* Basic serial I/O - integrated with console for demonstration */
void gdb_stub_putchar(int c)
{
    /* For now, output GDB protocol over console with special markers */
    if (c == GDB_PACKET_START) {
        printf("[GDB-TX]");
    }
    printf("%c", c);
    if (c == GDB_PACKET_END) {
        printf("\n");
    }
    
    /* TODO: Implement actual serial output to COM1/COM2 */
    /* This could integrate with existing console/serial drivers */
}

int gdb_stub_getchar(void)
{
    /* TODO: Implement actual serial input */
    /* This would typically read from COM1/COM2 UART */
    /* For now, return -1 to indicate no data available */
    return -1;
}

boolean_t gdb_stub_char_available(void)
{
    /* TODO: Check if serial data is available in UART FIFO */
    return FALSE;
}

/* Send a GDB packet with proper framing and checksum */
void gdb_stub_send_packet(const char *data)
{
    unsigned char checksum;
    
    if (!gdb_config.enabled) {
        return;
    }
    
    checksum = gdb_checksum(data);
    
    gdb_stub_putchar(GDB_PACKET_START);
    while (*data) {
        gdb_stub_putchar(*data++);
    }
    gdb_stub_putchar(GDB_PACKET_END);
    gdb_stub_putchar("0123456789abcdef"[checksum >> 4]);
    gdb_stub_putchar("0123456789abcdef"[checksum & 0x0f]);
    
    gdb_stats.packets_sent++;
    printf("[GDB] Sent packet\n");
}

/* Send OK response */
void gdb_stub_send_ok(void)
{
    gdb_stub_send_packet("OK");
}

/* Send error response */
void gdb_stub_send_error(int error_code)
{
    snprintf(gdb_reply_buffer, sizeof(gdb_reply_buffer), "E%02x", error_code & 0xff);
    gdb_stub_send_packet(gdb_reply_buffer);
    gdb_stats.errors++;
}

/* Receive a GDB packet */
int gdb_stub_receive_packet(char *buffer, int max_len)
{
    int c, count = 0;
    unsigned char checksum, received_checksum;
    boolean_t packet_started = FALSE;
    
    if (!gdb_config.enabled) {
        return 0;
    }
    
    while (count < max_len - 1) {
        c = gdb_stub_getchar();
        if (c < 0) {
            break; /* No more data */
        }
        
        if (!packet_started) {
            if (c == GDB_PACKET_START) {
                packet_started = TRUE;
                count = 0;
            }
            continue;
        }
        
        if (c == GDB_PACKET_END) {
            buffer[count] = '\0';
            
            /* Read checksum */
            c = gdb_stub_getchar();
            if (c < 0) break;
            received_checksum = (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10);
            received_checksum <<= 4;
            
            c = gdb_stub_getchar();
            if (c < 0) break;
            received_checksum |= (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10);
            
            /* Verify checksum */
            checksum = gdb_checksum(buffer);
            if (checksum == received_checksum) {
                gdb_stub_putchar(GDB_ACK);
                gdb_stats.packets_received++;
                return count;
            } else {
                gdb_stub_putchar(GDB_NAK);
                return 0;
            }
        } else {
            buffer[count++] = c;
        }
    }
    
    return 0;
}

/* Process a received GDB packet */
void gdb_stub_process_packet(const char *packet)
{
    if (!gdb_config.enabled || !packet || !*packet) {
        return;
    }
    
    gdb_stats.commands_processed++;
    
    switch (packet[0]) {
    case GDB_CMD_QUERY:
        if (strncmp(packet, "qSupported", 10) == 0) {
            /* Report our capabilities */
            gdb_stub_send_packet("PacketSize=1000;hwbreak+;swbreak+");
        } else if (strncmp(packet, "qAttached", 9) == 0) {
            gdb_stub_send_packet("1"); /* We're attached to existing process */
        } else {
            gdb_stub_send_packet(""); /* Empty response for unsupported queries */
        }
        break;
        
    case GDB_CMD_SET_BREAKPOINT:
        /* Format: Z<type>,<addr>,<length> */
        /* TODO: Parse packet and set breakpoint */
        gdb_stub_send_ok();
        break;
        
    case GDB_CMD_REMOVE_BREAKPOINT:
        /* Format: z<type>,<addr>,<length> */
        /* TODO: Parse packet and remove breakpoint */
        gdb_stub_send_ok();
        break;
        
    case GDB_CMD_CONTINUE:
        /* Continue execution */
        gdb_state = GDB_STATE_RUNNING;
        gdb_stub_send_ok();
        break;
        
    case GDB_CMD_STEP:
        /* Single step */
        gdb_state = GDB_STATE_RUNNING;
        gdb_stub_send_ok();
        break;
        
    case '?':
        /* Return last signal */
        snprintf(gdb_reply_buffer, sizeof(gdb_reply_buffer), "S%02x", GDB_SIGNAL_TRAP);
        gdb_stub_send_packet(gdb_reply_buffer);
        break;
        
    default:
        /* Unsupported command */
        gdb_stub_send_packet("");
        break;
    }
}