dnl -*- Autoconf -*-

define(GUILE_LLAMAPCPP_CONFIGURE_COPYRIGHT,[[

Copyright 2024 Li-Cheng (Andy) Tai
atai@atai.org

    This file is part of guile_llama_cpp.

    guile_llama_cpp is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    guile_llama_cpp is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

]])


AC_INIT(guile_llama_cpp, 0.2)

AC_SUBST(VERSION, "\"0.2\"")
AC_SUBST(AUTHOR, "\"Andy Tai\"")
AC_SUBST(COPYRIGHT, "'(2024)")
AC_SUBST(LICENSE, lgpl3+)
AC_COPYRIGHT(GUILE_LLAMAPCPP_CONFIGURE_COPYRIGHT)

AC_CONFIG_SRCDIR(src/llama.i)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.12 gnu silent-rules subdir-objects  color-tests parallel-tests -Woverride -Wno-portability])
AM_SILENT_RULES([yes])

AC_GNU_SOURCE

AC_PROG_MAKE_SET

AC_PROG_INSTALL

AC_PROG_CXX
LT_INIT

AX_PKG_SWIG(
    [4.0],
    [
        AC_MSG_NOTICE([Found SWIG version $SWIG_VERSION])
        AC_SUBST([SWIG])     # Make SWIG path available to Makefiles
        AC_SUBST([SWIG_LIB]) # Make SWIG library path available to Makefiles
    ],
    [
        AC_MSG_ERROR([SWIG version 4.0 or newer is required.])
    ]
)
AX_SWIG_ENABLE_CXX

dnl check for existenve of llama-cpp; TO DO
dnl for now, assuming it exists; no check
AC_SUBST([LLAMA_CFLAGS])
AC_SUBST([LLAMA_LIBS], "-lllama")


PKG_CHECK_MODULES([GUILE], [guile-3.0 >= 3.0],
                  [
                    # Guile is found
                    AC_DEFINE([HAVE_GUILE], [1], [Define if Guile is available])
                    # Optional: Store Guile-related info in variables
                    GUILE_CFLAGS="$GUILE_CFLAGS"
                    GUILE_LIBS="$GUILE_LIBS"
                  ],
                  [
                    # Guile is not found
                    AC_MSG_ERROR([Guile 3.0 or newer is required.])
                  ])

dnl Search for 'guile' and 'guild'.  This macro defines
dnl 'GUILE_EFFECTIVE_VERSION'.
GUILE_PKG([3.0])
GUILE_PROGS
GUILE_SITE_DIR
if test "x$GUILD" = "x"; then
   AC_MSG_ERROR(['guild' binary not found; please check your guile-3.x installation.])
fi

dnl Hall auto-generated guile-module dependencies


dnl Installation directories for .scm and .go files.
guilemoduledir="${datarootdir}/guile/site/$GUILE_EFFECTIVE_VERSION"
guileobjectdir="${libdir}/guile/$GUILE_EFFECTIVE_VERSION/site-ccache"
AC_SUBST([guilemoduledir])
AC_SUBST([guileobjectdir])

AC_CONFIG_FILES([Makefile] [src/Makefile])
AC_CONFIG_FILES([pre-inst-env], [chmod +x pre-inst-env])

AC_OUTPUT


echo "
$PACKAGE_NAME $PACKAGE_VERSION configuration:
-----------------------------
  Source code location:       ${srcdir}
  Host System Type:           ${host}
  Compiler:                   ${CC}
  Standard CFLAGS:            ${CFLAGS} ${GUILE_CFLAGS} ${LLAMA_CFLAGS} ${ac_devel_default_warnings}
  Libraries:                  ${GUILE_LIBS} ${LLAMA_LIBS}
  Install path (prefix):      ${prefix}

  Now type 'make' to build $PACKAGE_NAME $PACKAGE_VERSION.
"
