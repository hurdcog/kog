####
#### Copyright 2024 Li-Cheng (Andy) Tai
#### atai@atai.org.

#### This file is part of guile_llama_cpp.

#### guile_llama_cpp is free software: you can redistribute it and/or modify it
#### under the terms of the GNU Lesser General Public License as published by the
#### Free Software Foundation, either version 3 of the License, or (at your
#### option) any later version.

#### guile_llama_cpp is distributed in the hope that it will be useful, but
#### WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#### or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
#### License for more details.

#### You should have received a copy of the GNU Lesser General Public License
#### along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.
####

BUILT_SOURCES =  llama_wrap.cxx

EXTRA_DIST = llama.i guile_typemap.i notes.txt llama_c_header.h ecma262.scm
llama_wrap.cxx: $(abs_srcdir)/llama.i $(abs_srcdir)/guile_typemap.i $(abs_srcdir)/llama_c_header.h
	$(SWIG) -c++ -guile -Linkage module -I${C_INCLUDE_PATH} $(abs_srcdir)/llama.i

# reference: https://stackoverflow.com/a/5674708
# telling this is meant as a plugin (shared library plugin)
lib_LTLIBRARIES = libguile_llama_cpp.la

libguile_llama_cpp_la_SOURCES = $(BUILT_SOURCES)
libguile_llama_cpp_la_CPPFLAGS =  $(GUILE_CFLAGS) $(LLAMA_CFLAGS) -fPIC
libguile_llama_cpp_la_LIBADD = $(GUILE_LIBS) $(LLAMA_LIBS) -lm  -lstdc++
libguile_llama_cpp_la_LDFLAGS = -module -avoid-version -shared
