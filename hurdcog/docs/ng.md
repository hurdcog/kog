---
title: "ng"
source: "https://www.gnu.org/software/hurd/hurd/ng.html"
author:
published:
created: 2025-08-01
description:
tags:
  - "clippings"
---
Hurd-ng is an effort to build a new operating system that preserves the main design goals of the Hurd while fixing some of the Hurd's shortcomings. There is not yet an official roadmap or a concrete specification; indeed, much of the work is research oriented.

These pages try to summarize the major discussions and ideas.

## Why ngHurd

This section explains the motivations behind the new design:

- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=hurd%2Fng&page=Issues_with_L4_Pistachio)Issues with L4 Pistachio
- [Limitations of the original Hurd design](https://www.gnu.org/software/hurd/hurd/ng/limitations_of_the_original_hurd_design.html)
- History of the [port to another microkernel](https://www.gnu.org/software/hurd/history/port_to_another_microkernel.html)

## Work already done

A [critique](https://www.gnu.org/software/hurd/hurd/critique.html) of the original Hurd is available.

A [position paper](https://www.gnu.org/software/hurd/hurd/ng/position_paper.html) by Marcus Brinkmann and Neal H. Walfield can be found.

A draft specification of the Hurd-NG interfaces has been, but is no longer, available.

## Subjects

## Design processus

- [Design Goals](https://www.gnu.org/software/hurd/hurd/ng/designgoals.html)
- [Requirements For User](https://www.gnu.org/software/hurd/hurd/ng/requirementsforuser.html)
- [Design Principles](https://www.gnu.org/software/hurd/hurd/ng/designprinciples.html)
- [Philosophy](https://www.gnu.org/software/hurd/hurd/ng/philosophy.html)

## Concepts

- [Security](https://www.gnu.org/software/hurd/security.html)
- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=hurd%2Fng&page=CapabilityBasedMicrokernel)Capability Based Microkernel
- [First-class Receive Buffer](https://www.gnu.org/software/hurd/hurd/ng/firstclassreceivebuffer.html)
- [PowerBox](https://www.gnu.org/software/hurd/hurd/ng/powerbox.html)
- [What is a Capability](https://www.gnu.org/software/hurd/hurd/ng/whatisacapability.html)
- [What is a Constructor](https://www.gnu.org/software/hurd/hurd/ng/whatisaconstructor.html)
- [What is a Spacebank](https://www.gnu.org/software/hurd/hurd/ng/whatisaspacebank.html)
- [Trivial Confinement vs. Constructor vs. Fork](https://www.gnu.org/software/hurd/hurd/ng/trivialconfinementvsconstructorvsfork.html)
- [Copy vs. Revocable Copy vs. Map](https://www.gnu.org/software/hurd/hurd/ng/copyvsrevocablecopyvsmap.html)
- [Setuid vs. Constructor](https://www.gnu.org/software/hurd/hurd/ng/setuidvsconstructor.html)
- [Hurdish Applications for Persistence](https://www.gnu.org/software/hurd/hurd/ng/hurdishapplicationsforpersistence.html)
- [What's in a Group](https://www.gnu.org/software/hurd/hurd/ng/whatsinagroup.html)
- [The Polycast Interface](https://www.gnu.org/software/hurd/hurd/ng/thepolycastinterface.html)
- [Permission Bits](https://www.gnu.org/software/hurd/hurd/ng/permissionbits.html)
- [Cancellation Forwarding](https://www.gnu.org/software/hurd/hurd/ng/cancellationforwarding.html)

## Problems to solve

- [How Much Confinement Do We Want](https://www.gnu.org/software/hurd/hurd/ng/howmuchconfinementdowewant.html)
- [Shared Libraries](https://www.gnu.org/software/hurd/hurd/ng/sharedlibraries.html)
- [Path Max](https://www.gnu.org/software/hurd/hurd/ng/pathmax.html)

## Implementation

- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=hurd%2Fng&page=HurdInterafaces)Hurd Interafaces
- [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=hurd%2Fng&page=PosixLayer)Posix Layer
- [System Structure](https://www.gnu.org/software/hurd/hurd/ng/systemstructure.html)

## Use Cases

*please move me somewhere better! [?](https://darnassus.sceen.net/cgi-bin/hurd-web?do=create&from=hurd%2Fng&page=SamMason)SamMason*

- [Use Case User Filesystem](https://www.gnu.org/software/hurd/hurd/ng/usecaseuserfilesystem.html)
- [Use Case Private Keys](https://www.gnu.org/software/hurd/hurd/ng/usecaseprivatekeys.html)

## Organization

Summaries should obey the following structure:

- if there is a consensus, it is clearly described
- if controversial points remain, there are also described after the consensus
- if no choice has been clearly made, all valid positions are described
- withdrawn and invalid positions (proved wrong, unrealistic, contradictory to some design principle, etc.) should be described only very briefly, and developed in a separate article

Each time a point seems to be overly long with respect to the rest of the article, it should be summarized in place and developed in a separate article.