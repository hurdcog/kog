;;;     Copyright 2024 Li-Cheng (Andy) Tai
;;;                      atai@atai.org

;;;     based on guix.scm of guile-gi,
;;;     Copyright (C) 2019 Jan Nieuwenhuizen <janneke@gnu.org>

;;  This file is part of guile_llama_cpp.

;;; guile_llama_cpp is free software: you can redistribute it and/or modify it
;;; under the terms of the GNU Lesser General Public License as published by the
;;; Free Software Foundation, either version 3 of the License, or (at your
;;; option) any later version.

;;; guile_llama_cpp is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;;; or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
;;; License for more details.

;;; You should have received a copy of the GNU Lesser General Public License
;;; along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

;;; Commentary:
;;
;; GNU Guix development package.
;;
;; To build and install, run:
;;
;;   guix package -f guix.scm
;;
;; To build it, but not install it, run:
;;
;;   guix build -f guix.scm
;;
;; To use as the basis for a development environment, run:
;;
;;   guix shell -D -f guix.scm
;;
;;; Code:
(use-modules (guix packages)
             ((guix licenses) #:prefix license:)
             (guix git-download)
             (guix gexp)
             (guix build-system cmake)
             (guix build-system gnu)
             (guix packages)
             (guix utils)
             (gnu packages)
             (gnu packages autotools)
             (gnu packages gettext)
             (gnu packages guile)
             (gnu packages guile-xyz)
             (gnu packages libffi)
             (gnu packages machine-learning)
             (gnu packages pkg-config)
             (gnu packages swig)
             (gnu packages texinfo)
             (ice-9 match))

(define %source-dir (dirname (current-filename)))


(define-public llama-cpp-cpu-only
  (let ((base llama-cpp))
    (package/inherit base
      (arguments
        (substitute-keyword-arguments (package-arguments base)
          ((#:configure-flags configure-flags)
           #~(cons*  "-DCMAKE_POSITION_INDEPENDENT_CODE=TRUE" #$configure-flags))))
      (native-inputs (list pkg-config)))))


(package
  (name "guile_llama_cpp")
  (version "0.2")
  (source (local-file %source-dir
                        #:recursive? #t
                        #:select? (git-predicate %source-dir)))
  (build-system gnu-build-system)
  (arguments `())
  (native-inputs
    (list autoconf
      automake
      libtool
      pkg-config
      swig-next))
  (inputs (list
  		  guile-3.0
  		  guile-lib
  		  llama-cpp-cpu-only))
  (synopsis "")
  (description "")
  (home-page "")
  (license license:lgpl3+))

