Cheminformatics
===============

This is an early attempt to use OpenCog for cheminformatics.

Building and Installing
=======================
To build the code, you will need to build and install the
[OpenCog AtomSpace](https://github.com/opencog/atomspace) first.
All of the pre-requistes listed there are sufficient to also build
this project. Building is as "usual":
```
    cd to project root dir
    mkdir build
    cd build
    cmake ..
    make -j
    sudo make install
    make -j test
```

Examples
========
Examples can be found in the [examples](examples) directory.

If you run python virtualenv, and are experiencing issues with undefined
symbols, then try adding `/usr/local/lib/python3.11/dist-packages/`
to your `PYTHON_PATH` and adding `/usr/local/lib/opencog/` to your
`LD_LIBRARY_PATH`.
