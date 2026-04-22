;;; Guix package definition for Cognumach (Enhanced GNU Mach Microkernel)
;;; This package builds the Cognumach microkernel with advanced features

(define-module (cognumach)
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
  #:use-module (gnu packages texinfo)
  #:use-module (gnu packages perl)
  #:use-module (gnu packages bison)
  #:use-module (gnu packages flex))

(define-public cognumach
  (package
    (name "cognumach")
    (version "1.8+git")
    (source (local-file (string-append (dirname (dirname (current-filename))) "/cognumach")
                        "cognumach-checkout"
                        #:recursive? #t
                        #:select? (git-predicate (string-append (dirname (dirname (current-filename))) "/cognumach"))))
    (build-system gnu-build-system)
    (arguments
     (list #:configure-flags
           #~(list "--host=i686-gnu"
                   "CC=gcc -m32"
                   "LD=ld -melf_i386"
                   (string-append "MIG=" #$(this-package-native-input "mig") "/bin/mig")
                   "--enable-kdb"  ; Enable kernel debugger for development
                   "--enable-kmsg"
                   (string-append "--prefix=" #$output))
           #:phases
           #~(modify-phases %standard-phases
               (add-before 'configure 'setup-environment
                 (lambda* (#:key inputs #:allow-other-keys)
                   ;; Set up cross-compilation environment
                   (setenv "CC" "gcc -m32")
                   (setenv "LD" "ld -melf_i386")
                   (setenv "AR" "ar")
                   (setenv "RANLIB" "ranlib")
                   #t))
               (add-before 'configure 'run-autoreconf
                 (lambda _
                   ;; Generate configure script if needed
                   (when (not (file-exists? "configure"))
                     (invoke "autoreconf" "-vif"))
                   #t))
               (add-after 'install 'install-headers
                 (lambda* (#:key outputs #:allow-other-keys)
                   (let* ((out (assoc-ref outputs "out"))
                          (include (string-append out "/include")))
                     ;; Install Mach headers for Hurd to use
                     (mkdir-p include)
                     (for-each
                       (lambda (dir)
                         (when (file-exists? dir)
                           (copy-recursively dir
                                           (string-append include "/" (basename dir)))))
                       '("include" "kern" "device" "ipc" "vm"))
                     #t))))))
    (native-inputs
     (list autoconf
           automake
           libtool
           texinfo
           perl
           bison
           flex
           mig
           gcc-11))  ; Use GCC 11 for better 32-bit support
    (home-page "https://github.com/rzonedevops/cognumach")
    (synopsis "Enhanced GNU Mach microkernel with cognitive features")
    (description
     "Cognumach is an enhanced version of the GNU Mach microkernel, the foundation
of the GNU Hurd operating system.  It provides advanced features including:

@itemize
@item Advanced memory management with improved VM subsystem
@item SMP (Symmetric Multi-Processing) enhancements for better parallelism
@item VM optimizations for faster and more reliable virtual memory
@item PCI modernization for broader hardware support
@item Valgrind integration for debugging and analysis
@item Performance improvements and bug fixes
@end itemize

Cognumach serves as the microkernel foundation for HurdCog, the cognitive
AGI operating system, providing the low-level primitives needed for efficient
IPC and resource management in a cognitive architecture.")
    (license license:gpl2+)))

;; Return the package
cognumach
