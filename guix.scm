;;; Top-level Guix package file for OpenCog Collection
;;; Use with: guix build -f guix.scm
;;; 
;;; This version includes fixes for:
;;; - Proper module imports to avoid "unbound variable" errors
;;; - Optimized build configuration for complex multi-component builds
;;; - Better handling of Cargo.toml files in the repository

(use-modules (guix packages)
             (guix download)
             (guix git-download)
             (guix gexp)
             (guix build-system python)
             (guix build-system cmake)
             (guix build-system gnu)
             (guix build-system trivial)
             ((guix licenses) #:prefix license:)
             (gnu packages)
             (gnu packages base)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages python-science)
             (gnu packages machine-learning)
             (gnu packages cmake)
             (gnu packages pkg-config)
             (gnu packages guile)
             (gnu packages boost)
             (gnu packages serialization)
             (gnu packages databases)
             (gnu packages version-control)
             (gnu packages maths)
             (gnu packages algebra)
             (gnu packages cpp)
             (gnu packages check)
             (gnu packages build-tools)
             (gnu packages commencement)
             (gnu packages datastructures))  ;; For sparsehash

(define-public opencog-collection
  (package
    (name "opencog-collection")
    (version "0.1.0")
    (source (local-file "." "opencog-collection-checkout"
                        #:recursive? #t
                        #:select? (lambda (file stat)
                                    ;; Exclude build artifacts, cache directories, and Rust files
                                    ;; to prevent Guix from trying to detect rust-cargo build system
                                    (not (or (string-contains file "/.git/")
                                             (string-contains file "/build/")
                                             (string-contains file "/.cache/")
                                             (string-contains file "/target/")
                                             (string-contains file "/__pycache__/")
                                             (string-suffix? ".pyc" file)
                                             (string-suffix? ".o" file)
                                             (string-suffix? ".so" file)
                                             ;; Exclude Cargo files to prevent rust detection
                                             (string-suffix? "/Cargo.toml" file)
                                             (string-suffix? "/Cargo.lock" file)
                                             (string-contains file "/node_modules/")
                                             (string-contains file "/.vscode/")
                                             (string-contains file "/.devcontainer/"))))))
    (build-system cmake-build-system)
    (arguments
      `(#:tests? #f  ; Disable tests for now as they may require network access
        #:configure-flags
        ;; Fixed: Use quoted list instead of ,(list ...) to avoid Scheme error
        ;; "Wrong type to apply" - see commit 70c2752785a1b9316ffe03d09ebf81f4d01e6529
        '("-DCMAKE_BUILD_TYPE=Release"
          "-DCMAKE_VERBOSE_MAKEFILE=ON"
          "-DBUILD_COGUTIL=ON"
          "-DBUILD_ATOMSPACE=ON"
          "-DBUILD_COGSERVER=ON"
          "-DBUILD_MATRIX=ON"
          "-DBUILD_LEARN=ON"
          "-DBUILD_AGENTS=ON"
          "-DBUILD_SENSORY=ON"
          "-DBUILD_COGGML=ON"
          "-DBUILD_COGSELF=ON"
          "-DBUILD_ATOMSPACE_ACCELERATOR=ON"
          "-DBUILD_AGENTIC_CHATBOTS=ON"
          "-DBUILD_ATOMSPACE_STORAGE=OFF"
          "-DBUILD_ATOMSPACE_EXTENSIONS=OFF"
          "-DBUILD_GNUCASH=OFF"
          "-DBUILD_KOBOLDCPP=OFF"
          "-DBUILD_APHRODITE=OFF")
        #:phases
        (modify-phases %standard-phases
          (add-after 'unpack 'set-source-permissions
            (lambda _
              ;; Ensure all source files are readable and directories accessible
              (for-each (lambda (file)
                          (when (file-exists? file)
                            (chmod file #o644)))
                        (find-files "." ".*" #:directories? #f))
              (for-each (lambda (dir)
                          (when (file-exists? dir)
                            (chmod dir #o755)))
                        (find-files "." ".*" #:directories? #t))
              #t))
          (add-after 'unpack 'show-build-info
            (lambda _
              (format #t "~%=== OpenCog Collection Build Info ===~%")
              (format #t "Source directory: ~a~%" (getcwd))
              (format #t "Directory contents:~%")
              (system* "ls" "-la")
              (format #t "~%Checking for CMakeLists.txt:~%")
              (system* "ls" "-la" "CMakeLists.txt")
              (format #t "~%Checking subdirectories:~%")
              (for-each (lambda (dir)
                          (when (file-exists? (string-append dir "/CMakeLists.txt"))
                            (format #t "  ✓ ~a has CMakeLists.txt~%" dir)))
                        '("cogutil" "coggml" "atomspace" "atomspace-accelerator" 
                          "cogserver" "matrix" "learn" "agents" "sensory" 
                          "agentic-chatbots" "cogself"))
              (format #t "=====================================~%~%")
              #t))
          (add-before 'configure 'prepare-build-environment
            (lambda* (#:key inputs outputs #:allow-other-keys)
              ;; Set up environment variables for the build
              (let ((boost (assoc-ref inputs "boost"))
                    (guile (assoc-ref inputs "guile"))
                    (pkg-config (assoc-ref inputs "pkg-config"))
                    (sparsehash (assoc-ref inputs "sparsehash")))
                (when boost
                  (setenv "BOOST_ROOT" boost)
                  (format #t "BOOST_ROOT set to: ~a~%" boost))
                (when guile
                  (setenv "GUILE_LOAD_PATH" 
                          (string-append guile "/share/guile/site/3.0"))
                  (setenv "GUILE_LIBRARIES"
                          (string-append guile "/lib/libguile-3.0.so"))
                  (setenv "GUILE_INCLUDE_DIRS"
                          (string-append guile "/include/guile/3.0"))
                  (format #t "GUILE_LOAD_PATH set to: ~a~%" 
                          (string-append guile "/share/guile/site/3.0"))
                  (format #t "GUILE_LIBRARIES set to: ~a~%" 
                          (string-append guile "/lib/libguile-3.0.so"))
                  (format #t "GUILE_INCLUDE_DIRS set to: ~a~%" 
                          (string-append guile "/include/guile/3.0")))
                (when pkg-config
                  (setenv "PKG_CONFIG_PATH"
                          (string-append pkg-config "/lib/pkgconfig"))
                  (format #t "PKG_CONFIG_PATH set to: ~a~%" 
                          (string-append pkg-config "/lib/pkgconfig")))
                (when sparsehash
                  (format #t "SparseHash available at: ~a~%" sparsehash)))
              
              ;; Display diagnostic information
              (format #t "~%=== Pre-build Diagnostics ===~%")
              (format #t "CMake version:~%")
              (system* "cmake" "--version")
              
              ;; Check Guile version
              (format #t "~%Guile version:~%")
              (system* "guile" "--version")
              
              ;; Verify dependencies in store (with error handling)
              (format #t "~%Checking for dependencies in GNU store:~%")
              (let ((guile-check (system "ls -d /gnu/store/*guile* 2>/dev/null"))
                    (sparse-check (system "ls -d /gnu/store/*sparsehash* 2>/dev/null")))
                (if (zero? guile-check)
                    (format #t "  ✓ Guile found in store~%")
                    (format #t "  ⚠ Guile not found in store (may be expected)~%"))
                (if (zero? sparse-check)
                    (format #t "  ✓ SparseHash found in store~%")
                    (format #t "  ⚠ SparseHash not found in store (may be expected)~%")))
              
              (format #t "~%=================================~%~%")
              #t))
          (replace 'configure
            (lambda* (#:key configure-flags inputs outputs #:allow-other-keys)
              ;; Custom configure phase with better error handling
              (let ((out (assoc-ref outputs "out"))
                    (guile (assoc-ref inputs "guile")))
                (format #t "~%=== Starting CMake Configuration ===~%")
                (format #t "Output directory: ~a~%" out)
                (format #t "Configure flags: ~a~%" configure-flags)
                
                (mkdir-p "../build")
                (chdir "../build")
                
                (format #t "~%Running cmake...~%")
                ;; Build cmake args with explicit Guile paths if available
                (let ((cmake-args (append
                                   (list (string-append "-DCMAKE_INSTALL_PREFIX=" out))
                                   (if guile
                                       (list
                                        (string-append "-DGUILE_LIBRARIES=" guile "/lib/libguile-3.0.so")
                                        (string-append "-DGUILE_INCLUDE_DIRS=" guile "/include/guile/3.0"))
                                       '())
                                   configure-flags
                                   (list "../source"))))
                  (format #t "Full cmake command: cmake ~{~a ~}~%" cmake-args)
                  (apply invoke "cmake" cmake-args))
                
                (format #t "~%=== CMake Configuration Complete ===~%~%")
                #t)))
          (replace 'build
            (lambda* (#:key parallel-build? #:allow-other-keys)
              ;; Build with all available cores and verbose output
              (let ((job-count (if parallel-build?
                                   (number->string (parallel-job-count))
                                   "1")))
                (format #t "~%=== Starting Build ===~%")
                (format #t "Using ~a parallel jobs~%" job-count)
                (format #t "~%")
                
                ;; Use invoke instead of system* for better error handling
                (invoke "make" (string-append "-j" job-count) "VERBOSE=1")
                
                (format #t "~%=== Build Complete ===~%~%")
                #t)))
          (add-after 'build 'validate-build-artifacts
            (lambda _
              (format #t "~%=== Validating Build Artifacts ===~%")
              
              ;; Check for expected libraries
              (define expected-libs
                '("libcogutil" "libatomspace" "libcogserver" 
                  "libmatrix" "liblearn" "libagents" "libsensory"))
              
              (for-each
                (lambda (lib)
                  (let ((lib-file (string-append lib ".so")))
                    (if (or (file-exists? (string-append "opencog/" lib-file))
                            (file-exists? (string-append "cogutil/" lib-file))
                            (file-exists? (string-append "atomspace/" lib-file))
                            (file-exists? (string-append "cogserver/" lib-file))
                            (file-exists? (string-append "matrix/" lib-file))
                            (file-exists? (string-append "learn/" lib-file))
                            (file-exists? (string-append "agents/" lib-file))
                            (file-exists? (string-append "sensory/" lib-file)))
                        (format #t "  ✓ Found ~a~%" lib)
                        (format #t "  ⚠ Warning: ~a not found (may be expected)~%" lib))))
                expected-libs)
              
              (format #t "~%=== Build Artifact Validation Complete ===~%~%")
              #t))
          (replace 'install
            (lambda* (#:key outputs #:allow-other-keys)
              (format #t "~%=== Starting Installation ===~%")
              
              ;; Run make install
              (invoke "make" "install")
              
              (format #t "~%=== Installation Complete ===~%~%")
              #t))
          (add-after 'install 'install-additional-components
            (lambda* (#:key inputs outputs #:allow-other-keys)
              (let* ((out (assoc-ref outputs "out"))
                     (bin (string-append out "/bin"))
                     (lib (string-append out "/lib"))
                     (share (string-append out "/share/opencog-collection"))
                     (guile-site (string-append out "/share/guile/site/3.0"))
                     (python (search-input-file inputs "/bin/python3")))
                (format #t "~%=== Installing Additional Components ===~%")
                
                (mkdir-p share)
                (mkdir-p bin)
                (mkdir-p lib)
                (mkdir-p guile-site)
                
                ;; Navigate back to source directory from build directory
                (chdir "../source")
                (format #t "Current directory: ~a~%" (getcwd))
                
                ;; ========== Install Scheme Modules ==========
                ;; Install all Scheme modules from synergy/ directory
                (when (file-exists? "synergy")
                  (format #t "Installing Scheme modules from synergy/...~%")
                  (for-each
                    (lambda (scm-file)
                      (let* ((rel-path (if (string-prefix? "./" scm-file)
                                          (string-drop scm-file 2)
                                          scm-file))
                             (dir-path (dirname rel-path))
                             (target-dir (string-append guile-site "/" dir-path)))
                        (format #t "  Installing: ~a -> ~a~%" scm-file target-dir)
                        (mkdir-p target-dir)
                        (install-file scm-file target-dir)))
                    (find-files "synergy" "\\.scm$"))
                  
                  ;; Verify installation
                  (format #t "Installed Scheme modules:~%")
                  (for-each
                    (lambda (f) (format #t "  ~a~%" f))
                    (find-files (string-append guile-site "/synergy") "\\.scm$")))
                
                ;; Install test scripts
                (when (file-exists? "tests/synergy/interoperability-test.sh")
                  (let ((test-dir (string-append share "/tests/synergy")))
                    (format #t "Installing test scripts...~%")
                    (mkdir-p test-dir)
                    (install-file "tests/synergy/interoperability-test.sh" test-dir)
                    (chmod (string-append test-dir "/interoperability-test.sh") #o755)
                    (format #t "  Installed: tests/synergy/interoperability-test.sh~%")))
                ;; ============================================
                
                ;; Install Python demonstration application if present
                (when (file-exists? "app.py")
                  (format #t "Installing app.py...~%")
                  (install-file "app.py" share)
                  (call-with-output-file (string-append bin "/opencog-demo")
                    (lambda (port)
                      (format port "#!/bin/sh~%exec ~a ~a/app.py \"$@\"~%"
                              python share)))
                  (chmod (string-append bin "/opencog-demo") #o755))
                
                ;; Install documentation files
                (when (file-exists? "README.md")
                  (format #t "Installing README.md...~%")
                  (install-file "README.md" share))
                (when (file-exists? "requirements.txt")
                  (format #t "Installing requirements.txt...~%")
                  (install-file "requirements.txt" share))
                (when (file-exists? "LICENSE")
                  (format #t "Installing LICENSE...~%")
                  (install-file "LICENSE" share))
                
                (format #t "~%=== Additional Components Installation Complete ===~%~%")
                #t)))
          (add-after 'install 'validate-installation
            (lambda* (#:key outputs #:allow-other-keys)
              (let ((out (assoc-ref outputs "out")))
                (format #t "~%=== Validating Installation ===~%")
                
                ;; Check that key directories exist
                (for-each
                  (lambda (dir)
                    (let ((full-path (string-append out dir)))
                      (if (file-exists? full-path)
                          (format #t "  ✓ Directory exists: ~a~%" dir)
                          (format #t "  ✗ Directory missing: ~a~%" dir))))
                  '("/bin" "/lib" "/include" "/share"))
                
                ;; List installed files
                (format #t "~%Installed files in ~a:~%" out)
                (system* "find" out "-type" "f" "-ls")
                
                (format #t "~%=== Installation Validation Complete ===~%~%")
                #t))))))
    (native-inputs
     (list pkg-config
           cmake-minimal
           gcc-toolchain))
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
           gsl
           cxxtest
           sparsehash))  ;; Added for sparse data structures
    (propagated-inputs
     (list python-numpy
           python-pandas
           python-scikit-learn
           python-matplotlib))
    (home-page "https://github.com/cogpy/occ")
    (synopsis "OpenCog Collection - Cognitive Computing and AGI Framework")
    (description
     "This package provides the OpenCog Collection (OCC) monorepo, an integrated
development environment for cognitive computing and artificial general intelligence
(AGI) research and development.

The collection brings together multiple OpenCog-related projects into a coherent
whole, enabling cognitive synergy through the integration of various AI and
cognitive computing approaches.

Core OpenCog Components:
@itemize
@item CogUtil - Base utilities and configuration system for OpenCog
@item AtomSpace - Hypergraph database and knowledge representation system
@item CogServer - Networking and inter-process communication layer
@item Matrix - Sparse matrix and graph processing for machine learning
@item Learn - Symbolic learning algorithms and pattern mining
@item Agents - Interactive cognitive agents framework
@item Sensory - Dataflow system for external world interaction
@end itemize

Advanced Cognitive Architecture:
@itemize
@item CogGML - Self-aware microkernel for cognitive processing
@item CogSelf - AGI cognitive synergy framework
@item AtomSpace Accelerator - High-performance inference engine
@item Agentic Chatbots - Conversational AI integration
@end itemize

Additional Features:
@itemize
@item Python-based machine learning demonstrations using scikit-learn
@item Complete source code for research and development
@item Modular architecture for flexible deployment
@end itemize

This package is suitable for researchers, developers, and students working on
artificial general intelligence, cognitive computing, knowledge representation,
and related fields.")
    (license license:agpl3+)))

;; Return the package for building
opencog-collection
