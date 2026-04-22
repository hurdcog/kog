;; Shepherd service definitions for OpenCog Collection external integrations
;; These services manage the daemon processes for cognitive reasoning

(use-modules (gnu)
             (gnu services)
             (gnu services shepherd)
             (gnu system shadow))

;; Gnucash cognitive accounting daemon
(define gnucash-opencog-service
  (simple-service 'gnucash-opencog
                  shepherd-root-service-type
                  (list (shepherd-service
                         (provision '(gnucash-opencog))
                         (documentation "Gnucash OpenCog cognitive accounting daemon")
                         (start #~(make-forkexec-constructor
                                   (list #$(file-append gnucash-opencog "/bin/gnucash-daemon")
                                         "--opencog-mode"
                                         "--port" "8080"
                                         "--atomspace-endpoint" "localhost:17001")
                                   #:log-file "/var/log/gnucash-opencog.log"
                                   #:environment-variables
                                   '("GNUCASH_OPENCOG_CONFIG=/etc/opencog/gnucash.conf")))
                         (stop #~(make-kill-destructor))
                         (respawn? #t)))))

;; KoboldCpp story/world modeling service  
(define koboldcpp-opencog-service
  (simple-service 'koboldcpp-opencog
                  shepherd-root-service-type
                  (list (shepherd-service
                         (provision '(koboldcpp-opencog))
                         (documentation "KoboldCpp OpenCog story generation service")
                         (start #~(make-forkexec-constructor
                                   (list #$(file-append koboldcpp-opencog "/bin/koboldcpp-opencog")
                                         "--port" "5001"
                                         "--model" "/var/lib/opencog/models/story-model.gguf"
                                         "--threads" "4"
                                         "--ctx-size" "4096")
                                   #:log-file "/var/log/koboldcpp-opencog.log"
                                   #:environment-variables
                                   '("OPENCOG_KOBOLD_PORT=5001"
                                     "OPENCOG_KOBOLD_MODEL_PATH=/var/lib/opencog/models")))
                         (stop #~(make-kill-destructor))
                         (respawn? #t)))))

;; Aphrodite Engine LLM inference server
(define aphrodite-opencog-service  
  (simple-service 'aphrodite-opencog
                  shepherd-root-service-type
                  (list (shepherd-service
                         (provision '(aphrodite-opencog))
                         (documentation "Aphrodite Engine OpenCog LLM inference server")
                         (start #~(make-forkexec-constructor
                                   (list #$(file-append aphrodite-engine-opencog "/bin/aphrodite-opencog")
                                         "/var/lib/opencog/models/reasoning-model"
                                         "--host" "0.0.0.0"
                                         "--port" "2242"
                                         "--gpu-memory-utilization" "0.8"
                                         "--max-model-len" "4096")
                                   #:log-file "/var/log/aphrodite-opencog.log"
                                   #:environment-variables
                                   '("OPENCOG_APHRODITE_HOST=0.0.0.0"
                                     "OPENCOG_APHRODITE_PORT=2242"
                                     "CUDA_VISIBLE_DEVICES=0")))
                         (stop #~(make-kill-destructor))
                         (respawn? #t)))))

;; Service group for managing all OpenCog external integrations
(define opencog-external-services
  (simple-service 'opencog-external
                  shepherd-root-service-type
                  (list (shepherd-service
                         (provision '(opencog-external))
                         (documentation "OpenCog Collection external integration services")
                         (requirement '(gnucash-opencog koboldcpp-opencog aphrodite-opencog))
                         (start #~(const #t))
                         (stop #~(const #f))))))

;; Export services for use in system configuration
(list gnucash-opencog-service
      koboldcpp-opencog-service  
      aphrodite-opencog-service
      opencog-external-services)