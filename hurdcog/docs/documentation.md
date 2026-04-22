---
title: "documentation"
source: "https://www.gnu.org/software/hurd/hurd/documentation.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
## Introductory Material

- [What Is the GNU Hurd](https://www.gnu.org/software/hurd/hurd/what_is_the_gnu_hurd.html)
- [Advantages](https://www.gnu.org/software/hurd/advantages.html)
- [FAQ](https://www.gnu.org/software/hurd/faq.html)
- [*The Hurd and Linux*](https://www.gnu.org/software/hurd/hurd-and-linux.html), a comment by Richard Stallman.
- [*Towards a New Strategy of OS Design*](https://www.gnu.org/software/hurd/hurd-paper.html), an architectural overview by Thomas Bushnell, BSG, notably:
	- [The design](https://www.gnu.org/software/hurd/hurd-paper.html#design)
	- [Translators](https://www.gnu.org/software/hurd/hurd-paper.html#translator)
	- [The auth translator](https://www.gnu.org/software/hurd/hurd-paper.html#auth)
	- [The proc translator](https://www.gnu.org/software/hurd/hurd-paper.html#proc)
	- [The exec translator](https://www.gnu.org/software/hurd/hurd-paper.html#exec)
	- [The ftpfs translator](https://www.gnu.org/software/hurd/hurd-paper.html#ftpfs)
- [*The Hurd*](https://www.gnu.org/software/hurd/hurd-talk.html), a presentation by Marcus Brinkmann, notably:
	- [How to get a port?](https://www.gnu.org/software/hurd/hurd-talk.html#how)
	- [Pathname resolution example](https://www.gnu.org/software/hurd/hurd-talk.html#pat)
	- [Mapping the POSIX Interface](https://www.gnu.org/software/hurd/hurd-talk.html#map)
	- [Active vs Passive](https://www.gnu.org/software/hurd/hurd-talk.html#act)
	- [Authentication](https://www.gnu.org/software/hurd/hurd-talk.html#aut)
	- [Password Server](https://www.gnu.org/software/hurd/hurd-talk.html#pas)
	- [Process Server](https://www.gnu.org/software/hurd/hurd-talk.html#pro)
- The *[translator primer](https://www.gnu.org/software/hurd/hurd/documentation/translator_primer.html)*.
- A document about *[translators](https://www.gnu.org/software/hurd/hurd/documentation/translators.html)* by Marcus Brinkmann.
- [*A Critique of the GNU Hurd Multi-server Operating System*](https://www.gnu.org/software/hurd/hurd/critique.html), an analysis of the GNU Hurd on GNU Mach system, written by Neal Walfield and Marcus Brinkmann.

## External

- [*Examining the Legendary HURD Kernel*](http://www.informit.com/articles/printerfriendly.aspx?p=1180992), an article by David Chisnall.
	Also covers a bit of GNU's and the Hurd's history, fundamental techniques applied, comparisions to other systems.

## Development

- [RPC](https://www.gnu.org/software/hurd/hurd/rpc.html): our usage of *Remote Procedure Call* s.
- *[The GNU Hurd Reference Manual](https://www.gnu.org/software/hurd/hurd/reference_manual.html)*.
- The *[Hurd Hacking Guide](https://www.gnu.org/software/hurd/hurd/hurd_hacking_guide.html)*, an introduction to GNU Hurd and Mach programming by Wolfgang Jährling.
- [*Manually Bootstrapping a Translator*](http://walfield.org/pub/people/neal/papers/hurd-misc/manual-bootstrap.txt), a text by Neal Walfield about how to *manually connect the translator to the filesystem*.
- [*The Authentication Server*](https://www.gnu.org/software/hurd/hurd/documentation/auth.html), the transcript of a talk about the details of the authentication mechanisms in the Hurd by Wolfgang Jährling.
- [*The Mach Paging Interface as Used by the Hurd*](http://lists.gnu.org/archive/html/l4-hurd/2002-06/msg00001.html), a text by Neal Walfield.
- In the [Position paper *Improving Usability via Access Decomposition and Policy*](https://www.gnu.org/software/hurd/hurd/ng/position_paper.html) Neal Walfield and Marcus Brinkmann give an overview about how a future, subsequent system may be architected.
- [*Generalizing mobility for the Hurd*](http://users.student.lth.se/cs07fh9/2009-hammar-hurd-mobility.pdf), a thesis written by Carl Fredrik Hammar, investigates the mobility aspect of stores and how it can be generalized and used for other applications. The background chapter may be of interest to new developers.
- [Ada4Hurd](https://www.gnu.org/software/hurd/hurd/ada4hurd.html): some tools to write translators with Ada