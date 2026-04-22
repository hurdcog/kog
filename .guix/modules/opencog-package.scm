;;; OpenCog Collection (OCC) package definition for GNU Guix

(define-module (opencog-package)
  #:use-module (guix packages)
  #:use-module (guix download)
  #:use-module (guix git-download)
  #:use-module (guix gexp)
  #:use-module (guix build-system python)
  #:use-module (guix build-system cmake)
  #:use-module (guix build-system gnu)
  #:use-module (guix build-system trivial)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages)
  #:use-module (gnu packages python)
  #:use-module (gnu packages python-xyz)
  #:use-module (gnu packages python-science)
  #:use-module (gnu packages cmake)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages guile)
  #:use-module (gnu packages boost)
  #:use-module (gnu packages serialization)
  #:use-module (gnu packages databases)
  #:use-module (gnu packages version-control)
  #:use-module (gnu packages maths)
  #:use-module (gnu packages cpp)
  #:use-module (gnu packages check))

(define-public opencog-collection
  (package
    (name "opencog-collection")
    (version "0.1.0")
    (source (local-file "." "opencog-collection-checkout"
                        #:recursive? #t))
    (build-system cmake-build-system)
    (arguments
     `(#:tests? #f  ; Disable tests for now 
       #:configure-flags 
       ;; Fixed: Use quoted list instead of ,(list ...) to avoid Scheme error
       ;; "Wrong type to apply" - see commit 70c2752785a1b9316ffe03d09ebf81f4d01e6529
       '("-DCMAKE_BUILD_TYPE=Release"
         "-DBUILD_COGUTIL=ON"
         "-DBUILD_ATOMSPACE=ON"
         "-DBUILD_COGSERVER=ON"
         "-DBUILD_MATRIX=ON"
         "-DBUILD_LEARN=ON"
         "-DBUILD_AGENTS=ON"
         "-DBUILD_SENSORY=ON"
         "-DBUILD_ATOMSPACE_STORAGE=OFF"
         "-DBUILD_ATOMSPACE_EXTENSIONS=OFF")
       #:phases
       ,(modify-phases %standard-phases
          (add-before 'configure 'set-environment
            (lambda* (#:key inputs outputs #:allow-other-keys)
              (setenv "BOOST_ROOT" (assoc-ref inputs "boost"))
              (setenv "PKG_CONFIG_PATH" 
                      (string-append (assoc-ref inputs "pkg-config") "/lib/pkgconfig:"
                                   (or (getenv "PKG_CONFIG_PATH") "")))
              #t))
          (add-after 'install 'install-additional-components
            (lambda* (#:key inputs outputs #:allow-other-keys)
              (let* ((out (assoc-ref outputs "out"))
                     (bin (string-append out "/bin"))
                     (share (string-append out "/share/opencog-collection"))
                     (python (search-input-file inputs "/bin/python3")))
                (mkdir-p share)
                
                ;; Go back to source directory from build directory
                (chdir "..")
                
                ;; Install Python demo and documentation
                (when (file-exists? "app.py")
                  (install-file "app.py" share)
                  (call-with-output-file (string-append bin "/opencog-demo")
                    (lambda (port)
                      (format port "#!/bin/sh~%exec ~a ~a/app.py \"$@\"~%"
                              python share)))
                  (chmod (string-append bin "/opencog-demo") #o755))
                
                (when (file-exists? "README.md")
                  (install-file "README.md" share))
                #t))))))
    (native-inputs
     (list pkg-config
           cmake
           cxxtest))
    (inputs
     (list python
           python-numpy
           python-pandas
           python-scikit-learn
           python-matplotlib
           guile-3.0
           boost
           openblas
           lapack
           gsl))
    (propagated-inputs
     (list python-numpy
           python-pandas
           python-scikit-learn
           python-matplotlib))
    (home-page "https://github.com/cogpy/occ")
    (synopsis "OpenCog Collection - Machine Learning Integration Environment")
    (description
     "This package provides the OpenCog Collection monorepo - an integrated
development environment for cognitive computing and artificial general intelligence (AGI).
The collection brings together multiple OpenCog-related projects into a coherent
whole for cognitive synergy.

The package includes the core OpenCog components for building cognitive systems
and conducting AGI research.")
    (license license:mit)))

;; Export the package for use in other modules
opencog-collection