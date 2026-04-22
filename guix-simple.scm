;;; Simplified Guix package file for OpenCog Collection
;;; Use with: guix build -f guix-simple.scm
;;; This is a minimal version for CI/testing purposes

(use-modules (guix packages)
             (guix download)
             (guix git-download)
             (guix gexp)
             (guix build-system python)
             (guix build-system trivial)
             ((guix licenses) #:prefix license:)
             (gnu packages)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages python-science))

(define-public opencog-collection-simple
  (package
    (name "opencog-collection-simple")
    (version "0.1.0")
    (source (local-file "." "opencog-collection-checkout"
                        #:recursive? #t))
    (build-system python-build-system)
    (arguments
      `(#:tests? #f  ; Disable tests for CI
        #:phases
        ,(modify-phases %standard-phases
          (delete 'build)  ; Skip build phase for now
          (replace 'install
            (lambda* (#:key outputs #:allow-other-keys)
              (let* ((out (assoc-ref outputs "out"))
                     (bin (string-append out "/bin"))
                     (share (string-append out "/share/opencog-collection")))
                (mkdir-p share)
                (mkdir-p bin)
                
                ;; Install Python demo
                (when (file-exists? "app.py")
                  (install-file "app.py" share)
                  (call-with-output-file (string-append bin "/opencog-demo")
                    (lambda (port)
                      (format port "#!/bin/sh~%exec python3 ~a/app.py \"$@\"~%"
                              share)))
                  (chmod (string-append bin "/opencog-demo") #o755))
                
                ;; Install requirements file
                (when (file-exists? "requirements.txt")
                  (install-file "requirements.txt" share))
                
                ;; Create a simple info file
                (call-with-output-file (string-append share "/README")
                  (lambda (port)
                    (format port "OpenCog Collection - Simple Build~%")
                    (format port "This is a minimal installation for testing.~%")
                    (format port "Run: opencog-demo~%")))
                #t))))))
    (inputs
     (list python
           python-numpy
           python-pandas
           python-scikit-learn))
    (propagated-inputs
     (list python-numpy
           python-pandas
           python-scikit-learn))
    (home-page "https://github.com/cogpy/occ")
    (synopsis "OpenCog Collection - Simplified Build")
    (description
     "This is a simplified version of the OpenCog Collection package
for testing and CI purposes. It includes only the Python machine learning
demonstration components without the full OpenCog C++ stack.")
    (license license:gpl3+)))

;; Return the package for building
opencog-collection-simple
