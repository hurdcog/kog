Crawler Design Notes
--------------------
This README ponders different design ideas for the crawler, and sketches
out the pros and cons of for various design decisions.

The goal is to explore the concept of crawling a collection of
hirarchical data, or a "forest of trees". The prototype crawler, and
much of the discussion below will be aimed at filesystems as an example
of a collection of trees. The general design is meant to be general,
so that it can work for any kind of tree-like data.

Crawl Representation
--------------------
Before looking at the crawler design itself, its worth taking a quick
glance at the artifacts that will be produced. This will be a
representation of the file system in Atomese, along with properties
associated with those files.

A design goal here is to be relatively agnostic with regard to that
representation. Atomese allows for both deeply nested and very flat
representations of data. The nested representations correspond naturally
to hierarchical tree structures. However, flattened representations seem
to work better for high-performance algorithmic processing. A flattened
representation corresponds roughly to the idea of an
"[inode](https://en.wikipedia.org/wiki/inode)" in a unix file system.
The inode number itself need not be an integer. The desirable properties
of an inode number are provided by a URL: It uniquely represents the
indicated object, and it can be computed without access to a central
URL-dispensing authority (its "decentralized").

Data-transformation Applets
---------------------------
As a general rule, the data available is not in the format that some
given algorithm wants it in. This is true not just in "real life" but in
Atomese as well. Thus, the successful deployment of algorithms requires
the use of data-transforming applets ("transformers") that can accept
data in one format, and convert it to another format.

In "real life", you ask a programmer, admin or DBA to do this for you.
This clearly is the wrong answer in the present case.

Such converters have an input, and an output. The format of the input
and the output need to be described in machine-readable terms. There
also needs to be an algorithm that can connect an input to an output, to
build a functioning machine. The design of such a system is the topic
of the [Sensory project](https://github.com/opencog/sensory). At any
rate, it is challenging, and presents both conceptual issues, and
practical problems w.r.t. performance.

Data transformation has both a "narrow" and a "broad" interpretation.
In the narrow sense, this is nothing more than term rewriting and data
stream filtering. The Atomese
[FilterLink](https://opencog.org/wiki/FilterLink) provides a mechanism
for rewriting data streams, by applying
[RuleLink](https://opencog.org/wiki/RuleLink)s to them. These are
dataflow analogs to the Atomose
[QueryLink](https://opencog.org/wiki/QueryLink), which loosely
corresponds to the SQL concept of a query. Queries are for finding and
rewriting "static" data that sits in the AtomSpace. The Filters are
for modifying dynamically-flowing data.

Broad rewrites are provided by neural nets, which scale to much higher
dimensions than are available either with FilterLink or QueryLink.

Directory Listing
-----------------
To obtain a directory listing, one must `cd` to the directory, open it,
and then iterate over contents. Lets assume the `cd` step is completed,
and that we have a handle to the location. Lets assume the handle is of
the form
```
   (ItemNode "/some/location/some/where")
```
The string in the above node acts as an inode: it's opaque to the user,
but meaningful to the directory lister. Instead of slashes, it might
have dots (as in python modules locations) or blank spaces (as in many
command-line tools, where subcommands are separated by spaces). There
may be dashes (single dashes or double-dashes, used to pass flags,
options, parameters to some command-line tool.) Thus:
```
   (ItemNode "ceph --cluster enfield config set mgr.fanny debug_mgr 0/5")
   (ItemNode "from sys import foo.bar.baz")
	(ItemNode "47e2d7bd-e3ea-4338-bb47-78ffe9efec65")
```
are all interpretable as locations within some hierarchihcal tree, and
the proper semantics & decoding of "what it means" is left to the module
that knows how to walk the system.

The attitude being taken here is similar to that of Plan9, where
"everything is a file", except here, "everything is a filepath" and
"we dont know what that filepath string means".

### Demo and discussion
The `Sensory` project `examples/filesys.scm` has a demo of working
with a file system. That dome is in scheme, its ported to python
in the current `examples` dir.

Reviewing that example here. The first step is to open the stream,
like so
```
   (OpenLink
      (TypeNode 'FileSysStream)
      (SensoryNode "file:///tmp"))
```
The result is a stream to which assorted commands can be issued. The
set of available commands is provided by the `LookatLink`, see the
Sensory docs for more. For now, we assume we just know what the
possible commands are.

A file listing can be obtained by issuing
```
   (WriteLink stream (ItemNode "ls"))
```
where the `(ItemNode "ls")` is one of the available commands that were
provided by the `LookatLink`.  The return value is a sequence of
directory entries, encoded as `StringValue`. Go run the demo, it will be
clear.

As a stream of `StringValue`, its fairly useless. We'll want to perform
one of several operations on it:
* Convert `StringValue`s to `Node`s, so that the AtomSpace can store
  these.
* Obtain the file type, the file size, the file mtime and atime, all the
  stuff that unix `fstat` normally returns.
* Perform some custom operation on the file contents: e.g. computing a
  content hash, or, if its a music file, playing it, or a video,
  watching it.

Each of these operations is explored in turn, below.

### Converting Values to Nodes
A primary task is to convert Stream data into Atom data. This is the
very simplest example of a data transformation applet, discussed above.
For the present example, the stream contains
```
   (StringValue "file:///tmp/foo/bar")
```
and we wish to convert this to
```
   (EdgeLink
      (PredicateNode "URL")
      (ItemNode "file:///tmp/foo/bar"))
```
The reason we want to do this is so that we can, at a later time,
perform a query on the AtomSpace and find (for example) all URL's that
had been observed at some point in time. In this example:
```
   (Meet
      (EdgeLink
         (PredicateNode "URL")
         (VariableNode "$query-result")))
```
will provide a listing of all URL's held in the AtomSpace.

This is nothing more than a stream rewrite. The `FilterLink` can process
streams, with the rewrite defined by a `RuleLink`. This is demoed in the
examples directory, in the [examples/rewrite.scm](examples/rewrite.scm)
demo.

### Getting Metadata
The file size and modification date are considered to be part of the
"metadata" associated with a file. Linux file systems record a variety
of file metadata, accessible with the `fstat()` system call. Additional
attributes include file ownership, access permissions, and extensions
provided by `lsattr` and similar. Any kind of collection of trees will
typically have metadata associated with each tree, branch or leaf. There
must be a way of accessing this data.

Metadata may be of two types: stored statically with the filesystem, and
generated dynamically, on the fly. Thus, the filesize and modification
date are stored statically, as a part of the filesystem itself. The file
mime-type is not: it can only be obtained by running some sort of
external tool. File magic is extracted by looking at the file contents.

The task of the crawler is not only to visit the various trees in the
tree collection, but to also gather data as it visits. The gathered data
is then recorded "in memory", i.e. to the AtomSpace. That is, attached
to the crawler is a "metadata perception system", which makes
observations on what the crawler finds. This perception system must be
configurable: the first time through, one might be looking for one
thing; the second time through, something else. The focus of attention
might even change as the crawl is ongoing.

Perception control and attentional focus is distinct from motor control.
Motor control determines what trees and branches are visited; perception
control determines  what is observed when a specific location is
visited.
