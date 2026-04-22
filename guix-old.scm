;;; Top-level Guix package file for OpenCog Collection
;;; Use with: guix build -f guix.scm

(use-modules (guix packages)
             (guix download)
             (guix git-download)
             (guix build-system python)
             (guix build-system cmake)
             (guix build-system gnu)
             (guix build-system trivial)
             ((guix licenses) #:prefix license:)
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
             (gnu packages maths)
             (gnu packages cpp)
             (gnu packages check))

(define-public opencog-collection
  (package
    (name "opencog-collection")
    (version "0.1.0")
    (source (local-file "." "opencog-collection-checkout"
                        #:recursive? #t))
    (build-system cmake-build-system)
    (arguments
     `(#:tests? #f  ; Disable tests for now as they may require network access
       #:configure-flags 
       ,(list "-DCMAKE_BUILD_TYPE=Release"
              "-DCMAKE_INSTALL_PREFIX=/var/www/opencog-collection"  ; SSR server-side deployment path
              "-DBUILD_COGUTIL=ON"
              "-DBUILD_ATOMSPACE=ON"
              "-DBUILD_COGSERVER=ON"
              "-DBUILD_MATRIX=ON"
              "-DBUILD_LEARN=ON"
              "-DBUILD_AGENTS=ON"
              "-DBUILD_SENSORY=ON"
              "-DBUILD_ATOMSPACE_STORAGE=OFF"  ; Disable storage for now to reduce complexity
              "-DBUILD_ATOMSPACE_EXTENSIONS=OFF")
       #:phases
       ,(modify-phases %standard-phases
          (add-before 'configure 'set-environment
            (lambda* (#:key inputs outputs #:allow-other-keys)
              ;; Set up environment for building
              (setenv "BOOST_ROOT" (assoc-ref inputs "boost"))
              (setenv "PKG_CONFIG_PATH" 
                      (string-append (assoc-ref inputs "pkg-config") "/lib/pkgconfig:"
                                   (getenv "PKG_CONFIG_PATH")))
              #t))
          (add-after 'install 'install-python-components
            (lambda* (#:key outputs #:allow-other-keys)
              (let* ((out (assoc-ref outputs "out"))
                     (bin (string-append out "/bin"))
                     (python-sitedir (string-append out
                                                   "/lib/python"
                                                   ,(version-major+minor (package-version python))
                                                   "/site-packages")))
                ;; Install Python demo application
                (mkdir-p python-sitedir)
                (install-file "app.py" (string-append out "/share/opencog-collection/"))
                
                ;; Create wrapper script for Python demo
                (call-with-output-file (string-append bin "/opencog-demo")
                  (lambda (port)
                    (format port "#!/bin/sh
export PYTHONPATH=~a:$PYTHONPATH
exec ~a ~a/app.py \"$@\"~%"
                            python-sitedir
                            (which "python3")
                            (string-append out "/share/opencog-collection"))))
                (chmod (string-append bin "/opencog-demo") #o755)
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

The package includes the core OpenCog components:
@itemize
@item CogUtil - Base utilities and configuration system
@item AtomSpace - Hypergraph database and query engine  
@item CogServer - Networking and communication layer
@item Matrix - Sparse vector and graph processing
@item Learn - Symbolic learning algorithms
@item Agents - Interactive cognitive agents
@item Sensory - Dataflow system for external world interaction
@end itemize

Additionally includes:
@itemize
@item Python-based machine learning demonstration using scikit-learn
@item Complete source for research and development
@item Development environment for cognitive computing applications
@end itemize")
    (license license:mit)))

;; Return the package for building
opencog-collection
