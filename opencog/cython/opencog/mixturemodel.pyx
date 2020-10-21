from opencog.atomspace import types
from cython.operator cimport dereference as deref, preincrement as inc
from opencog.atomspace cimport cHandle, Atom, AtomSpace, TruthValue
from ure cimport cMixtureModel

# Create a Cython extension type which holds a C++ instance
# as an attribute and create a bunch of forwarding methods
# Python extension type.


cdef class MixtureModel:
    cdef cMixtureModel * mixture_model
    def __cinit__(self, set[Handle] models, 
