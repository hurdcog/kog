;; Shepherd service configuration for OpenCog development
(use-modules (shepherd service))

;; OpenCog build service
(service
 '(opencog-build)
 #:start (make-forkexec-constructor '("guix" "build" "./packaging/opencog.scm"))
 #:stop (make-kill-destructor)
 #:description "Build OpenCog package with Guix")

;; Development server service (when available)
(service
 '(opencog-server)
 #:start (make-forkexec-constructor '("guix" "environment" "--" "cogserver"))
 #:stop (make-kill-destructor)
 #:requires '(opencog-build)
 #:description "Run OpenCog CogServer for development")

;; Package installation service
(service
 '(opencog-install)
 #:start (make-forkexec-constructor '("guix" "install" "-f" "./packaging/opencog.scm"))
 #:stop (make-kill-destructor)
 #:requires '(opencog-build)
 #:description "Install OpenCog package locally")