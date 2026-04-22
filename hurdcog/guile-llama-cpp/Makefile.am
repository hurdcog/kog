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

SUBDIRS = src

bin_SCRIPTS = scripts/chat.scm \
    scripts/simple.scm \
    scripts/tokenize.scm \
    scripts/ecma262-bridge.scm

NOCOMP_SOURCES = $(bin_SCRIPTS) scripts/init_common.scm \
    scripts/utils.scm

SOURCES = src/ecma262.scm


# Handle substitution of fully-expanded Autoconf variables.
do_subst = $(SED)					\
  -e 's,[@]GUILE[@],$(GUILE),g'				\
  -e 's,[@]guilemoduledir[@],$(guilemoduledir),g'	\
  -e 's,[@]guileobjectdir[@],$(guileobjectdir),g'	\
  -e 's,[@]localedir[@],$(localedir),g'

noinst_SCRIPTS = pre-inst-env

GOBJECTS = $(SOURCES:%.scm=%.go)

moddir=$(prefix)/share/guile/site/$(GUILE_EFFECTIVE_VERSION)
godir=$(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/site-ccache
ccachedir=$(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/site-ccache

nobase_mod_DATA = $(SOURCES) $(NOCOMP_SOURCES)
nobase_go_DATA = $(GOBJECTS)

# Make sure source files are installed first, so that the mtime of
# installed compiled files is greater than that of installed source
# files.  See
# <http://lists.gnu.org/archive/html/guile-devel/2010-07/msg00125.html>
# for details.
guile_install_go_files = install-nobase_goDATA
$(guile_install_go_files): install-nobase_modDATA

EXTRA_DIST = $(SOURCES) $(NOCOMP_SOURCES) License docs/ECMA262_INTEGRATION.md examples/ecma262-llm-example.js

GUILE_WARNINGS = -Wunbound-variable -Warity-mismatch -Wformat
SUFFIXES = .scm .go
.scm.go:
	$(AM_V_GEN)$(top_builddir)/pre-inst-env $(GUILE_TOOLS) compile $(GUILE_WARNINGS) -o "$@" "$<"


TESTS =

TEST_EXTENSIONS = .scm
SCM_LOG_DRIVER =                                \
  $(top_builddir)/pre-inst-env                  \
  $(GUILE) --no-auto-compile -e main            \
      $(top_srcdir)/build-aux/test-driver.scm

# Tell 'build-aux/test-driver.scm' to display only source file names,
# not indivdual test names.
AM_SCM_LOG_DRIVER_FLAGS = --brief=yes

AM_SCM_LOG_FLAGS = --no-auto-compile -L "$(top_srcdir)"

AM_TESTS_ENVIRONMENT = abs_top_srcdir="$(abs_top_srcdir)"

EXTRA_DIST += README.md \
              COPYING

ACLOCAL_AMFLAGS = -I m4

clean-go:
	-$(RM) $(GOBJECTS)
.PHONY: clean-go

CLEANFILES =					\
  $(GOBJECTS)					\
  $(TESTS:tests/%.scm=%.log)
