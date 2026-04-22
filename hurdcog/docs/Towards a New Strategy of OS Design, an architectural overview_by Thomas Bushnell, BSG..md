---
title: "Towards a New Strategy of OS Design, an architectural overviewby Thomas Bushnell, BSG."
source: "https://www.gnu.org/software/hurd/hurd-paper.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
This article explains why FSF is developing a new operating system named the Hurd, which will be a foundation of the whole GNU system. The Hurd is built on top of CMU's Mach 3.0 kernel and uses Mach's virtual memory management and message-passing facilities. The GNU C Library will provide the Unix system call interface, and will call the Hurd for needed services it can't provide itself. The design and implementation of the Hurd is being lead by Michael Bushnell, with assistance from Richard Stallman, Roland McGrath, Jan Brittenson, and others.

## Part 1: A More Usable Approach to OS Design

The fundamental purpose of an operating system (OS) is to enable a variety of programs to share a single computer efficiently and productively. This demands memory protection, preemptively scheduled timesharing, coordinated access to I/O peripherals, and other services. In addition, an OS can allow several users to share a computer. In this case, efficiency demands services that protect users from harming each other, enable them to share without prior arrangement, and mediate access to physical devices.

On today's computer systems, programmers usually implement these goals through a large program called the kernel. Since this program must be accessible to all user programs, it is the natural place to add functionality to the system. Since the only model for process interaction is that of specific, individual services provided by the kernel, no one creates other places to add functionality. As time goes by, more and more is added to the kernel.

A traditional system allows users to add components to a kernel only if they both understand most of it and have a privileged status within the system. Testing new components requires a much more painful edit-compile-debug cycle than testing other programs. It cannot be done while others are using the system. Bugs usually cause fatal system crashes, further disrupting others' use of the system. The entire kernel is usually non-pageable. (There are systems with pageable kernels, but deciding what can be paged is difficult and error prone. Usually the mechanisms are complex, making them difficult to use even when adding simple extensions.)

Because of these restrictions, functionality which properly belongs **behind** the wall of a traditional kernel is usually left out of systems unless it is absolutely mandatory. Many good ideas, best done with an open/read/write interface cannot be implemented because of the problems inherent in the monolithic nature of a traditional system. Further, even among those with the endurance to implement new ideas, only those who are privileged users of their computers can do so. The software copyright system darkens the mire by preventing unlicensed people from even reading the kernel source.

Some systems have tried to address these difficulties. Smalltalk-80 and the Lisp Machine both represented one method of getting around the problem. System code is not distinguished from user code; all of the system is accessible to the user and can be changed as need be. Both systems were built around languages that facilitated such easy replacement and extension, and were moderately successful. But they both were fairly poor at insulating users and programs from each other, failing one of the principal goals of OS design.

Most projects that use the Mach 3.0 kernel carry on the hard-to-change tradition of OS design. The internal structure is different, but the same heavy barrier between user and system remains. The single-servers, while fairly easy to construct, inherit all the deficiencies of the monolithic kernels.

A multi-server divides the kernel functionality up into logical blocks with well-defined interfaces. Properly done, it is easier to make changes and add functionality. So most multi-server projects do somewhat better. Much more of the system is pageable. You can debug the system more easily. You can test new system components without interfering with other users. But the wall between user and system remains; no user can cross it without special privilege.

The GNU Hurd, by contrast, is designed to make the area of **system** code as limited as possible. Programs are required to communicate only with a few essential parts of the kernel; the rest of the system is replaceable dynamically. Users can use whatever parts of the remainder of the system they want, and can easily add components themselves for other users to take advantage of. No mutual trust need exist in advance for users to use each other's services, nor does the system become vulnerable by trusting the services of arbitrary users.

This has been done by identifying those system components which users **must** use in order to communicate with each other. One of these is responsible for identifying users' identities and is called the authentication server.In order to establish each other's identities, programs must communicate, each with an authentication server they trust. Another component establishes control over system components by the superuser, provides global bookkeeping operations, and is called the process server.

Not all user programs need to communicate with the process server; it is only necessary for programs which require its services. Likewise, the authentication server is only necessary for programs that wish to communicate their identity to another. None of the remaining services carry any special status; not the network implementation, the filesystems, the program execution mechanism (including setuid), or any others.

### The Translator Mechanism

The Hurd uses Mach ports primarily as methods for communicating between users and servers. (A Mach port is a communication point on a Mach task where messages are sent and received.) Each port implements a particular set of protocols, representing operations that can be undertaken on the underlying object represented by the port. Some of the protocols specified by the Hurd are the I/O protocol, used for generic I/O operations; the file protocol, used for filesystem operations; the socket protocol, used for network operations; and the process protocol, used for manipulating processes et al.

Most servers are accessed by opening files. Normally, when you open a file, you create a port associated with that file that is owned by the server that owns the directory containing the file. For example, a disk-based filesystem will normally serve a large number of ports, each of which represents an open file or directory. When a file is opened, the server creates a new port, associates it with the file, and returns the port to the calling program.

However, a file can have a translator associated with it. In this case, rather than return its own port which refers to the contents of the file, the server executes a translator program associated with that file. This translator is given a port to the actual contents of the file, and is then asked to return a port to the original user to complete the open operation.

This mechanism is used for `mount` by having a translator associated with each mount point. When a program opens the mount point, the translator (in this case, a program which understands the disk format of the mounted filesystem) is executed and returns a port to the program. After the translator is started, it need not be run again unless it dies; the parent filesystem retains a port to the translator to use in further requests.

The owner of a file can associate a translator with it without special permission. This means that any program can be specified as a translator. Obviously the system will not work properly if the translator does not implement the file protocol correctly. However, the Hurd is constructed so that the worst possible consequence is an interruptible hang.

One way to use translators is to access hierarchically structured data using the file protocol. For example, all the complexity of the user interface to the `ftp` program is removed. Users need only know that a particular directory represents FTP and can use all the standard file manipulation commands (e.g `ls` or `cp`) to access the remote system, rather than learning a new set. Similarly, a simple translator could ease the complexity of `tar` or `gzip`. (Such transparent access would have some added cost, but it would be convenient.)

### Generic Services

With translators, the filesystem can act as a rendezvous for interfaces which are not similar to files. Consider a service which implements some version of the X protocol, using Mach messages as an underlying transport. For each X display, a file can be created with the appropriate program as its translator. X clients would open that file. At that point, few file operations would be useful (read and write, for example, would be useless), but new operations (`XCreateWindow` or `XDrawText`) might become meaningful. In this case, the filesystem protocol is used only to manipulate characteristics of the node used for the rendezvous. The node need not support I/O operations, though it should reply to any such messages with a `message_not_understood` return code.

This translator technique is used to contact most of the services in the Hurd that are not structured like hierarchical filesystems. For example, the password server, which hands out authorization tags in exchange for passwords, is contacted this way. Network protocol servers are also contacted in this fashion. Roland McGrath thought up this use of translators.

### Clever Filesystem Pictures

In the Hurd, translators can also be used to present a filesystem-like view of another part of the filesystem, with some semantics changed. For example, it would be nice to have a filesystem that cannot itself be changed, but nonetheless records changed versions of its files elsewhere. (This could be useful for source code management.)

The Hurd will have a translator which creates a directory which is a conceptual union of other directories, with collision resolution rules of various sorts. This can be used to present a single directory to users that contains all the programs they would want to execute. There are other useful variations on this theme.

### What The User Can Do

No translator gains extra privilege by virtue of being hooked into the filesystem. Translators run with the uid of the owner of the file being translated, and can only be set or changed by that owner. The I/O and filesystem protocols are carefully designed to allow their use by mutually untrusting clients and servers. Indeed, translators are just ordinary programs. The GNU C library has a variety of facilities to make common sorts of translators easier to write.

Some translators may need special privileges, such as the password server or translators which allow setuid execution. These translators could be run by anyone, but only if they are set on a root-owned node would they be able to provide all their services successfully. This is analogous to letting any user call the `reboot` system call, but only honoring it if that user is root.

### Why This Is So Different

What this design provides is completely novel to the Unix world. Until now, OSs have kept huge portions of their functionality in the realm of system code, thus preventing its modification and extension except in extreme need. Users cannot replace parts of the system in their programs no matter how much easier that would make their task, and system managers are loath to install random tweaks off the net into their kernels.

In the Hurd, users can change almost all of the things that are decided for them in advance by traditional systems. In combination with the tremendous control given by the Mach kernel over task address spaces and properties, the Hurd provides a system in which users will, for the first time, be able to replace parts of the system they dislike, without disrupting other users.

Most Mach-based OSs to date have mostly implemented a wider set of the **same old** Unix semantics in a new environment. In contrast, GNU is extending those semantics to allow users to improve, bypass, or replace them.

## Part 2: A Look at Some of the Hurd's Beasts

### The Authentication Server

One of the Hurd's more central servers is the authentication server. Each port to this server identifies a user and is associated by this server with an id block. Each id block contains sets of user and group ids. Either set may be empty. This server is not the same as the password server referred to above.

The authentication server exports three services. First, it provides simple boolean operations on authentication ports: given two authentication ports, this server will provide a third port representing the union of the two sets of uids and gids. Second, this server allows any user with a uid of zero to create an arbitrary authentication port. Finally, this server provides RPCs (Remote Procedure Calls between different programs and possibly different hosts) which allow mutually untrusting clients and servers to establish their identities and pass initial information on each other. This is crucial to the security of the filesystem and I/O protocols.

Any user could write a program which implements the authentication protocol; this does not violate the system's security. When a service needs to authenticate a user, it communicates with its trusted authentication server. If that user is using a different authentication server, the transaction will fail and the server can refuse to communicate further. Because, in effect, this forces all programs on the system to use the same authentication server, we have designed its interface to make any safe operation possible, and to include no extraneous operations. (This is why there is a separate password server.)

### The Process Server

The process server acts as an information categorization repository. There are four main services supported by this server. First, the process server keeps track of generic host-level information not handled by the Mach kernel. For example, the hostname, the hostid, and the system version are maintained by the process server. Second, this server maintains the Posix notions of sessions and process groups, to help out programs that wish to use Posix features.

Third, the process server maintains a one-to-one mapping between Mach tasks and Hurd processes. Every task is assigned a pid. Processes can register a message port with this server, which can then be given out to any program which requests it. This server makes no attempt to keep these message ports private, so user programs are expected to implement whatever security they need themselves. (The GNU C Library provides convenient functions for all this.) Processes can tell the process server their current \`argv' and \`envp' values; this server will then provide, on request, these vectors of arguments and environment. This is useful for writing `ps` -like programs and also makes it easier to hide or change this information. None of these features are mandatory. Programs are free to disregard all of this and never register themselves with the process server at all. They will, however, still have a pid assigned.

Finally, the process server implements process collections, which are used to collect a number of process message ports at the same time. Also, facilities are provided for converting between pids, process server ports, and Mach task ports, while ensuring the security of the ports managed.

It is important to stress that the process server is optional. Because of restrictions in Mach, programs must run as root in order to identify all the tasks in the system. But given that, multiple process servers could co-exist, each with their own clients, giving their own model of the universe. Those process server features which do not require root privileges to be implemented could be done as per-user servers. The user's hands are not tied.

### Transparent FTP

Transparent FTP is an intriguing idea whose time has come. The popular `ange-ftp` package available for GNU Emacs makes access to FTP files virtually transparent to all the Emacs file manipulation functions. Transparent FTP does the same thing, but in a system wide fashion. This server is not yet written; the details remain to be fleshed out, and will doubtless change with experience \[Note: since the writing of this, ftpfs was implemented and works as described here\]

In a BSD kernel, a transparent FTP filesystem would be no harder to write than in the Hurd. But mention the idea to a BSD kernel hacker, and the response is that \`\`such a thing doesn't belong in the kernel''. In a sense, this is correct. It violates all the layering principles of such systems to place such things in the kernel. The unfortunate side effect, however, is that the design methodology (which is based on preventing users from changing things they don't like) is being used to prevent system designers from making things better. (Recent BSD kernels make it possible to write a user program that provides transparent FTP. An example is `alex`, but it needs to run with full root privileges.)

In the Hurd, there are no obstacles to doing transparent FTP. A translator will be provided for the node `/ftp`. The contents of `/ftp` will probably not be directly listable, though further subdirectories will be. There will be a variety of possible formats. For example, to access files on uunet, one could `  cd /ftp/ftp.uu.net:anonymous:mib@gnu.  `Or to access files on a remote account, one might `  cd /ftp/gnu.org:mib:passwd.  `Parts of this command could be left out and the transparent FTP program would read them from a user's`.netrc` file. In the last case, one might just `  cd /ftp/gnu.org;  `when the rest of the data is already in`.netrc`.

There is no need to do a `cd` first--use any file command. To find out about RFC 1097 (the Telnet Subliminal Message Option), just type `  more /ftp/ftp.uu.net/inet/rfc/rfc1097.  `A copy command to a local disk could be used if the RFC would be read frequently.

### Filesystems

Ordinary filesystems are also being implemented. The initial release of the Hurd will contain a filesystem upwardly compatible with the BSD 4.4 Fast File System. In addition to the ordinary semantics, it will provide means to record translators, offer thirty-two bit user ids and group ids, and supply a new id per file, called the author of the file, which can be set by the owner arbitrarily. In addition, because users in the Hurd can have multiple uids (or even none), there is an additional set of permission bits providing access control for unknown user (no uids) as distinct from known but arbitrary user (some uids: the existing world category of file permissions).

The Network File System protocol will be implemented using 4.4 BSD as a starting point. A log-structured filesystem will also be implemented using the same ideas as in Sprite, but probably not the same format. A GNU network file protocol may be designed in time, or NFS may be extended to remove its deficiencies. There will also be various \`\`little'' filesystems, such as the MS-DOS filesystem, to help people move files between GNU and other OSs.

### Terminals

An I/O server will provide the terminal semantics of Posix. The GNU C Library has features for keeping track of the controlling terminal and for arranging to have proper job control signals sent at the proper times, as well as features for obeying keyboard and hangup signals.

Programs will be able to insert a terminal driver into communications channels in a variety of ways. Servers like `rlogind` will be able to insert the terminal protocol onto their network communication port. Pseudo-terminals will not be necessary, though they will be provided for backward compatibility with older programs. No programs in GNU will depend on them.

Nothing about a terminal driver is forced upon users. A terminal driver allows a user to get at the underlying communications channel easily, to bypass itself on an as-needed basis or altogether, or to substitute a different terminal driver-like program. In the last case, provided the alternate program implements the necessary interfaces, it will be used by the C Library exactly as if it were the ordinary terminal driver.

Because of this flexibility, the original terminal driver will not provide complex line editing features, restricting itself to the behavior found in Posix and BSD. In time, there will be a `readline` -based terminal driver, which will provide complex line-editing features for those users who want them.

The terminal driver will probably not provide good support for the high-volume, rapid data transmission required by UUCP or SLIP. Those programs do not need any of its features. Instead they will be using the underlying Mach device ports for terminals, which support moving large amounts of data efficiently.

### Executing Programs

The implementation of the `execve` call is spread across three programs. The library marshals the argument and environment vectors. It then sends a message to the file server that holds the file to be executed. The file server checks execute permissions and makes whatever changes it desires in the exec call. For example, if the file is marked setuid and the fileserver has the ability, it will change the user identification of the new image. The file server also decides if programs which had access to the old task should continue to have access to the new task. If the file server is augmenting permissions, or executing an unreadable image, then the exec needs to take place in a new Mach task to maintain security.

After deciding the policy associated with the new image, the filesystem calls the exec server to load the task. This server, using the BFD (Binary File Descriptor) library, loads the image. BFD supports a large number of object file formats; almost any supported format will be executable. This server also handles scripts starting with `#!`, running them through the indicated program.

The standard exec server also looks at the environment of the new image; if it contains a variable `EXECSERVERS` then it uses the programs specified there as exec servers instead of the system default. (This is, of course, not done for execs that the file server has requested be kept secure.)

The new image starts running in the GNU C Library, which sends a message to the exec server to get the arguments, environment, umask, current directory, etc. None of this additional state is special to the file or exec servers; if programs wish, they can use it in a different manner than the Library.

### New Processes

The `fork` call is implemented almost entirely in the GNU C Library. The new task is created by Mach kernel calls. The C Library arranges to have its image inherited properly. The new task is registered with the process server (though this is not mandatory). The C Library provides vectors of functions to be called at fork time: one vector to be called before the fork, one after in the parent, and one after in the child. (These features should not be used to replace the normal fork-calling sequence; it is intended for libraries which need to close ports or clean up before a fork occurs.) The C library will implement both fork calls specified by the draft Posix.4a (the proposed standard dealing with the threads extension to the real-time extension).

Nothing forces the user to create new tasks this way. If a program wants to use almost the normal fork, but with some special characteristics, then it can do so. Hooks will be provided by the C Library, or the function can even be completely replaced. None of this is possible in a traditional Unix system.

### Asynchronous Messages

As mentioned above, the process server maintains a message port for each task registered with it. These ports are public, and are used to send asynchronous messages to the task. Signals, for example, are sent to the message port. The signal message also provides a port as an indication that the sender should be trusted to send the signal. The GNU C Library lists a variety of ports in a table, each of which identifies a set of signals that can be sent by anyone who possesses that port. For example, if the user possesses the task's kernel port, it is allowed to send any signal. If the user possesses a special terminal id port, it is allowed to send the keyboard and hangup signals. Users can add arbitrary new entries into the C library's signal permissions table.

When a process's process group changes, the process server will send it a message indicating the new process group. In this case, the process server proves its authority by providing the task's kernel port.

The C library also has messages to add and delete uids currently used by the process. If new uids are sent to the program, the library adds them to its current set, and then exchanges messages with all the I/O servers it knows about, proving to them its new authorization. Similarly, a message can delete uids. In the latter case, the caller must provide the process's task port. (You can't harm a process by giving it extra permission, but you can harm it by taking permission away.) The Hurd will provide user programs to send these messages to processes. For example, the `su` command will be able to cause all the programs in your current login session, to gain a new uid, rather than spawn a subshell.

The C library will allow programs to add asynchronous messages they wish to recognize, as well as prevent recognition of the standard set.

### Making It Look Like Unix

The C Library will implement all of the calls from BSD and Posix as well as some obvious extensions to them. This enables users to replace those calls they dislike or bypass them entirely, whereas in Unix the calls must be used \`\`as they come'' with no alternatives possible.

In some environments binary compatibility will also be supported. This works by building a special version of the library which is then loaded somewhere in the address space of the process. (For example, on a VAX, it would be tucked in above the stack.) A feature of Mach, called system call redirection, is then used to trap Unix system calls and turn them into jumps into this special version of the library. (On almost all machines, the cost of such a redirection is very small; this is a highly optimized path in Mach. On a 386 it's about two dozen instructions. This is little worse than a simple procedure call.)

Many features of Unix, such as signal masks and vectors, are handled completely by the library. This makes such features significantly cheaper than in Unix. It is now reasonable to use `sigblock` extensively to protect critical sections, rather than seeking out some other, less expensive method.

### Network Protocols

The Hurd will have a library that will make it very easy to port 4.4 BSD protocol stacks into the Hurd. This will enable operation, virtually for free, of all the protocols supported by BSD. Currently, this includes the CCITT protocols, the TCP/IP protocols, the Xerox NS protocols, and the ISO protocols.

For optimal performance some work would be necessary to take advantage of Hurd features that provide for very high speed I/O. For most protocols this will require some thought, but not too much time. The Hurd will run the TCP/IP protocols as efficiently as possible.

As an interesting example of the flexibility of the Hurd design, consider the case of IP trailers, used extensively in BSD for performance. While the Hurd will be willing to send and receive trailers, it will gain fairly little advantage in doing so because there is no requirement that data be copied and avoiding copies for page-aligned data is irrelevant.