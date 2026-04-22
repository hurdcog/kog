;;; OpenCog package definition for packaging
(use-modules (guix packages)
             (guix git-download)
             (guix build-system gnu)
             (guix build-system cmake)
             (guix build-system python)
             ((guix licenses) #:prefix license:)
             (gnu packages base)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages cmake)
             (gnu packages pkg-config)
             (gnu packages guile)
             (gnu packages boost)
             (gnu packages maths)
             (gnu packages check))

(define-public opencog
  (package
    (name "opencog")
    (version "latest-git")
    (source (git-checkout
             (url "https://github.com/cogpy/occ.git")
             (commit "HEAD")))
    (build-system cmake-build-system)
    (arguments
     `(#:tests? #f  ; Disable tests for now
       #:configure-flags 
       ;; Fixed: Use quoted list instead of ,(list ...) to avoid Scheme error
       ;; "Wrong type to apply" - see commit 70c2752785a1b9316ffe03d09ebf81f4d01e6529
       '("-DCMAKE_BUILD_TYPE=Release"
         "-DBUILD_COGUTIL=ON"
         "-DBUILD_ATOMSPACE=ON"
         "-DBUILD_COGSERVER=ON")
       #:phases
       ,(modify-phases %standard-phases
          (add-before 'configure 'set-environment
            (lambda* (#:key inputs outputs #:allow-other-keys)
              (setenv "BOOST_ROOT" (assoc-ref inputs "boost"))
              #t)))))
    (native-inputs
     (list pkg-config
           cmake))
    (inputs
     (list python
           guile-3.0
           boost
           openblas
           lapack
           gsl))
    (synopsis "OpenCog AGI Framework")
    (description "The OpenCog cognitive architecturing toolkit for AGI research and development.")
    (home-page "https://github.com/cogpy/occ")
    (license license:mit)))

;; Return the package for building
opencog