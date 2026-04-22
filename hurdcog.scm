;;; Guix package definition for HurdCog (OpenCog-Powered GNU Hurd)
;;; This package builds the HurdCog cognitive operating system

(define-module (hurdcog)
  #:use-module (guix packages)
  #:use-module (guix git-download)
  #:use-module (guix gexp)
  #:use-module (guix build-system gnu)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages)
  #:use-module (gnu packages autotools)
  #:use-module (gnu packages base)
  #:use-module (gnu packages gcc)
  #:use-module (gnu packages hurd)
  #:use-module (gnu packages guile)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages texinfo)
  #:use-module (gnu packages perl)
  #:use-module (gnu packages python)
  #:use-module (gnu packages python-xyz)
  #:use-module (gnu packages boost)
  #:use-module (gnu packages cpp)
  #:use-module (cognumach))  ; Import our Cognumach package

(define-public hurdcog
  (package
    (name "hurdcog")
    (version "2.1")
    (source (local-file (string-append (dirname (dirname (current-filename))) "/hurdcog")
                        "hurdcog-checkout"
                        #:recursive? #t
                        #:select? (git-predicate (string-append (dirname (dirname (current-filename))) "/hurdcog"))))
    (build-system gnu-build-system)
    (arguments
     (list #:configure-flags
           #~(list "--host=i686-gnu"
                   "CC=gcc -m32"
                   (string-append "--with-mach=" #$(this-package-input "cognumach"))
                   (string-append "--prefix=" #$output))
           #:phases
           #~(modify-phases %standard-phases
               (add-before 'configure 'setup-environment
                 (lambda* (#:key inputs #:allow-other-keys)
                   ;; Set up build environment
                   (setenv "CC" "gcc -m32")
                   (setenv "GUILE_LOAD_PATH"
                           (string-append (assoc-ref inputs "guile") "/share/guile/site/3.0:"
                                        (or (getenv "GUILE_LOAD_PATH") "")))
                   #t))
               (add-before 'configure 'build-cogkernel
                 (lambda* (#:key inputs outputs #:allow-other-keys)
                   ;; Build the cognitive kernel components first
                   (let ((cogkernel-dir "cogkernel"))
                     (when (file-exists? cogkernel-dir)
                       (with-directory-excursion cogkernel-dir
                         ;; Compile Scheme modules
                         (for-each
                           (lambda (scm-file)
                             (format #t "Compiling ~a~%" scm-file)
                             (invoke "guild" "compile" "-o"
                                   (string-append (basename scm-file ".scm") ".go")
                                   scm-file))
                           (find-files "." "\\.scm$"))
                         ;; Build C components if Makefile exists
                         (when (file-exists? "Makefile")
                           (invoke "make" "-j" (number->string (parallel-job-count)))))))
                   #t))
               (add-before 'configure 'run-autoreconf
                 (lambda _
                   ;; Generate configure script if needed
                   (when (not (file-exists? "configure"))
                     (when (file-exists? "configure.ac")
                       (invoke "autoreconf" "-vif")))
                   #t))
               (replace 'configure
                 (lambda* (#:key configure-flags #:allow-other-keys)
                   ;; HurdCog may not have a traditional configure script
                   ;; Check if configure exists before running it
                   (if (file-exists? "configure")
                       (apply invoke "./configure" configure-flags)
                       (begin
                         (format #t "No configure script found, proceeding with manual setup~%")
                         #t))))
               (replace 'build
                 (lambda* (#:key #:allow-other-keys)
                   ;; Build Hurd servers and translators
                   (if (file-exists? "Makefile")
                       (invoke "make" "-j" (number->string (parallel-job-count)))
                       (begin
                         (format #t "No Makefile found, skipping build~%")
                         #t))))
               (replace 'install
                 (lambda* (#:key outputs #:allow-other-keys)
                   (let* ((out (assoc-ref outputs "out"))
                          (bin (string-append out "/bin"))
                          (lib (string-append out "/lib"))
                          (share (string-append out "/share/hurdcog"))
                          (cogkernel (string-append share "/cogkernel")))
                     ;; Create directories
                     (mkdir-p bin)
                     (mkdir-p lib)
                     (mkdir-p share)
                     (mkdir-p cogkernel)
                     
                     ;; Install traditional Hurd components if they exist
                     (when (file-exists? "Makefile")
                       (invoke "make" "install"))
                     
                     ;; Install cognitive kernel
                     (when (file-exists? "cogkernel")
                       (copy-recursively "cogkernel" cogkernel))
                     
                     ;; Install cognitive components
                     (for-each
                       (lambda (dir)
                         (when (file-exists? dir)
                           (copy-recursively dir (string-append share "/" dir))))
                       '("cognitive" "distributed" "performance" "development"))
                     
                     ;; Install documentation
                     (for-each
                       (lambda (doc)
                         (when (file-exists? doc)
                           (install-file doc share)))
                       '("README.md" "FUSION_REACTOR_QUICK_START.md"))
                     
                     ;; Create startup scripts
                     (call-with-output-file (string-append bin "/hurdcog-boot")
                       (lambda (port)
                         (format port "#!/bin/sh~%")
                         (format port "# HurdCog Boot Script~%")
                         (format port "exec ~a/cogkernel/hurdcog-bootstrap.scm~%" share)))
                     (chmod (string-append bin "/hurdcog-boot") #o755)
                     
                     (call-with-output-file (string-append bin "/hurdcog-dashboard")
                       (lambda (port)
                         (format port "#!/bin/sh~%")
                         (format port "# HurdCog Master Control Dashboard~%")
                         (format port "cd ~a/cogkernel~%" share)
                         (format port "exec python3 fusion-reactor-server.py~%")))
                     (chmod (string-append bin "/hurdcog-dashboard") #o755)
                     
                     #t))))))
    (native-inputs
     (list autoconf
           automake
           libtool
           pkg-config
           texinfo
           perl))
    (inputs
     (list cognumach      ; Our enhanced microkernel
           guile-3.0      ; For Scheme scripting
           python        ; For dashboard and tools
           boost         ; For C++ components
           mig))         ; Mach Interface Generator
    (propagated-inputs
     (list guile-3.0))
    (home-page "https://github.com/rzonedevops/hurdcog")
    (synopsis "OpenCog-powered GNU Hurd cognitive AGI operating system")
    (description
     "HurdCog is the world's first cognitive AGI operating system, integrating
OpenCog's AGI framework with GNU Hurd's microkernel architecture.  Unlike
traditional operating systems that execute fixed algorithms, HurdCog learns,
reasons, and adapts.

Key features include:

@itemize
@item Cognitive Fusion Reactor for distributed cognitive processing
@item Master Control Dashboard for real-time monitoring and management
@item AtomSpace-based hypergraph knowledge representation
@item PLN (Probabilistic Logic Networks) for reasoning under uncertainty
@item ECAN (Economic Attention Network) for resource allocation
@item Pattern mining for learning from system operation
@item Self-diagnosis and self-healing capabilities
@item Integration with Plan9, Inferno, and other distributed systems
@item Kokkos-based performance optimization
@end itemize

HurdCog runs on top of the Cognumach microkernel and provides a complete
cognitive operating system environment for AGI research and development.")
    (license license:gpl2+)))

;; Return the package
hurdcog
