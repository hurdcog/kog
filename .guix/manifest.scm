;;; Guix manifest for OpenCog Collection development environment
;;; Use with: guix shell -m .guix/manifest.scm

(use-modules (gnu)
             (gnu packages)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages python-science)
             (gnu packages cmake)
             (gnu packages pkg-config)
             (gnu packages guile)
             (gnu packages boost)
             (gnu packages serialization)
             (gnu packages databases)
             (gnu packages version-control)
             (gnu packages text-editors)
             (gnu packages base)
             (gnu packages maths)
             (gnu packages cpp)
             (gnu packages check)
             (gnu packages compression)
             (gnu packages tls))

;; Development environment manifest with all necessary tools and dependencies
(specifications->manifest
 '(;; Build tools and compilers
   "cmake"
   "pkg-config"
   "gcc-toolchain"
   "make"
   "autoconf"
   "automake"
   "libtool"
   
   ;; C++ development dependencies
   "boost"
   "cxxtest"
   "protobuf"
   "gsl"
   "blas"
   "lapack"
   "eigen"
   "zlib"
   "openssl"
   
   ;; Dependencies for external integrations
   ;; Gnucash dependencies
   "gtk+"
   "libxml2"
   "sqlite"
   "webkitgtk"
   "libdbi"
   
   ;; KoboldCpp dependencies  
   "openblas"
   "llvm"
   
   ;; Aphrodite Engine dependencies
   "cuda-toolkit"
   "python-torch"
   "python-transformers"
   "python-fastapi"
   "python-uvicorn"
   "python-aiohttp"
   
   ;; Python and scientific computing
   "python"
   "python-pip"
   "python-setuptools"
   "python-wheel"
   "python-numpy"
   "python-pandas"
   "python-scikit-learn"
   "python-matplotlib"
   "python-jupyter"
   "python-ipython"
   "python-pytest"
   
   ;; Guile and Scheme development
   "guile"
   "guile-readline"
   "guile-json"
   
   ;; Development utilities
   "git"
   "which"
   "grep"
   "sed"
   "coreutils"
   "findutils"
   "tar"
   "gzip"
   
   ;; Optional: text editors for development
   "emacs-minimal"
   "vim"))