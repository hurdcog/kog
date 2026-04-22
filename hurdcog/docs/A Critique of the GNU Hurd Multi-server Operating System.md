---
title: "A Critique of the GNU Hurd Multi-server Operating System"
source: "https://www.gnu.org/software/hurd/hurd/critique.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
Neal Walfield and Marcus Brinkmann wrote a paper titled [*A Critique of the GNU Hurd Multi-server Operating System*](http://walfield.org/papers/200707-walfield-critique-of-the-GNU-Hurd.pdf). This was published in ACM SIGOPS Operating Systems Review in July 2007. This is sometimes referred to as *the critique*.

The paper provides a technical overview of the Hurd's architecture and critiques some of the decisions made.

## IRC, freenode, #hurd, 2014-02-06

```
<bwright> Just read a paper on hurd.
<bwright> Some interesting dot-dot and chroot issues were raised.
<bwright> But this was written my guess is in about 2007.
```

## IRC, freenode, #hurd, 2014-02-08

```
<antrik> bwright: both the dot-dot and chroot issues are fairly easy to
  solve... of course they do indicate some more fundamental things to keep
  in mind though. in fact, a few years ago we came up with a concept for
  making filesystem permission handling more robust... but nobody ever got
  to implementing it :-(
<antrik> bwright: this paper, I guess you are referring to the "critique"?
  it was in fact written by the Hurd/L4 initiators. the observations made
  in this paper are right, but IMHO they got carried away on the
  conclusions -- most of the issues can be solved within the existing
  framework, if you think about the actual problems seriously
<azeem> so they didn't think about it seriously?
<antrik> azeem: not in the right mindset I'd say :-)
<antrik> macrus actually said himself a while later that he probably
  could/should have implemented some of the ideas within the existing
  Hurd...
```