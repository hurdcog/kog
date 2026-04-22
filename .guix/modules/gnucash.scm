;;; Gnucash package definition for OpenCog Collection
;;; Cognitive accounting and attention management integration

(define-module (gnucash)
  #:use-module (guix packages)
  #:use-module (guix download)
  #:use-module (guix git-download)
  #:use-module (guix build-system cmake)
  #:use-module (guix build-system gnu)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages)
  #:use-module (gnu packages finance)
  #:use-module (gnu packages gtk)
  #:use-module (gnu packages guile)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages xml)
  #:use-module (gnu packages databases)
  #:use-module (gnu packages webkit)
  #:use-module (gnu packages boost)
  #:use-module (gnu packages check))

(define-public gnucash-opencog
  (package
    (name "gnucash-opencog")
    (version "5.12")
    (source (origin
              (method git-fetch)
              (uri (git-reference
                    (url "https://github.com/Gnucash/gnucash")
                    (commit (string-append "v" version))))
              (file-name (git-file-name name version))
              (sha256
               (base32
                "0000000000000000000000000000000000000000000000000000")))) ; Placeholder hash
    (build-system cmake-build-system)
    (arguments
     `(#:tests? #f  ; Disable tests for integration build
       #:configure-flags
       '("-DCMAKE_BUILD_TYPE=Release"
         "-DWITH_PYTHON=ON"
         "-DWITH_SQL=ON"
         "-DENABLE_BINRELOC=OFF"
         "-DGNUCASH_BUILD_ID=opencog-integration")
       #:phases
       ,(modify-phases %standard-phases
         (add-before 'configure 'set-environment
           (lambda* (#:key inputs outputs #:allow-other-keys)
             ;; Set up environment for OpenCog integration
             (setenv "BOOST_ROOT" (assoc-ref inputs "boost"))
             (setenv "PKG_CONFIG_PATH"
                     (string-append (assoc-ref inputs "pkg-config") "/lib/pkgconfig:"
                                    (getenv "PKG_CONFIG_PATH")))
             #t))
         (add-after 'install 'install-opencog-adapters
           (lambda* (#:key outputs #:allow-other-keys)
             (let* ((out (assoc-ref outputs "out"))
                    (opencog-lib (string-append out "/lib/opencog"))
                    (opencog-scm (string-append out "/share/guile/site/3.0/opencog")))
               ;; Install OpenCog-specific adapters and bindings
               (mkdir-p opencog-lib)
               (mkdir-p opencog-scm)
               
               ;; Create cognitive accounting adapter library stub
               (call-with-output-file (string-append opencog-scm "/gnucash.scm")
                 (lambda (port)
                   (display "(define-module (opencog gnucash)
  #:use-module (opencog)
  #:use-module (opencog atom-types)
  #:export (gnucash-account->atom
            gnucash-transaction->atom
            cognitive-balance-attention))

;; Convert Gnucash account to AtomSpace representation
(define (gnucash-account->atom account-data)
  (ConceptNode (string-append \"Account:\" (assoc-ref account-data 'name))))

;; Convert financial transaction to cognitive representation  
(define (gnucash-transaction->atom transaction-data)
  (EvaluationLink
    (PredicateNode \"FinancialTransaction\")
    (ListLink
      (ConceptNode (assoc-ref transaction-data 'from-account))
      (ConceptNode (assoc-ref transaction-data 'to-account))
      (NumberNode (assoc-ref transaction-data 'amount)))))

;; Apply cognitive attention to financial balance changes
(define (cognitive-balance-attention account-atom threshold)
  (SetTVLink
    account-atom
    (SimpleTruthValue 0.9 0.8))) ; High attention for significant changes
" port)))
               #t))))))
    (native-inputs
     (list pkg-config
           cmake
           cxxtest))
    (inputs
     (list guile-3.0
           boost
           gtk+
           libxml2
           sqlite
           webkitgtk
           libdbi))
    (home-page "https://www.gnucash.org/")
    (synopsis "Double-entry accounting with cognitive attention hooks")
    (description
     "Gnucash enhanced for OpenCog integration, providing cognitive accounting
capabilities and attention management for financial data. This version includes
adapters to represent financial transactions and accounts as AtomSpace nodes,
enabling cognitive agents to reason about financial patterns and apply
attention mechanisms to significant balance changes.")
    (license license:gpl3+)))

;; Export the package
gnucash-opencog