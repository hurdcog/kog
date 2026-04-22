# Examples

This directory contains examples of defining atoms, bonds, molecules
and reactions in [Atomese](https://wiki.opencog.org/w/Atomese)
using several different execution environments: 1) Scheme, 2) Python,
3) Python within Jupyter notebooks.  Recall that Atomese can be freely
mixed between scheme and python: under the covers, the data is always in
Atomese, and not in the programming language environment itself.
(See the examples in the core AtomSpace git repo for demos.)

## Python

Changedir to the `python` directory and use the Python 3 interpreter
to run scripts there:

```
$ python3 intro_example.py
```

This is the most basic demo: it shows how to create a single atom, a single
chemical bond, and a methane molecule.

If you run python virtualenv, and are experiencing issues with undefined
symbols, then try adding `/usr/local/lib/python3.11/dist-packages/`
to your `PYTHON_PATH` and adding `/usr/local/lib/opencog/` to your
`LD_LIBRARY_PATH`.

## Scheme

The scheme directory contains a much more complex example. It uses
the AtomSpace query engine to define a rewrite rule that performs
an esterification reaction. Although written in scheme, it could also
be re-written in python, by re-arranging the parenthesis. (This isn't
hard, just tedious.)

Changedir to the `scheme` directory and use the Guile interpreter
to run the demo:

```
$ guile -s reaction.scm
```

In order to monkey with the example, it can be more convenient to
work at the command prompt, and cut-n-paste portions of the example
into the guile REPL:
```
$ guile
(use-modules (opencog) (opencog cheminformatics))
(load "reaction.scm")
```
The current examples are:
* [reaction.scm](scheme/reaction.scm) - An esterification reaction.

## Jupyter notebooks

Change to the `jupyter` directory, start a Jupyter server and use the
automatically opened web GUI to inspect and run a notebook:

```
$ jupyter-notebook
```
