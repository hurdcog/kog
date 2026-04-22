#! /usr/bin/env python3
#
# intro_example.py
#
# An introductory example for using the Python bindings to the
# cheminformatics package.  Creates some simple molecules.
#
# --------------------------------------------------------------

# Import the AtomSpace, and the basic AtomSpace types
from opencog.atomspace import AtomSpace
from opencog.type_constructors import *

# Import all of the chemical element types, and bond types too.
from opencog.cheminformatics import *

# Nothing works without a default AtomSpace, so create that first.
spa = AtomSpace()
set_default_atomspace(spa)

print(('Hello! The AtomSpace is ' + str(spa)))

# Lets create a Helium atom named 'foo'
He('foo')

# Uhh, well, lets grab hold of it
x = He('foo')

print(('The Helium atom is ' + str(x)))

# Let's create a single bond between a Carbon and a Hydrogen
# The atoms must have names. The names can be anything.
ch = SB(C('some carbon atom'), H('just a proton, ok?'))

print(('The CH bond is ' + str(ch)))

# Let's create a methane molecule. The carbon and the hydrogens will all
# be numbered.
methane = Molecule( \
    SB(C('1'), H('1')), \
    SB(C('1'), H('2')), \
    SB(C('1'), H('3')), \
    SB(C('1'), H('4')))

print(('Methane is a molecule: ' + str(methane)))

# The end.
# That's all, folks!
