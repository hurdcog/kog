---
title: "GNU Mes Reference Manual"
source: "https://www.gnu.org/software/mes/manual/mes.html"
author:
published:
created: 2025-08-02
description: "GNU Mes Reference Manual"
tags:
  - "clippings"
---
## GNU Mes

This document describes GNU Mes version 0.26.1, a bootstrappable Scheme interpreter and C compiler written for bootstrapping the GNU system.

## Table of Contents

- [1 Introduction](https://www.gnu.org/software/mes/manual/#Introduction)
	- [1.1 Software Freedom](https://www.gnu.org/software/mes/manual/#Software-Freedom)
	- [1.2 Reproducible Builds](https://www.gnu.org/software/mes/manual/#Reproducible-Builds)
		- [1.2.1 Can we trust our freedom?](https://www.gnu.org/software/mes/manual/#Can-we-trust-our-freedom_003f)
		- [1.2.2 An Old Idea](https://www.gnu.org/software/mes/manual/#An-Old-Idea)
	- [1.3 Bootstrappable Builds](https://www.gnu.org/software/mes/manual/#Bootstrappable-Builds)
		- [1.3.1 Bootstrap Binary Seed](https://www.gnu.org/software/mes/manual/#Bootstrap-Binary-Seed)
	- [1.4 Reduced Binary Seed Bootstrap](https://www.gnu.org/software/mes/manual/#Reduced-Binary-Seed-Bootstrap)
	- [1.5 Scheme-only Bootstrap](https://www.gnu.org/software/mes/manual/#Scheme_002donly-Bootsrap)
	- [1.6 Full Source Bootstrap](https://www.gnu.org/software/mes/manual/#Full-Source-Bootstrap)
		- [1.6.1 Stage0](https://www.gnu.org/software/mes/manual/#Stage0)
		- [1.6.2 M2-Planet](https://www.gnu.org/software/mes/manual/#M2_002dPlanet)
	- [1.7 LISP as Maxwell’s Equations of Software](https://www.gnu.org/software/mes/manual/#LISP-as-Maxwell_0027s-Equations-of-Software)
		- [1.7.1 Auditable Elegance](https://www.gnu.org/software/mes/manual/#Auditable-Elegance)
- [2 Installation](https://www.gnu.org/software/mes/manual/#Installation)
	- [2.1 Requirements](https://www.gnu.org/software/mes/manual/#Requirements)
	- [2.2 Bootstrap Requirements](https://www.gnu.org/software/mes/manual/#Bootstrap-Requirements)
	- [2.3 Running the Test Suites](https://www.gnu.org/software/mes/manual/#Running-the-Test-Suites)
- [3 Bootstrapping](https://www.gnu.org/software/mes/manual/#Bootstrapping)
	- [3.1 The Mes Bootstrap Process](https://www.gnu.org/software/mes/manual/#The-Mes-Bootstrap-Process)
		- [3.1.1 Full Source Bootstrap Deployments](https://www.gnu.org/software/mes/manual/#Full-Source-Bootstrap-Deployments)
	- [3.2 Invoking mes](https://www.gnu.org/software/mes/manual/#Invoking-mes)
		- [3.2.1 Environment Variables](https://www.gnu.org/software/mes/manual/#Environment-Variables)
	- [3.3 Invoking mescc](https://www.gnu.org/software/mes/manual/#Invoking-mescc)
		- [3.3.1 MesCC Environment Variables](https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables)
	- [3.4 Invoking mesar](https://www.gnu.org/software/mes/manual/#Invoking-mesar)
- [4 Contributing](https://www.gnu.org/software/mes/manual/#Contributing)
	- [4.1 Building from Git](https://www.gnu.org/software/mes/manual/#Building-from-Git)
	- [4.2 Running Mes From the Source Tree](https://www.gnu.org/software/mes/manual/#Running-Mes-From-the-Source-Tree)
	- [4.3 Porting GNU Mes](https://www.gnu.org/software/mes/manual/#Porting-GNU-Mes)
	- [4.4 The Perfect Setup](https://www.gnu.org/software/mes/manual/#The-Perfect-Setup)
	- [4.5 Coding Style](https://www.gnu.org/software/mes/manual/#Coding-Style)
		- [4.5.1 Programming Paradigm](https://www.gnu.org/software/mes/manual/#Programming-Paradigm)
		- [4.5.2 Formatting Code](https://www.gnu.org/software/mes/manual/#Formatting-Code)
	- [4.6 Submitting Patches](https://www.gnu.org/software/mes/manual/#Submitting-Patches)
		- [4.6.1 Reporting Bugs](https://www.gnu.org/software/mes/manual/#Reporting-Bugs)
- [5 Acknowledgments](https://www.gnu.org/software/mes/manual/#Acknowledgments)
- [6 Resources](https://www.gnu.org/software/mes/manual/#Resources)
- [Concept Index](https://www.gnu.org/software/mes/manual/#Concept-Index)
- [Programming Index](https://www.gnu.org/software/mes/manual/#Programming-Index)

---

## 1 Introduction

> These were “Maxwell’s Equations of Software!”

— *Alan Kay*

The purpose of GNU Mes [<sup>1</sup>](https://www.gnu.org/software/mes/manual/#FOOT1) is to help create a computer operating system that we can trust.

Mes consists of a mutual self-hosting Scheme interpreter written in C and a Nyacc-based (see see [NYACC User's Guide](https://www.gnu.org/software/mes/manual/nyacc-ug.html#NYACC-User_0027s-Guide) in NYACC User’s Guide) C compiler written in Scheme. The Scheme interpreter mes.c is about 5,000LOC of restricted C, to be compiled with M2-Planet [<sup>2</sup>](https://www.gnu.org/software/mes/manual/#FOOT2), a very simple C compiler.

If we want to trust our computers to do what we instructed them to do then we need to be able to inspect all instructions—all softwares—that we have given it to run.

- [Software Freedom](https://www.gnu.org/software/mes/manual/#Software-Freedom)
- [Reproducible Builds](https://www.gnu.org/software/mes/manual/#Reproducible-Builds)
- [Bootstrappable Builds](https://www.gnu.org/software/mes/manual/#Bootstrappable-Builds)
- [Reduced Binary Seed Bootstrap](https://www.gnu.org/software/mes/manual/#Reduced-Binary-Seed-Bootstrap)
- [Scheme-only Bootstrap](https://www.gnu.org/software/mes/manual/#Scheme_002donly-Bootsrap)
- [Full Source Bootstrap](https://www.gnu.org/software/mes/manual/#Full-Source-Bootstrap)
- [LISP as Maxwell’s Equations of Software](https://www.gnu.org/software/mes/manual/#LISP-as-Maxwell_0027s-Equations-of-Software)

### 1.1 Software Freedom

The four essential Freedoms of Software are at the core of our GNU community. Quoting the GNU philosophy [<sup>3</sup>](https://www.gnu.org/software/mes/manual/#FOOT3)

> A program is free software if the program’s users have the four essential freedoms:
> 
> 1. The freedom to run the program as you wish, for any purpose (freedom 0).
> 2. The freedom to study how the program works, and change it so it does your computing as you wish (freedom 1). Access to the source code is a precondition for this.
> 3. The freedom to redistribute copies so you can help others (freedom 2).
> 4. The freedom to distribute copies of your modified versions to others (freedom 3). By doing this you can give the whole community a chance to benefit from your changes. Access to the source code is a precondition for this.

A computer operating system that respects the user’s freedom is one essential ingredient for building a reliable, trustable computing system. There are about a dozen general purpose operating systems that can be trusted in this way, see [Free Distributions](https://www.gnu.org/distros/free-distros.html). For all softwares on such a system we have the full source code and build recipes available.

So we have access to all the software, we have studied it, possibly modified it, then we built it and we installed it on a computer or some device or appliance. How can we trust that when we run the program we are indeed running the untainted product of the source code that we studied? Unless we are certain of this we cannot really enjoy Freedom 1.

---

### 1.2 Reproducible Builds

The current Reproducible Builds effort incubated in the Debian project [<sup>4</sup>](https://www.gnu.org/software/mes/manual/#FOOT4) and was organized by Lunar. Quoting the Reproducible Builds website [<sup>5</sup>](https://www.gnu.org/software/mes/manual/#FOOT5)

> A build is reproducible if given the same source code, build environment and build instructions, any party can recreate bit-by-bit identical copies of all specified artifacts.

- [Can we trust our freedom?](https://www.gnu.org/software/mes/manual/#Can-we-trust-our-freedom_003f)
- [An Old Idea](https://www.gnu.org/software/mes/manual/#An-Old-Idea)

#### 1.2.1 Can we trust our freedom?

Now consider the opposite, that a second build of a piece of source code produces a different binary program. Upon further investigation we might find that the only difference is probably harmless: a timestamp that was embedded in the binary, or perhaps the name of the user that built it or directory it was built in. Such investigations can be nontrivial and are highly unpractical. And what if the binary difference is not so trivial, cannot be easily accounted for?

A piece of software that cannot be built bit-by-bit reproducible is probably not a good community member in the world of software freedom. We think the importance of reproducibility should not be underestimated largely because failing that precondition makes justifable trust in binaries provided suspect at best and downright dangerous in reality.

It becomes clear that a bit-by-bit reproducible build of all our sofwares is essential if we value our Freedom 1.

#### 1.2.2 An Old Idea

The idea of reproducible builds is not very new. It was implemented for GNU tools in the early 1990s (which we learned, much later in 2017). In the Debian world it was mentioned first in 2000 and then more explicitly in 2007 on debian-devel [<sup>6</sup>](https://www.gnu.org/software/mes/manual/#FOOT6)

> I think it would be really cool if the Debian policy required that packages could be rebuild bit-identical from source.

— *Martin Uecker*

---

### 1.3 Bootstrappable Builds

Software distributions that take reproducible builds seriously are currently shipping well over 90% reproducible packages.

That a package builds bit-by-bit reproducibly however is not enough to guarantee Freedom 1. There is another factor that is often overlooked: opaque ascii or binary *seeds* that are injected during build time. Yes, a package may build reproduciblly from all inspectable sourcess...but what functionality is programmed in the opaque seed?

- [Bootstrap Binary Seed](https://www.gnu.org/software/mes/manual/#Bootstrap-Binary-Seed)

#### 1.3.1 Bootstrap Binary Seed

Possibly one of the most harmless, but certainly by far the biggest binary seed that all software distributions inject are the so called *bootstrap binary seed*. Bootstrap binaries are the initial binary seeds that are used to start building the distribution.

The GNU Guix operating system (see [The GNU Guix Manual](https://www.gnu.org/software/guix/manual/guix.html#Top)), version 1.0 had a relatively small closure of bootstrap binary seed: GNU binutils, GNU gcc, GNU Libc, GNU Guile, and “Static binaries” (think: bash, bzip2, coreutils, gawk, grep, gzip, patch, sed, tar, xz).

```
$ du -schx $(readlink $(guix build bootstrap-tarballs)/*)
2.1M    /gnu/store/9623n4bq6iq5c8cwwdq99qb7d0xj93ym-binutils-static-stripped-tarball-2.28.1/binutils-static-stripped-2.28.1-x86_64-linux.tar.xz
18M    /gnu/store/437xwygmmwwpkddcyy1qvjcv4hak89pb-gcc-stripped-tarball-5.5.0/gcc-stripped-5.5.0-x86_64-linux.tar.xz
1.8M    /gnu/store/55ccx18a0d1x5y6a575jf1yr0ywizvdg-glibc-stripped-tarball-2.26.105-g0890d5379c/glibc-stripped-2.26.105-g0890d5379c-x86_64-linux.tar.xz
5.7M    /gnu/store/bqf0ajclbvnbm0a46819f30804y3ilx0-guile-static-stripped-tarball-2.2.3/guile-static-stripped-2.2.3-x86_64-linux.tar.xz
5.8M    /gnu/store/j8yzjmh9sy4gbdfwjrhw46zca43aah6x-static-binaries-tarball-0/static-binaries-0-x86_64-linux.tar.xz
33M    total
```

only a 33MB download that unpacks to a 252MB *seed* of opaque binary code.

---

### 1.4 Reduced Binary Seed Bootstrap

During the Guix 1.1 development series we managed to create the first reduction by 50% of the Guix *bootstrap binary seed* [<sup>7</sup>](https://www.gnu.org/software/mes/manual/#FOOT7). This was a very important step because the ~250MB *seed* of binary code was practically non-auditable, which makes it hard to establish what source code produced them.

---

### 1.5 Scheme-only Bootstrap

The next step that Guix has taken is to replace the shell and all its utilities with implementations in Guile Scheme, the *Scheme-only bootstrap*. This second halving of the boostrap binaries reduced their size to 25% [<sup>8</sup>](https://www.gnu.org/software/mes/manual/#FOOT8). Gash (see [Gash](https://www.gnu.org/software/mes/manual/gash.html#Gash) in The Gash manual) is a POSIX-compatible shell that replaces Bash, and it comes with Gash Utils which has minimalist replacements for Awk, the GNU Core Utilities, Grep, Gzip, Sed, and Tar. The rest of the bootstrap binary seeds that were removed are now built from source.

---

### 1.6 Full Source Bootstrap

> The holy grail for bootstrappability will be connecting `hex0` to `mes`.

— *Carl Dong*

Reduction of binary seeds is great, but there is an obvious target: we cannot allow any binary seeds in our software stack. Not even in the bootstrap binary seed. Maybe that is a bit too strong: we want to have the absolute minimum of binary seeds and all binary seeds need to be inspectable and must be reviewed. How big would the absolute minimal set be? During the Guix 1.5 development series we managed to close the gap between `stage0-posix` and `mes`. We refer to this as the *Full-Source Bootstrap*.

- [Stage0](https://www.gnu.org/software/mes/manual/#Stage0)
- [M2-Planet](https://www.gnu.org/software/mes/manual/#M2_002dPlanet)

---

#### 1.6.1 Stage0

June 2016 I learnt about [Stage0](https://github.com/oriansj/stage0/). Jeremiah Orians created hex0 a ~500 byte self-hosting hex assembler. The source code is well documented and the binary is the exact mirror of the source code. I was inspired.

Here is an example of what the hex0 program looks like; the start of the hex function

```
00000060: 4883 f830 7c6f 4883 f83a 7c5a 4883 f841  H..0|oH..:|ZH..A
…
000000d0: 48c7 c0ff ffff ffc3 0000 0000 0000 0000  H...............
000000e0: 4883 e830 c300 0000 0000 0000 0000 0000  H..0............
```

All computer programs look like this: an opaque list of computer codes. The initial programs that we take for granted—the bootstrap binary seed—are about 250MB of such numbers: think 250,000 pages full of numbers.

Most computers work pretty well so apparently there is not a pressing need to inspect and study all of these codes. At the same time it is tricky to fully trust [<sup>9</sup>](https://www.gnu.org/software/mes/manual/#FOOT9) a computer that was bootstrapped in this way.

Here is what the source code of the hex0 assembler looks like

```
## function: hex
48 83 f8 30                # cmp $0x30,%rax
7c 6f                      # jl 6000f3 <ascii_other>
48 83 f8 3a                # cmp $0x3a,%rax
7c 5a                      # jl 6000e4 <ascii_num>
48 83 f8 41                # cmp $0x41,%rax
…
## function: ascii_other
48 c7 c0 ff ff ff ff       # mov $0xffffffffffffffff,%rax
c3                         # ret
…
## function: ascii_num
48 83 e8 30                # sub $0x30,%rax
c3                         # ret
```

While it may be hard to understand what this piece of the program does, it should be possible for anyone to verify that the computer codes above correspond to the source code with comments.

One step beyond these annotated codes is Assembly language. To write a program in Assembly, you only need to provide the instructions; the codes are computed by the assembler program.

```
hex:
    # deal all ascii less than 0
    cmp $48, %rax
    jl ascii_other
    # deal with 0-9
    cmp $58, %rax
    jl ascii_num
…
ascii_other:
    mov $-1, %rax
    ret
ascii_num:
    sub $48, %rax
    ret
```

More readable still, a similar program text in the C programming language.

```
int
hex (int c)
{
  if (c >= '0' && c <= '9')
    return c - 48;
…
}
```

What if we could bootstrap our entire system from only this one hex0 assembler binary seed? We would only ever need to inspect these 500 bytes of computer codes. Every [<sup>10</sup>](https://www.gnu.org/software/mes/manual/#FOOT10) later program is written in a more friendly programming language: Assembly, C, … Scheme.

Inspecting all these programs is a lot of work, but it can certainly be done. We might be able to create a fully inspectable path from almost nothing to all of the programs that our computer runs. Something that seemed to be an impossible dream is suddenly starting to look like “just a couple years of work”.

---

#### 1.6.2 M2-Planet

[M2-Planet](https://github.com/oriansj/m2-planet/) [<sup>11</sup>](https://www.gnu.org/software/mes/manual/#FOOT11), when combined with [mescc-tools](https://savannah.gnu.org/projects/mescc-tools/); allows one to compile a subset of the C language into working binaries with introspective steps inbetween. In 2021 M2-Planet with release 1.8.0 reached a level of maturity that allowed to build MesCC-Tools and Mes. This allows for another reduction the Guix bootstrap binaries: mes and mescc-tools can be removed.

---

### 1.7 LISP as Maxwell’s Equations of Software

As fate would have it, I stumbled upon this [interview with Alan Kay](https://queue.acm.org/detail.cfm?id=1039523), where he shares a revelation he had when reading John McCarthy’s [LISP-1.5](https://www.softwarepreservation.org/projects/LISP/book/LISP%201.5%20Programmers%20Manual.pdf) manual:

> that was the big revelation to me … when I finally understood that the half page of code on the bottom of page 13 of the Lisp 1.5 manual was Lisp in itself. These were “Maxwell’s Equations of Software!” This is the whole world of programming in a few lines that I can put my hand over.

— *Alan Kay*

Our starting point is hex0, a 500 byte hex assembler and we need to somehow close the gap to building the bootstrap binary seed, esp. GNU Gcc and the GNU C Library. What better way to do that than by leveraging the powers of LISP?

GNU Mes is a Scheme [<sup>12</sup>](https://www.gnu.org/software/mes/manual/#FOOT12) interpreter that will be indirectly bootstrapped from hex0 and that wields the magical powers of LISP to close the bootstrap gap, asserting we can enjoy software Freedom 1.

- [Auditable Elegance](https://www.gnu.org/software/mes/manual/#Auditable-Elegance)

#### 1.7.1 Auditable Elegance

`eval` and `apply` are mutual recursing functions that—using a few helper functions—describe the core of the universe of computing.

```lisp
(define (apply fn x a)
  (cond
   ((atom fn)
    (cond
     ((eq fn CAR)  (caar x))
     ((eq fn CDR)  (cdar x))
     ((eq fn CONS) (cons (car x) (cadr x)))
     ((eq fn ATOM) (atom (car x)))
     ((eq fn EQ)   (eq (car x) (cadr x)))
     (#t           (apply (eval fn a) x a))))
   ((eq (car fn) LAMBDA)
                   (eval (caddr fn) (pairlis (cadr fn) x a)))
   ((eq (car fn) LABEL)
                   (apply (caddr fn) x
                          (cons (cons (cadr fn) (caddr fn)) a)))))
```

```lisp
(define (eval e a)
  (cond
   ((atom e) (cdr (assoc e a)))
   ((atom (car e))
    (cond ((eq (car e) QUOTE) (cadr e))
          ((eq (car e) COND)  (evcon (cdr e) a))
          (#t                 (apply (car e) (evlis (cdr e) a) a))))
   (#t       (apply (car e) (evlis (cdr e) a) a))))
```

It will be a big day when our computers are fully bootstrapped from source. It would be nice if that source code were readable, auditable and elegant. To be honest, the elegance displayed above that we achieved at the very start of the Mes project is currently hard to find. It is our sincerest hope to bring back this level of quality and elegance..

---

## 2 Installation

Mes is available for download from its website at [https://www.gnu.org/pub/gnu/mes/](https://www.gnu.org/pub/gnu/mes/). This section describes the software requirements of Mes, as well as how to install it and get ready to use it.

- [Requirements](https://www.gnu.org/software/mes/manual/#Requirements)
- [Bootstrap Requirements](https://www.gnu.org/software/mes/manual/#Bootstrap-Requirements)
- [Running the Test Suites](https://www.gnu.org/software/mes/manual/#Running-the-Test-Suites)

---

### 2.1 Requirements

This section lists requirements when building Mes from source. The build procedure for Mes is the same as for other GNU software, and is not covered here. Please see the files README and INSTALL in the Mes source tree for additional details.

GNU Mes depends on the following packages:

- [GNU Guile](https://gnu.org/software/guile/), version 2.0.13 or later, including 2.2.x and 3.0.x,
- [GNU Make](https://www.gnu.org/software/make/).
- [NYACC](https://savannah.gnu.org/projects/nyacc/), version 1.00.2 or later; 1.09.4 is known to work,
- [GCC’s gcc](https://gcc.gnu.org/), version 2.95.3 or later, including 10.2.0,
- [mescc-tools](https://savannah.gnu.org/projects/mescc-tools/), version 1.5.0,

The following dependencies are optional:

- Support for building the bootstrap bin/mes-m2 depends on
- [M2-Planet](https://github.com/oriansj/m2-planet/), version 1.11.0.

Mes is compatible with GNU Guile, so it is possible to share the same Scheme code between both. Currently Mes only supports the minimal subset of R5RS and Guile extensions to run MesCC.

---

### 2.2 Bootstrap Requirements

This section lists requirements when building Mes as a bootstrap package. The bootstrap build procedure for Mes is similar to building GNU software and goes like this

```
sh configure.sh --prefix=/your/prefix/here
sh bootstrap.sh
sh check.sh
sh install.sh
```

See configure.sh and bootstrap.sh for inspiration on what environment variables to set.

Bootstrapping Mes depends on the following packages:

- a POSIX-compatible shell
- [mescc-tools](https://savannah.gnu.org/projects/mescc-tools/), version 1.5.0,
- [M2-Planet](https://github.com/oriansj/m2-planet/), version 1.11.0.
- [NYACC](https://savannah.gnu.org/projects/nyacc/), version 1.00.2 or later; 1.09.4 is known to work.

---

### 2.3 Running the Test Suites

After a successful `configure` and `make` run, it is a good idea to run the test suites.

```
make check
```

Run Mes Scheme language semantics tests (scaffold/boot) only

```
build-aux/check-boot.sh
```

Run a single Mes boot test

```
MES_BOOT=scaffold/boot/00-zero.scm bin/mes
```

Run a single Mes Scheme test

```
./pre-inst-env tests/boot.test
MES=guile ./pre-inst-env tests/boot.test
```

Run MesCC tests only

```
build-aux/check-mescc.sh
```

Run a single MesCC test

```
CC=gcc CC32=i686-unknown-linux-gnu-gcc MES=guile \
  build-aux/test.sh scaffold/tests/00-exit-0
```

---

## 3 Bootstrapping

> Recipe for yogurt: Add yogurt to milk.

— *Anonymous*

The bootstrap problem we have set out to solve is that none of our modern software distributions, and Guix in particular, can be created all from source code. In addition to the carefully signed source code of all the programs (the ‘milk’) an opaque binary seed (the ‘yogurt’) is injected as an essential dependency.

Why would this be a problem, I hear you ask? This is how it is done, we always did it this way, everyone does it like this! Indeed, a popular way of handling the bootstrapping issue is by ignoring it.

> Your compiler becoming self-hosting…a language creator’s wet dream.

— *PFH*

It seems that writing a self-hosting compiler is considered to be a language creator’s ultimate goal. It means that their language and compiler have become powerful enough to not depend on a pre-exising language that possibly is—but certainly was until now—more powerful; it feels like passing the rite to adulthood.

When you see the irony, you grasp what our bootstrapping effort means in practice. Creating bootstrappable software is not hard; actually most softwares’ first releases are bootstrappable. The problem of bootstrapping is not a technical one, it is a lack of awareness and responsibility.

- [The Mes Bootstrap Process](https://www.gnu.org/software/mes/manual/#The-Mes-Bootstrap-Process)
- [Invoking mes](https://www.gnu.org/software/mes/manual/#Invoking-mes)
- [Invoking mescc](https://www.gnu.org/software/mes/manual/#Invoking-mescc)
- [Invoking mesar](https://www.gnu.org/software/mes/manual/#Invoking-mesar)

---

### 3.1 The Mes Bootstrap Process

The Full Source Bootstrap currently adopted by Guix [<sup>13</sup>](https://www.gnu.org/software/mes/manual/#FOOT13). In its intiial form it is only available for x86-linux.

Currently, it goes like this:

```
gcc-mesboot (4.9.4)
                                ^
                                |
                              (...)
                                ^
                                |
         binutils-mesboot (2.20.1a), glibc-mesboot (2.2.5),
                    gcc-core-mesboot (2.95.3)
                                ^
                                |
                       patch-mesboot (2.5.9)
                                ^
                                |
                 bootstrappable-tcc (0.9.26+31 patches)
                                ^
                                |
                      gnu-make-mesboot0 (3.80)
                                ^
                                |
                        gzip-mesboot (1.2.4)
                                ^
                                |
                         tcc-boot (0.9.27)
                                ^
                                |
                           mes-boot (0.24.2)
                                ^
                                |
                   stage0-posix (hex0..M2-Planet)
                                ^
                                |
                    gash-boot, gash-utils-boot
                                ^
                                |
                                *
               bootstrap-seeds (357-bytes for x86)
                               ~~~
             [bootstrap-guile-2.0.9 driver (~25 MiB)]
```

Here’s a generated dependency diagram to for the initial bootstrap gcc that builds the rest of Guix.

![Reference graph of the gcc-core-mesboot0](https://www.gnu.org/software/mes/manual/images/gcc-mesboot-graph.png)

For now, this additional non-bootstrapped dependencies (i.e., binary seeds) are taken for granted

```
bootstrap-guile
```

Our next priority is to eleminate the dependency on `bootstrap-guile` for `Gash` and `Gash-Utils`, and thus for `Mes`.

- [Full Source Bootstrap Deployments](https://www.gnu.org/software/mes/manual/#Full-Source-Bootstrap-Deployments)

---

#### 3.1.1 Full Source Bootstrap Deployments

`GNU Guix`

Reference implementation of the Full Source Bootstrap for `i686-linux` and `x86_64-linux` [building from source all the way down](https://guix.gnu.org/en/blog/2023/the-full-source-bootstrap-building-from-source-all-the-way-down/).

`Bitcoin Core`

The first application of Guix’ Reduced Binary Seed Bootstrap was done in bitcoin core [Bootstrappable Bitcoin Core Builds](https://github.com/bitcoin/bitcoin/blob/master/contrib/guix/README.md).

`Live Bootstrap`

The [live-bootstrap](https://github.com/fosslinux/live-bootstrap/) has no dependency on `guile-bootstrap`, and removes and bootstraps generated autotools and flex/bison files.

`Freedesktop SDK`

The Freedesktop SDK used to have an informal dependency on its previous version being installed in binary from. This dependency was mostly broken in [bootstrap freedesktop binary seed](https://gitlab.com/freedesktop-sdk/freedesktop-sdk/-/merge_requests/11557), they still have a dependency on binary Rust.

`Full Source Bootstrap from Git`

Building on the Live Bootstrap, but building most everything from Git [FSB from Git](https://github.com/schierlm/FullSourceBootstrapFromGit).

`NixOS`

Implementation of a FSB has started [NixOS 256b bootstrap (in progress)](https://github.com/NixOS/nixpkgs/pull/227914).

---

### 3.2 Invoking mes

The `mes` command is the Scheme interpreter whose prime directive is to run the `MesCC` program.

For convenience and testing purposes, `mes` tries to mimic guile.

```
mes option… FILE…
```

The option s can be among the following:

`-s script arg…`

By default, mes will read a file named on the command line as a script. Any command-line arguments arg … following script become the script’s arguments; the `command-line` function returns a list of strings of the form `(script arg…)`.

Scripts are read and evaluated as Scheme source code just as the `load` function would. After loading script, mes exits.

`-c expr arg…`

Evaluate expr as Scheme code, and then exit. Any command-line arguments arg …) following expr become command-line arguments; the `command-line` function returns a list of strings of the form `(guile arg…)`, where mes is the path of the mes executable.

`-- arg…`

Run interactively, prompting the user for expressions and evaluating them. Any command-line arguments arg … following the \-- become command-line arguments for the interactive session; the `command-line` function returns a list of strings of the form `(guile arg…)`, where mes is the path of the mes executable.

`-L,--load-path=directory`

Add directory to the front of Mes module load path. The given directories are searched in the order given on the command line and before any directories in the `GUILE_LOAD_PATH` environment variable.

`-C,--compiled-path=directory`

Accepted and ignored for Guile compatibility.

`-l file`

Load Scheme source code from file, and continue processing the command line.

`-e,--main=function`

Make function the *entry point* of the script. After loading the script file (with \-s) or evaluating the expression (with \-c), apply function to a list containing the program name and the command-line arguments—the list provided by the `command-line` function.

`-h, --help`

Display help on invoking mes, and then exit.

`-v, --version`

Display the current version of mes%, and then exit.

- [Environment Variables](https://www.gnu.org/software/mes/manual/#Environment-Variables)

---

#### 3.2.1 Environment Variables

Here are the environment variables (see see [Environment Variables](https://www.gnu.org/software/guile/manual/guile.html#Environment-Variables) in Guile Reference) that affect the run-time behavior of mes:

`MES_BOOT`

Set `MES_BOOT` to change the initial Scheme program that mes runs.

`MES_ARENA`

The initial size of the arena see [5.3](https://www.gnu.org/software/mes/manual/sicp.html#g_t5_002e3) in SICP in cells. Default: 20,000.

`MES_MAX_ARENA`

The maximum size of the arena in cells. Default: 100,000,000.

`MES_MAX_STRING`

The maximum size of a string. Default: 524,288.

`MES_DEBUG`

1. Informational:
	- MODULEDIR
	- included SCM modules and sources
	- result of program
	- gc stats at exit
2. opened files
3. runtime gc stats
4. detailed info
	- parsed, expanded program
	- list of builtins
	- list of symbol
	- opened input strings
	- gc details
5. usage of opened input strings

`GUILE_LOAD_PATH`

This variable may be used to augment the path that is searched for Scheme files when loading. Its value should be a colon-separated list of directories. If it contains the special path component `...`(ellipsis), then the default path is put in place of the ellipsis, otherwise the default path is placed at the end. The result is stored in `%load-path`.

Mes uses **GUILE** \_LOAD\_PATH for compatibility with Guile.

---

### 3.3 Invoking mescc

```
mescc option… FILE…
```

The option s can be among the following:

`--align=symbol`

align symbol, the default is `functions`; other valid values are: `globals`.

`--base-address=ADDRESS`

use BaseAddress ADDRESS \[0x1000000\]

`-c`

preprocess, compile and assemble only; do not link

`-D DEFINE[=VALUE]`

`-dumpmachine`

display the compiler’s target processor

`-E`

preprocess only; do not compile, assemble or link

`-g`

add `blood-elf` debug info

This enables GDB setting breakpoints on function names, and to have the GDB backtrace command to show the function call stack.

`-h, --help`

display this help and exit

`-I DIR`

append DIR to include path

`-L DIR`

append DIR to library path

`-l LIBNAME`

link with LIBNAME

`-m BITS`

compile for BITS bits \[32\]

`-O LEVEL`

use optimizing LEVEL

`-o FILE`

write output to FILE

`-S`

preprocess and compile only; do not assemble or link

`--std=STANDARD`

assume that the input sources are for STANDARD

`-V,--version`

display version and exit

`-w,--write=TYPE`

dump Nyacc AST using TYPE {pretty-print,write}

`-x LANGUAGE`

specify LANGUAGE of the following input files

- [MesCC Environment Variables](https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables)

---

#### 3.3.1 MesCC Environment Variables

`MES`

Setting `MES` to a mes-compatible Scheme will run mescc using that

```
MES=guile mescc -c scaffold/main.c
```

See, now Guile has become compatible with Mes, instead of the other way around;-)

`C_INCLUDE_PATH`

`LIBRARY_PATH`

`NYACC_DEBUG`

Setting `NYACC_DEBUG` makes nyacc print names of function during the parsing phase.

---

### 3.4 Invoking mesar

```
mesar option… command ARCHIVE-FILE FILE…
```

The command is ignored for compatibility with ar

```
r[ab][f][u]  - replace existing or insert new file(s) into the archive
  [c]          - do not warn if the library had to be created
  [D]          - use zero for timestamps and uids/gids (default)
```

and assumed to be crD.

The option s can be among the following:

`-h, --help`

display this help and exit

`-V,--version`

display version and exit

---

## 4 Contributing

- [Building from Git](https://www.gnu.org/software/mes/manual/#Building-from-Git)
- [Running Mes From the Source Tree](https://www.gnu.org/software/mes/manual/#Running-Mes-From-the-Source-Tree)
- [Porting GNU Mes](https://www.gnu.org/software/mes/manual/#Porting-GNU-Mes)
- [The Perfect Setup](https://www.gnu.org/software/mes/manual/#The-Perfect-Setup)
- [Coding Style](https://www.gnu.org/software/mes/manual/#Coding-Style)
- [Submitting Patches](https://www.gnu.org/software/mes/manual/#Submitting-Patches)

---

### 4.1 Building from Git

If you want to hack GNU Mes itself, it is recommended to use the latest version from the Git repository:

```
git clone git://git.savannah.gnu.org/mes.git
```

The easiest way to set up a development environment for Mes is, of course, by using Guix! The following command starts a new shell where all the dependencies and appropriate environment variables are set up to hack on Mes:

```
guix shell
```

If you are unable to use Guix when building Mes from a Git checkout, the following are the required packages in addition to those mentioned in the installation instructions (see [Requirements](https://www.gnu.org/software/mes/manual/#Requirements)).

- [GNU Help2man](https://gnu.org/software/help2man/);
- [GNU Texinfo](https://gnu.org/software/texinfo/);
- [Graphviz](https://www.graphviz.org/);
- [Perl](https://www.perl.org/).

Finally, you have to invoke `make check` to run tests (see [Running the Test Suites](https://www.gnu.org/software/mes/manual/#Running-the-Test-Suites)). If anything fails, take a look at installation instructions (see [Installation](https://www.gnu.org/software/mes/manual/#Installation)) or send a message to the [bug-mes@gnu.org](https://www.gnu.org/software/mes/manual/) mailing list.

---

### 4.2 Running Mes From the Source Tree

First, you need to have an environment with all the dependencies available (see [Building from Git](https://www.gnu.org/software/mes/manual/#Building-from-Git)), and then simply prefix each command by `./pre-inst-env` (the pre-inst-env script lives in the top build tree of Mes).

---

### 4.3 Porting GNU Mes

Mes is supported for x86-linux and armhf-linux. A 64 bit (x86\_64-linux) is almost done, only a few bugs remain. The Guix bootstrap for x86\_64-linux uses mes for x86-lunix and that is not expected to change. Likewise, aarch64-linux uses mes for armhf-linux.

A port to GNU/Hurd (x86-gnu) is underway.

Initial scaffold, built by build-aux/build-scaffold.sh:

```
lib/linux/x86-mes-gcc/exit-42.S
  lib/linux/x86-mes/elf32-0exit-42.hex2
  lib/linux/x86-mes/elf32-body-exit-42.hex2

  lib/linux/x86-mes-gcc/hello-mes.S
  lib/linux/x86-mes/elf32-0hello-mes.hex2
  lib/linux/x86-mes/elf32-body-hello-mes.hex2
```

Porting MesCC:

```
lib/x86-mes/x86.M1

  module/mescc/mescc.scm
  module/mescc/i386/as.scm
  module/mescc/i386/info.scm

  mes/module/mescc/i386/as.mes
  mes/module/mescc/i386/info.mes
```

---

### 4.4 The Perfect Setup

The Perfect Setup to hack on Mes is basically the perfect setup used for Guile hacking (see [Using Guile in Emacs](https://www.gnu.org/software/guile/manual/guile.html#Using-Guile-in-Emacs) in Guile Reference Manual). First, you need more than an editor, you need [Emacs](https://www.gnu.org/software/emacs), empowered by the wonderful [Geiser](https://nongnu.org/geiser/).

Geiser allows for interactive and incremental development from within Emacs: code compilation and evaluation from within buffers, access to on-line documentation (docstrings), context-sensitive completion,M-. to jump to an object definition, a REPL to try out your code, and more (see [Introduction](https://www.gnu.org/software/mes/manual/geiser.html#Introduction) in Geiser User Manual).

---

### 4.5 Coding Style

In general our code follows the GNU Coding Standards (see [GNU Coding Standards](https://www.gnu.org/prep/standards/standards.html#Top)). However, they do not say much about Scheme, so here are some additional rules.

- [Programming Paradigm](https://www.gnu.org/software/mes/manual/#Programming-Paradigm)
- [Formatting Code](https://www.gnu.org/software/mes/manual/#Formatting-Code)

#### 4.5.2 Formatting Code

When writing Scheme code, we follow common wisdom among Scheme programmers. In general, we follow the [Riastradh’s Lisp Style Rules](https://mumble.net/~campbell/scheme/style.txt). This document happens to describe the conventions mostly used in Guile’s code too. It is very thoughtful and well written, so please do read it.

If you do not use Emacs, please make sure to let your editor knows these rules.

Additionally, in Mes we prefer to format `if` statements like this

```
(if foo? trivial-then
    (let ((bar (the-longer …)))
      more-complicated
      …
      else))
```

---

### 4.6 Submitting Patches

Development is done using the Git distributed version control system. Thus, access to the repository is not strictly necessary. We welcome contributions in the form of patches as produced by `git format-patch` sent to the [bug-mes@gnu.org](https://www.gnu.org/software/mes/manual/) mailing list.

Please write commit logs in the ChangeLog format (see [Change Logs](https://www.gnu.org/prep/standards/standards.html#Change-Logs) in GNU Coding Standards); you can check the commit history for examples.

- [Reporting Bugs](https://www.gnu.org/software/mes/manual/#Reporting-Bugs)

#### 4.6.1 Reporting Bugs

Encountering a problem or bug can be very frustrating for you as a user or potential contributor. For us as Mes maintainers, the preferred bug report includes a beautiful and tested patch that we can integrate without any effort.

However, please don’t let our preference stop you from reporting a bug. There’s one thing *much* worse for us than getting a bug report without a patch: Reading a complaint or rant online about your frustrations and how our work sucks, without having heard directly what you experienced.

So if you report a problem, will it be fixed? And **when**? The most honest answer is: It depends. Let’s curry that informationless honesty with a more helpful and more blunt reminder of a mantra of free software:

> **Q:**
> 
> When will it be finished?
> 
> **A:**
> 
> It will be ready sooner if you help.

— *Richard Stallman*

Join us on `#bootstrappable` on the Libera Chat IRC network or on [bug-mes@gnu.org](https://www.gnu.org/software/mes/manual/) to share your experience—good or bad.

Please send bug reports with full details to [bug-mes@gnu.org](https://www.gnu.org/software/mes/manual/).

---

## 5 Acknowledgments

We would like to thank the following people for their help: Jeremiah Orians, Peter de Wachter, rain1, Ricardo Wurmus, Rutger van Beusekom.

We also thank Ludovic Courtès for creating GNU Guix and making the bootstrap problem so painfully visible, John McCarthy for creating LISP-1.5 and Alan Kay for their inspiring comment on [Page 13](https://queue.acm.org/detail.cfm?id=1039523).

---

## 6 Resources

- [Bootstrappable Builds](https://bootstrappable.org/) Minimize the amount and size of opaque binary seeds we need to swallow.
- [Reproducible Builds](https://reproducible-builds.org/) Provide a verifiable path from source code to binary.
- [Stage0](https://gitlab.com/oriansj/stage0) If we want, it could all start with a ~500 byte self-hosting hex assembler.
- [Bootstrapping wiki](https://bootstrapping.miraheze.org/) An amazing collection of small/bootstrappable compilers, operating systems, anything you need.
- [#bootstrappable](https://www.gnu.org/software/mes/manual/irc.libera.chat) The bootstrapping community home at the Libera Chat IRC network.
- guix-devel@gnu.org The Guix mailing list, where it all started.[guix-devel archives](https://lists.gnu.org/archive/html/guix-devel/).

---

## Concept Index

| Jump to: | [**A**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-A) [**B**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-B) [**C**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-C) [**D**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-D) [**E**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-E) [**F**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-F) [**G**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-G) [**I**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-I) [**L**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-L) [**M**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-M) [**P**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-P) [**R**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-R) [**S**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-S) [**T**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-T) |
| --- | --- |

<table><tbody><tr><td></td><th align="left">Index Entry</th><td></td><th align="left">Section</th></tr><tr><td colspan="4"><hr></td></tr><tr><th>A</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-arch">arch</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mescc">Invoking mescc</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-architecture">architecture</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mescc">Invoking mescc</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>B</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-bug_002c-bug-report_002c-reporting-a-bug">bug, bug report, reporting a bug</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Submitting-Patches">Submitting Patches</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>C</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-coding-style">coding style</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Coding-Style">Coding Style</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-compile">compile</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mescc">Invoking mescc</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-contact_002c-irc_002c-mailing-list">contact, irc, mailing list</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Submitting-Patches">Submitting Patches</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>D</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-define-DEFINE-_005bVALUE_003d1_005d">define DEFINE [VALUE=1]</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mescc">Invoking mescc</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>E</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-environment-variables">environment variables</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-evaluate-expression_002c-command_002dline-argument">evaluate expression, command-line argument</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mes">Invoking mes</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>F</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-formatting-code">formatting code</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Coding-Style">Coding Style</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-formatting_002c-of-code">formatting, of code</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Coding-Style">Coding Style</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>G</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-Guile_002c-compatibility">Guile, compatibility</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Requirements">Requirements</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>I</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-indentation_002c-of-code">indentation, of code</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Coding-Style">Coding Style</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-initialization">initialization</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-installing-Mes">installing Mes</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Installation">Installation</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>L</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-license_002c-GNU-Free-Documentation-License">license, GNU Free Documentation License</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#GNU-Free-Documentation-License">GNU Free Documentation License</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>M</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-machine">machine</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mescc">Invoking mescc</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>P</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-purpose">purpose</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Introduction">Introduction</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>R</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-repl">repl</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mes">Invoking mes</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>S</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-script-mode">script mode</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Invoking-mes">Invoking mes</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-shell">shell</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>T</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-test-suites">test suites</a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Running-the-Test-Suites">Running the Test Suites</a></td></tr><tr><td colspan="4"><hr></td></tr></tbody></table>

| Jump to: | [**A**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-A) [**B**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-B) [**C**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-C) [**D**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-D) [**E**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-E) [**F**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-F) [**G**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-G) [**I**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-I) [**L**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-L) [**M**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-M) [**P**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-P) [**R**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-R) [**S**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-S) [**T**](https://www.gnu.org/software/mes/manual/#Concept-Index_cp_letter-T) |
| --- | --- |

---

## Programming Index

| Jump to: | [**C**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-C) [**G**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-G) [**L**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-L) [**M**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-M) [**N**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-N) |
| --- | --- |

<table><tbody><tr><td></td><th align="left">Index Entry</th><td></td><th align="left">Section</th></tr><tr><td colspan="4"><hr></td></tr><tr><th>C</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-C_005fINCLUDE_005fPATH"><code>C_INCLUDE_PATH</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables">MesCC Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>G</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-GUILE_005fLOAD_005fPATH"><code>GUILE_LOAD_PATH</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>L</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-LIBRARY_005fPATH"><code>LIBRARY_PATH</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables">MesCC Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>M</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES"><code>MES</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables">MesCC Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES_005fARENA"><code>MES_ARENA</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES_005fBOOT"><code>MES_BOOT</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES_005fDEBUG"><code>MES_DEBUG</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES_005fMAX_005fARENA"><code>MES_MAX_ARENA</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-MES_005fMAX_005fSTRING"><code>MES_MAX_STRING</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#Environment-Variables">Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr><tr><th>N</th><td></td><td></td></tr><tr><td></td><td><a href="https://www.gnu.org/software/mes/manual/#index-NYACC_005fDEBUG"><code>NYACC_DEBUG</code></a>:</td><td></td><td><a href="https://www.gnu.org/software/mes/manual/#MesCC-Environment-Variables">MesCC Environment Variables</a></td></tr><tr><td colspan="4"><hr></td></tr></tbody></table>

| Jump to: | [**C**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-C) [**G**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-G) [**L**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-L) [**M**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-M) [**N**](https://www.gnu.org/software/mes/manual/#Programming-Index_fn_letter-N) |
| --- | --- |

---

#### Footnotes

##### (1)

“Mes” is an acronym for the Maxwell Equations of Software.

##### (2)

See [https://github.com/oriansj/m2-planet](https://github.com/oriansj/m2-planet)

##### (3)

The four essential freedoms [https://www.gnu.org/philosophy/free-sw.html](https://www.gnu.org/philosophy/free-sw.html)

##### (4)

[The Debian Project](https://debian.org/)

##### (5)

[Reproducible Builds](https://reproducible-builds.org/)

##### (6)

[Martin Uecker on debian-devel on bit-reproducibility](https://lists.debian.org/debian-devel/2007/09/msg00746.html)

##### (7)

See [https://guix.gnu.org/blog/2019/guix-reduces-bootstrap-seed-by-50/](https://guix.gnu.org/blog/2019/guix-reduces-bootstrap-seed-by-50/)

##### (8)

See [https://guix.gnu.org/en/blog/2020/guix-further-reduces-bootstrap-seed-to-25/](https://guix.gnu.org/en/blog/2020/guix-further-reduces-bootstrap-seed-to-25/)

##### (9)

Ken Thompson’s 1984 Turing award acceptance speech [Reflections on Trusting Tust](https://www.ece.cmu.edu/~ganger/712.fall02/papers/p761-thompson.pdf).

##### (10)

Some program languages have become very hard or practically impossible to bootstrap. Instead of depending on a simple language such as C, they depend on a recent version of itself, or on other binary or ASCII seeds, on other recent programs written in that language, or even on manual intervention. Programs written in a language that cannot be bootstrapped can still run on our systems, but cannot enjoy any of the trust we intend to create.

##### (11)

The PLAtform NEutral Transpiler

##### (12)

Scheme is a modern LISP

##### (13)

See gnu/packages/commencement.scm in the master branch in Guix git [https://git.savannah.gnu.org/cgit/guix.git/tree/gnu/packages/commencement.scm](https://git.savannah.gnu.org/cgit/guix.git/tree/gnu/packages/commencement.scm)