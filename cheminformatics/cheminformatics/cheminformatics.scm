;
; Opencog cheminformatics atom-types module
;
(define-module (opencog cheminformatics))

(use-modules (opencog))
(use-modules (opencog cheminfo-config))

; Load the C library that calls the classserver to load the types.
(load-extension
	(string-append opencog-ext-path-cheminfo "libcheminformatics-types")
	"cheminformatics_types_init")

(load-from-path "opencog/cheminformatics/types/cheminformatics_types.scm")
