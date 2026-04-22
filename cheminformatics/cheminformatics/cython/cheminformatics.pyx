#!python
#cython: language_level=3

from opencog.atomspace import get_refreshed_types
from opencog.utilities import add_node, add_link


cdef extern :
    void cheminformatics_types_init()


cheminformatics_types_init()
types = get_refreshed_types() 

include "cheminformatics/types/cheminformatics_types.pyx"
