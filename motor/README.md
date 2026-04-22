Sensori-Motor Research
======================
Conventional machine-learning systems are force-fed data by a collection
of scripts and control panels; they "data process" and "machine learn"
for a while, using up CPU time, and eventually producing some answer
that some customer, client or user wanted. This kind of forced
processing is the modern variant of what used to be called "batch
processing". The obvious industrial alternative is continuous-flow
processing. A canonical example might be the self-driving car: it gets
continuous data from a variety of sensors, synthesizes that data, and
arrives at "real-time", millisecond-by-millisecond motor-control
decisions.

Smart engineers have no problem creating both batch and continuous-flow
systems. Its a standard engineering task pervasive in industry, from
baking bread to creating plastics. It's, well, "obvious". Like all good
things that are obvious, it can be made the topic of research, and so
here we are.

This is part of a research program that includes the following
[Opencog](https://github.com/opencog/) projects:
* [Sensory](https://github.com/opencog/sensory), which looks at the
  abstract structure of perception and action. Perhaps too abstract;
  this project aims at a simpler, easier and more practical approach.
* [Agents](https://github.com/opencog/agents), the original motivating
  project that aims to assemble the frameworks provided by the
  sensori-motor, perception-action systems into functional agents.
* [Learn](https://github.com/opencog/learn), the structure learning
  system. It's currently a batch-processing system, and converting it
  to a continuous-stream system resulted in the Agents project, which
  in turn spun off the Sensory project, which in turn spun off this
  project. The goal is to integrate all of these, and get back to the
  task of generic structural learning.

Caution
-------
Some of what you read below might seem so basic and obvious, that you
might want to dismiss the entire project as silliness of some sort.
If so, you can save yourself some time and stop reading right now.

The central object of study is a file system crawler. The concept
of a crawler has been around since before the web. There are many
freely available crawlers. If you just want that, go download one and
use it.  This project is not for you.

If you think there might be some AI magic in the below, you will be
sorely disappointed. The result will not be a conversational agent
providing a miraculous oracle to all of your questions.

The Crawler
-----------
The task is to crawl some file system, collect some data about it,
perform some measurements on that data, including structural similarity,
use those results to look a second time, and gather and process what
was missed the first time.

A file system is chosen because it is one of the simplest data
structures that is not trivial. File systems can be thought of as trees,
and quite a a lot of human-generated societal data is organized in a
hierarchical fashion. So, the first task of writing a file-system
crawler now becomes non-trivial: it should be able to crawl any kind of
hierarchical structure.

There are several design choices. One is to pre-process this "input
hierarchical data" into a tree format that is machine-readable, and
deploy the analysis program on it. This design choice is rejected out of
hand, for multiple reasons:
* It requires a batch-processing step, whereas this line of research
  is interested in continuous-process systems.
* There is a risk of data loss in the conversion from this other format
  into some pre-defined input format. Data conversion is a pain in the
  neck; ask anyone with business software experience. Something is
  always lost in the process. There are multi-billion dollar
  corporations specializing in data conversion. There is no desire to
  enter that competitive landscape.
* It requires knowing, in advance, what aspects of the data are
  "important". This is counter-productive to the task of AGI learning,
  where the importance of some data element is not known, a priori.
  Note the last sentence says "AGI learning" and not "machine learning";
  in machine learning, the engineers usually already have a pretty
  good grasp of what they are looking for, and what they expect to get.
  In AGI, we don't know. For AGI, the task is "here's a blob of stuff,
  look at it, and figure it out."

Thus, one aim of this project is to build a proof-of-concept crawler
that can walk a collection of hierarchical structures, in the abstract,
using a low-level API, perhaps similar to the unix commands `cd` and
`ls`, and perhaps with some `open` and `close` as the walk is being
performed.

### Motor Control
The crawler can be thought of as a motor: it moves around from here to
there. Not in physical 3D space, but in a virtual world of filesystems.
Like all good motors, it needs a motor-control system. For crawlers,
this is conventionally a collection of rules about what kinds of data
to ignore, where not to go, how to bound the search to some limited
domain.

As a practical example, the `/etc/updatedb.conf` file in Linux systems
with slocate/plocate installed provides a list file system types that
should not be crawled (the `/tmp` direectory. NFS mounts. The `/proc`
filesystem) More sophisticated config files can be found in the
`/etc/systemd` folder. These config files are nice because they are
human-readable, human-editable and yet encode significant behavioral
constraints. Yet another example is the pervasive use of JSON and YAML
files to configure a vast variety of modern software systems.

So the controller needs a configurable control system. Here's the catch:
as the agent learns about the system it is exploring, it needs to be
able to modify this configuration. Since the agent is running
[Atomese](https://opencog.org/wiki/Atomese), the config info needs to be
in Atomese. This creates problems.
* Atomese is human-readable, but barely. It's verbose. There's a reason
  people use JSON and YAML but not s-expressions (or, god forbid, XML).
* The config is not to be stored in a file, because the config is itself
  a dynamic object, being acted on, updated and changed (by the agent).

Since we're bootstrapping, it's probably reasonable to start with a text
file holding the config, and ingest that. Those familiar with large
systems are aware of several problems. One is that typically, one needs
to have to manage multiple config files. Well, you can check them into
`git`, but now, it lives there, outside of where Atomese can touch it.

Many sysadmins like to use a GUI to manage their systems. These days,
the GUI is typically a web-based control panel. And, for this project,
it would also be nice to have a web-based control panel of some kind.
See how design requirements spiral out of control? All this for a
control system that becomes effectively obsolete, when the agent takes
over and starts manipulating the control file to it's own ends.

### Dynamic control
The above sketches the crawler in enough detail that the first part of
this project can be cleanly stated. The research task is:
* Define and develop a generic crawl API, so that generic hierarchies
  can be explored.
* Define and develop a control system that can guide the crawler to go
  to certain places, but not others.

This shouldn't be hard, but its also not easy.
* The API should be simple enough that human engineers could write shims
  or callbacks that adapt the crawl to a specific type of hierarchical
  structure.
* The control structures need to be in Atomese. Yes, they might start
  as simple text files, but after ingestion, they become Atomese, so
  that they become accessible to agents. Designing high-quality Atomese
  is a challenge.

Perception
----------
During the crawl, the agent needs to collect data. Perhaps it is as
simple as collecting the file aname, path, timestamp, filesize and
perhaps a hash of the file contents. But perhaps something else: perhaps
the file contents are to be processed in some way. All this needs to be
configurable.

The implementation would be to provide a callback for each new
encountered item, let the user decide what to do with it.

A fancier implementation would provide rule-driven decisions.
Given that the crawl itself is rule-driven, having the observation
be also seems reasonable.

Structural similarity
---------------------
The third (and final?) goal is a collection of tools to perform
structural analysis on the filesystem itself.

This last goal drives the project. Without this, crawling a filesystem
indeed becomes trivial, and this project becomes stupid.

Structural analysis is the "machine learning" part of this. What
paths are similar? What's different? What's the overall structure?
There's a richness of questions one can ask here, and a richness of
tools and algorithms that can provide answers. The task is to manage
the richness of these algos and the data they produce.

TBD. Explain this.


Status
------
* '''Version 0.0.4''' - Basic design is being developed. A basic
  demo mostly works, kind-of.

HOWTO
-----
Code is organized into two directories:
* Short [examples](examples) illustrating snippets of ideas. The
  examples all work, int that they're runnable, and they do what they
  claim to do. They are of a tutorial nature: You're supposed to copy
  the code into a scheme or python3 session, run it, see what it does,
  and ponder what it means.  The examples are all short, simple,
  bite-sixzed.
* Longer [demos](demos) which combine aspects of the examples into
  longer quasi-functional assemblies that do portions of what this
  README above discusses.

Prerequisites are
[AtomSpace](https://github.com/opencog/atomspace) and
[Sensory](https://github.com/opencog/sensory).
Build and install both of these.
