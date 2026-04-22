Examples
--------
A collection of examples are given below. These all "work", and
demonstrate various ideas that the README's talk about. They veer
towards the experimental: although they illustrate key ideas and
concepts, they are subject to change. This is not the final design.

As with most demos, these are meant to be read, and then cut-n-pasted
into a guile or python3 REPL shell, played with and modified as desired.

* `filesys-api.py` -- An example of looking at directory contents,
  using the Atomese python bindings. This is a port of the first portion
  of the `filesys.scm` example in the Sensory project.

* `filesys-api.scm` -- Same as above, but in scheme.

* `rewrite.scm` -- Take a stream of inflowing strings, and rewrite them
  into Nodes. This is much like the conventional AtomSpace flow
  filtering and rewriting demos, here adapted to the FileSysStream
  object.
