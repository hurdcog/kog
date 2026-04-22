Evidence
========
A primary intellectual activity is the gathering of evidence to
determine the nature and structure of things. It is closely related to
the task of discerning similarity, and of reasoning by analogy.
This git repo is a rumination of these tasks, plus an attempt to express
these ideas as functional code, expressed as OpenCog [Atomese](https://wiki.opencog.org/w/Atomese).

Overview
--------
The process of evidence-gathering and stitching together a story is
used everywhere in human undertakings: examples include journalism, to
put together a news story; jury trials, to determine guilt or innocence;
scientific investigation, to understand the structure of nature.

An example from biology is whole-genome sequencing: stitching
together snippets of genetic sequences, obtained from PCR, into a whole.
Unlike journalism, jury trials or scientific investigation, genome
sequencing can be machine-automated. It is, however, extremely domain
specific: it can only be used for genetics. It is not extensible to
other domains.

The goal of the present project is to explore the foundations of
evidence gathering and the weaving together of (coherent) stories,
and to expose this foundation so that it is amenable to algorithmic
processing. To describe the process of construction sufficiently well,
that it can be applied to arbitrary domains and situations. And not just
philosophically or mathematically, but expressed as software that can
actually do "useful things" that "people want".

The project here is a mash-up of theory and experiment, of philosophy
math and code. The terminology will bounce between abstract concepts and
low-level code. Apologies in advance if it veers off into the opaque.
The topic is difficult.

An Example from Genealogy
-------------------------
A concrete example is in order, one that sits part-way between
abstraction and algorithmic directness. Genealogical research provides
such an example.

Genealogy, as practiced by familysearch.org (controlled by the Church
of the LDS) is semi-automated.  The raw material consists of public data
about births, deaths, census records, military draft records,
naturalization records, passenger lists, parish church records,
marriage certificates, obituaries and the like. Each data item provides
a bit of information that attests a specific event. Some are highly
structured: birth certificates have a regular form. Others, not so much:
obituaries may or may not list relatives, and may or may not make the
relationships clear.  The task is to stitch together a genealogical
structure out of these fragmentary jigsaw pieces.

The evidentiary issue is that no single attestation of an event or
relation is fully trustworthy.  A short survey is worthwhile.
Most of the records predate computerization, and are hand-written,
usually in cursive. Cursive writing is not standardized: there is a vast
variety of styles, especially problematic in 18th and 19th century
European records. There are errors from the transcription of these
texts. The spelling of names and surnames presents a challenge,
especially for European immigrants, where the spelling conventions clash
between the native language and English: many names are simplified and
Anglicized. Catholic Church parish records often convert native names to
Church Latin. Census records indicate that some people are either sloppy
or are fudging the facts, or perhaps they really do not know their
birthdays or how they were spelling their surname a decade earlier.
Many surnames are extremely common, and there are many couples named
"John & Mary".

In practice, the stitching-together is semi-automated. The
familysearch.org system offers up a list of possible record matches,
based on names, dates, locations. A human reviewer then has to review
the record and make a judgement call: is this an appropriate match? If
so, it can be added to a record, buttressing support for a particular
name, date, location, spelling, event.

The judgement call is often hierarchical in nature. For example: do the
father's and mother's names match? Well, perhaps they do, but there is
some alteration in spelling: Maria vs. Mary, Elizabeth vs. Betty. One
can assign some probability or confidence in the belief that these are
the same parents. To buttress that belief, one can look at siblings: if
two children are born three months apart to the same mother, then
this is a piece of negative evidence that absolutely demands a rejection
of a match. That is, of course, only if the recorded birthday is
correct. Perhaps there are other attestations to the birthday? The
evidentiary chain is now four levels deep: individual -> parents ->
siblings -> birthdays -> evidence of birthdays. Each arrow in this chain
can be assigned a confidence; each act of confidence assignment is some
agglomeration of the supporting evidence. A preponderance of evidence
is desired. This can be taken as a mathematical mean or average, but
it can also be some more complex algorithm extending across the
structure: for example, "voting in new members to the club of facts".
In comp sci, similarity is measured with cosine distance, Jacquard
distance, mutual information, Kullback-Leibler divergence or any one
of dozens or hundreds of different similarity measures, which can be
aggregated and weighted in a large variety of ways (random forests,
collections of experts, membership clubs, etc.)

A much simpler, and more easily automated example occurs in operating
system storage integrity. Here, one is presented with multiple files,
often having the same name, but not always; often having the same
timestamp, but not always; often appearing in the same directory, but
not always; often having the same size, but not always; often having the
same content, but sometimes having all-zeros where the data should have
been. Or perhaps MP3's with missing bytes, or images with steganographic
watermarks. The challenge is to find the copy of the file that is least
likely to be corrupted, and is most likely to be "correct". Despite the
fact that all data is directly available from the operating system,
using very pure and well-defined, structured software interfaces, this
turns out to be a hard problem.

Project Goals
-------------
The goal of the project is to be able to provide a generic framework for
perceiving structural factoids, and stitching them together. Earlier
work insists that structural factoids resemble jigsaw puzzle pieces,
which can be stitched together to form sheaves. [AtomSpace Sheaves](https://github.com/opencog/atomspace/tree/master/opencog/sheaf)
provides a general introduction; the idea has a long pedigree in
academia, dating back to the 1960's.

Here, the idea is modified: The jigsaw connector tabs are not strict
yes/no matches, but have some confidence or likelihood. This confidence
or likelihood is obtained from hierarchical sub-relations: do
sub-pieces also fit? This confidence may also be obtained by spreading
over the local neighborhood: is coherence maintained over not just
nearest neighbors, but also next-nearest-neighbors, etc.? In Link
Grammar, this is called a "linkage": all jigsaws must be linkable, from
one end to the other; this is a global property inferred from the local
linkage constraints.

There do structural factoids come from? In natural language parsing,
they come from a "lexis", a dictionary of possible forms created by
professional linguists. That lexis is presumed to encompass the
morpho-syntactic structure of language. The lessons of structural
linguistics and syntactic analysis from the 1990's-2010's is that such
dictionaries are always incomplete, and that no herculean effort by grad
students can ever come to a conclusion. The task is overwhelming. At the
same time, transformers, such as BERT, and now, more generally DL-NN
LLM's have shown that statistical techniques, taken from corpus
linguistics, are far more adept at encoding the structure of language.

Thus, part of the problem is to obtain structural factoids. That is,
they need to be perceived, sensed, obtained from the external
environment. This requires the perception of not just individual data,
e.g. a single birth record, but also the perception of the "shape" of
the structure: birth records have a date and a name; but the date may
only appear at the top of the page, with a list of names below it. Some
classes of birth records provide additional info, others do not. An
extreme form is the obituary: all of us know what to expect; but the
actual informational detail can be spotty or it can provide a wealth of
info. Roughly speaking, no two obituaries have the same "shape", even
though they all have at least a few "jigsaw tab connectors" which can be
joined into the existing weave of genealogical cloth.

That is, not only must the structural factoids be perceived, using some
sort of sensory system, but also the structure itself must be perceived,
presumably with a sensory system built into a post-processing stage.
That is, the data flow must consist of blobs of data, together with a
description of the encoding of that blob of data. The encoding is itself
the result of an act of sensory perception applied to the data stream,
but at a meta-level: i.e. after preliminary analysis has been done.
Thus, this is a recursive process.

Figuring out what to perceive is also a learning process: examining and
comparing input streams allows common patterns to be perceived in those
streams. The common patterns are "learned". The common patterns can then
be used to classify the input stream into buckets. Thus, classification
can also be thought of as a sensory organ. It's not instant-on: it takes
a while to come up to speed. Once running, it can provide descriptions
of data. Which perhaps sounds fine, but raises the problem of the
homunculus: who, exactly is perceiving the structural forms that the
classification subsystem is generating?

Clearly, there are a number of recursive issues. Transformers side-step
these issues into two ways. The low-level issue of perceiving structure
is swept aside by only asking for the probability P(X_n | Y_1, ..., Y_k)
of some occurrence of X_n conditioned in a set of (earlier) Y's. This
pushes all structural information into the weight matrix, in some opaque
and non-interpretable way. Yes, leaps and bounds in interpretability are
being made: the tens of thousands of employees of the
multi-hundred-billion-dollar AI companies are not sitting on their
hands. This project is not a single-handed reversal of those efforts.

The second way that transformers avoid the problem of the homunculus
is that they don't: the actual human user of ChatGPT or Claude or Grok
has at least some vague idea of what they wish to accomplish by using
these systems. It remains an open question in neuroscience as to exactly
how humans decide what to do next.

To recap: the goal of the project is to perceive structure, and to
provide a formal, mathematical, algorithmic description of how structure
is perceived. A subgoal is to boot-strap into perception, i.e. avoid the
homunculus.

Although the word "structure" here is meant to evoke a sensation of good
olde-fashioned symbolic AI, there is no presupposition of avoiding
neural nets. Indeed, the structure of a given fixed DL-NN is described
symbolically: its written in ROCm or CUDA, its written in Python or
TensorFlow. These are structural, symbolic languages.  Insofar as there
is progress at the meta-level, then it is in the use of vibe coding to
design transformers. Which is indeed an interesting approach.  But the
goal here could be rephrased as saying "a system that can vibe-code
itself". And the basic research to achieve this.

Methodology
-----------
Little has been done or decided as of yet. However, it is expected that
the general theoretical foundations, and the actual algorithmic
explorations will be based on the following systems, theories and
infrastructure:
* [Atomese](https://wiki.opencog.org/w/Atomese), a structural description system.
* [Sheaves](https://github.com/opencog/atomspace/tree/master/opencog/sheaf),
  a general theory for assembling. weaving together structural
  information.
* [Sensorimotor](https://github.com/opencog/sensory), a framework for
  sensory perception.
* [Agents](https://github.com/opencog/agents), a framework for
  distinguishing "inside" from "outside", such that the "inside"
  accumulates information about the "outside" into a "world model" of
  "what's out there".

It is expected that transformers, or at least some transformer-like
structure, will play an active part in the implementation. Exactly what
that will be is unknown.

The intent is to make this a primarily experimental effort: to actually
write some code that actually does something, no matter how
underwhelming, incomplete, or inadequate that code might be. The current
intent is to not use vibe coding or any kind of vibes for this, as it is
the author's goal to understand what is happening, at the sub-atomic
level, as it were.

Detailed Design
---------------
See the [design notes](Design.md) for further rambling design thoughts.

See also some texts written in Lyx (LaTeX), viewable as PDF's:
* [Aggregate Satisfaction](aggregate.pdf) attempts to formulate a
  definition of satsifiability that is "effective": that satisfies as
  many constraints as is reasonable, while also resolving contradictions
  as much as reasonable, and providing an effective strategy for
  arriving at "good" values of aggregate satisfiability.


Status
------
***Version 0.0.1.*** An outline of the project goals has been written.
