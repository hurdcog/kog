---
title: "microkernel"
source: "https://www.gnu.org/software/hurd/microkernel.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
A *microkernel* is one kind of a [kernel](https://www.gnu.org/software/hurd/kernel.html) implementation.

[Liedtke](https://www.gnu.org/software/hurd/liedtke.html) explains in [On Microkernel Construction](http://l4ka.org/publications/paper.php?docid=642) (or from [TU Dresden](http://www.tud-os.org/papers_ps/jochen/Mikern.ps), or from [ACM](http://dl.acm.org/citation.cfm?id=224075)) that a microkernel attempts to minimize the mandatory part of the operating system by providing the minimal number of [mechanism](https://www.gnu.org/software/hurd/mechanism.html) s that maximize the flexibility of implementation (by imposing minimal [policy](https://www.gnu.org/software/hurd/policy.html)) while allowing the efficient implementation of the remainder of the system.

The idea of a microkernel as explained above was first explored by Per Brinch-Hansen in 1970 in [The Nucleus of a Multiprogramming System](http://brinch-hansen.net/papers/1970a.pdf).

An [introduction](http://www.cs.cornell.edu/Info/People/ulfar/ukernel/ukernel.html) by Úlfar Erlingsson and Athanasios Kyparlis (from 1996) to microkernel concepts.

[Research](https://www.gnu.org/software/hurd/microkernel/research.html).

[Multiserver Microkernel](https://www.gnu.org/software/hurd/faq/multiserver_microkernel.html).

[Microkernels for beginners](https://www.gnu.org/software/hurd/microkernel/for_beginners.html).

A 2002 article about [microkernel FUD](https://www.gnu.org/software/hurd/microkernel/fud.html) (Fear, Uncertainty, Doubt).

## Implementations

- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel&page=Hydra)Hydra
- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel&page=KeyKOS)KeyKOS
- [Mach](https://www.gnu.org/software/hurd/microkernel/mach.html) -- used by the GNU/Hurd
- [EROS](https://www.gnu.org/software/hurd/microkernel/eros.html)
- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=microkernel&page=CapROS)CapROS
- [Coyotos](https://www.gnu.org/software/hurd/microkernel/coyotos.html)
- [L4](https://www.gnu.org/software/hurd/microkernel/l4.html)
- [Barrelfish](https://www.gnu.org/software/hurd/microkernel/barrelfish.html)
- [Viengoos](https://www.gnu.org/software/hurd/microkernel/viengoos.html)
- [Genode](https://www.gnu.org/software/hurd/microkernel/genode.html)

License:

[GFDL 1.2+](https://www.gnu.org/software/hurd/#microkernel.license)

Last edited