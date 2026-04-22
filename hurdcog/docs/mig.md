---
title: "mig"
source: "https://www.gnu.org/software/hurd/microkernel/mach/mig.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
The *Mach Interface Generator* (*MIG*) is an [IDL](https://www.gnu.org/software/hurd/idl.html) compiler. Based on an interface definition, it creates stub code to [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel%2Fmach%2Fmig&page=invoke)invoke object methods and to demultiplex incoming messages. These stub functions conveniently hide the details of Mach's [IPC](https://www.gnu.org/software/hurd/microkernel/mach/ipc.html) and [port](https://www.gnu.org/software/hurd/microkernel/mach/port.html) machinery and make it easy to implement and use Mach [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel%2Fmach%2Fmig&page=interface)interface s as [remote procedure calls (RPC)](https://www.gnu.org/software/hurd/microkernel/mach/rpc.html): by using the stub functions, the client programs can call remote procedures more or less like any other C function.

These functions encode arguments into [message](https://www.gnu.org/software/hurd/microkernel/mach/message.html) s' format (*marshalling*), wait for a result on a newly created [reply port](https://www.gnu.org/software/hurd/microkernel/mach/port.html), decode return arguments from the reply message (*demarshalling*, or *unmarshalling*) and pass them to the client program. Similar actions are provided in the skeletons that are linked to server programs. MIG allows very precise semantics to be specified about what the arguments are and how to be passed. It has its problems with [structured data](https://www.gnu.org/software/hurd/open_issues/mig_portable_rpc_declarations.html), however.

- [Documentation](https://www.gnu.org/software/hurd/microkernel/mach/mig/documentation.html)

## Implementations

- [GNU MIG](https://www.gnu.org/software/hurd/microkernel/mach/mig/gnu_mig.html)

License:

[GFDL 1.2+](https://www.gnu.org/software/hurd/microkernel/mach/#microkernel-mach-mig.license)

Last edited