---
title: "gnumach"
source: "https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
GNU Mach is the microkernel upon which a GNU Hurd system is based. It provides an Inter Process Communication (IPC) mechanism that the Hurd uses to define interfaces for implementing in a distributed multi-server fashion the services a traditional operating system kernel provides.

It is maintained by the Hurd developers for the GNU project and remains compatible with [Mach](https://www.gnu.org/software/hurd/microkernel/mach.html) 3.0.

The majority of GNU Mach's [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel%2Fmach%2Fgnumach&page=device_driver)device driver s are from Linux 2.0. They were added using glue code, i.e., a Linux [emulation](https://www.gnu.org/software/hurd/emulation.html) layer in Mach.

GNU Mach runs on x86 machines. See the [hardware compatibility list](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/hardware_compatibility_list.html) and information about [ports](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/ports.html) to other architectures.

The latest release is [GNU Mach 1.8](https://www.gnu.org/software/hurd/news/2016-12-18-releases.html).

## Advantages of GNU Mach

GNU Mach is not the most advanced [microkernel](https://www.gnu.org/software/hurd/microkernel.html) known to the planet, nor is it the fastest or smallest, but it has a rich set of [interface](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/interface.html) s and some features which make it useful as the base of the [Hurd](https://www.gnu.org/software/hurd/hurd.html) system.

- **it's free software**
	Anybody can use, modify, and redistribute it under the terms of the[?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel%2Fmach%2Fgnumach&page=gpl)GNU General Public License (GPL).
- **it's built to survive**
	As a [microkernel](https://www.gnu.org/software/hurd/microkernel.html), GNU Mach doesn't implement a lot of the features commonly found in an operating system, but only the bare minimum that is required to implement a full operating system on top of it. This means that a lot of the operating system code is maintained outside of GNU Mach, and while this code may go through a complete redesign, the code of the microkernel can remain comparatively stable.
- **it's scalable**
	Mach is particularly well suited for SMP and network cluster techniques. Thread support is provided at the kernel level, and the kernel itself takes advantage of that. Network transparency at the [IPC](https://www.gnu.org/software/hurd/microkernel/mach/ipc.html) level makes resources of the system available across machine boundaries (with NORMA IPC, currently not available in GNU Mach).
- **it exists**
	The Mach microkernel is real software that works Right Now. It is not a research or a proposal. You don't have to wait at all before you can start using and developing it. Mach has been used in many operating systems in the past, usually as the base for a single UNIX server. In the GNU system, Mach is the base of a functional multi-server operating system, the [Hurd](https://www.gnu.org/software/hurd/hurd.html).

## Booting

To actually use the kernel and boot the GNU operating system, you need a boot loader. Not all boot loaders are capable to boot the GNU system, you need one that supports the multiboot standard. The bootloader of the GNU system is [GNU GRUB](https://www.gnu.org/software/hurd/grub.html), which supports a broad range of operating systems including GNU/Hurd.

## Development

- [Reference Manual](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/reference_manual.html)
- [Building](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/building.html)
- [Debugging](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/debugging.html)
- [Profiling](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/profiling.html)
- [Boot Trace](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/boot_trace.html)
- [Memory Management](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/memory_management.html)
- [Continuation](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/continuation.html) s
- [Preemption](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/preemption.html)
- [Projects](https://www.gnu.org/software/hurd/microkernel/mach/gnumach/projects.html)
	- [Rules](https://www.gnu.org/software/hurd/rules.html)