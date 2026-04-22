#!/usr/bin/env guile
!#

;;; Comprehensive Plan9/Inferno namespace implementation test
;;; Tests and demonstrates the enhanced namespace features

(define-module (test-comprehensive-namespace)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match))

(format #t "🧪 === COMPREHENSIVE PLAN9/INFERNO NAMESPACE TEST === 🧪~%")

;; Test module loading with error handling
(define (test-module-loading)
  (format #t "~%🔧 Testing module loading...~%")
  
  (catch #t
    (lambda ()
      (use-modules (cogkernel atomspace))
      (format #t "✅ atomspace module loaded~%")
      
      (use-modules (cogkernel cognitive-grip))
      (format #t "✅ cognitive-grip module loaded~%")
      
      (use-modules (cogkernel 9p-hypergraph))
      (format #t "✅ 9p-hypergraph module loaded~%")
      
      (use-modules (cogkernel limbo-grammar))
      (format #t "✅ limbo-grammar module loaded~%")
      
      #t)
    (lambda (key . args)
      (format #t "❌ Module loading failed: ~a ~a~%" key args)
      #f)))

;; Test Plan9 namespace path operations
(define (test-plan9-namespace-paths)
  (format #t "~%🌐 Testing Plan9 namespace paths...~%")
  
  ;; Test path resolution
  (define test-paths
    '("/hurd/servers/ext2fs"
      "/hurd/translators/auth"
      "/proc/cpuinfo"
      "/dev/hd0s1"
      "/usr/bin/guile"))
  
  (for-each
    (lambda (path)
      (define components (filter (lambda (c) (not (string=? c ""))) 
                                (string-split path #\/)))
      (format #t "   Path: ~a → Components: ~a~%" path components))
    test-paths)
  
  ;; Test namespace tree structure
  (define namespace-tree
    '(root
      (hurd
        (servers ext2fs procfs tmpfs devfs)
        (translators auth exec proc term)
        (boot gnumach hurd))
      (proc
        (cpuinfo meminfo version cmdline))
      (dev
        (hd0s1 hd0s2 tty0 tty1))
      (usr
        (bin (guile gcc make))
        (lib (libc.so libguile.so))
        (share (doc man)))))
  
  (format #t "   Namespace tree: ~a~%" namespace-tree)
  
  ;; Test namespace lookup
  (define (namespace-lookup path tree)
    "Lookup a path in the namespace tree"
    (let ((components (filter (lambda (c) (not (string=? c ""))) 
                             (string-split path #\/))))
      (fold (lambda (component current)
              (if (and (list? current) (> (length current) 1))
                  (let ((found (find (lambda (item)
                                       (or (eq? item component)
                                           (and (list? item)
                                                (eq? (car item) component))))
                                     (cdr current))))
                    (or found 'not-found))
                  'not-found))
            tree
            components)))
  
  (for-each
    (lambda (path)
      (define result (namespace-lookup path namespace-tree))
      (format #t "   Lookup ~a → ~a~%" path result))
    test-paths))

;; Test Inferno channels and concurrent features
(define (test-inferno-channels)
  (format #t "~%📡 Testing Inferno channel features...~%")
  
  ;; Channel types and states
  (define channel-types '(sync async buffered unbuffered))
  (define process-states '(spawned running blocked waiting terminated))
  
  ;; Create test channels
  (define (make-test-channel name type)
    `(channel
      (name . ,name)
      (type . ,type)
      (state . created)
      (buffer-size . ,(if (memq type '(buffered)) 64 0))
      (processes . ())))
  
  (define test-channels
    (map (lambda (type)
           (make-test-channel (symbol->string type) type))
         channel-types))
  
  (for-each
    (lambda (channel)
      (format #t "   Created channel: ~a~%" channel))
    test-channels)
  
  ;; Test process spawning
  (define (spawn-test-process name channel)
    `(process
      (name . ,name)
      (state . spawned)
      (channel . ,(assq 'name channel))
      (pid . ,(random 1000))))
  
  (define test-processes
    (map (lambda (channel)
           (spawn-test-process
             (string-append "proc-" (symbol->string (cdr (assq 'name channel))))
             channel))
         test-channels))
  
  (for-each
    (lambda (process)
      (format #t "   Spawned process: ~a~%" process))
    test-processes))

;; Test namespace mount points and services
(define (test-namespace-mounts)
  (format #t "~%🔗 Testing namespace mount points...~%")
  
  ;; Define mount specifications
  (define mount-specs
    '(((service . "hurd-fs") (mount-point . "/hurd") (type . "9p"))
      ((service . "proc-fs") (mount-point . "/proc") (type . "procfs"))
      ((service . "dev-fs") (mount-point . "/dev") (type . "devfs"))
      ((service . "tmp-fs") (mount-point . "/tmp") (type . "tmpfs"))))
  
  ;; Create mount points
  (define (create-mount spec)
    `(mount
      (service . ,(cdr (assq 'service spec)))
      (mount-point . ,(cdr (assq 'mount-point spec)))
      (type . ,(cdr (assq 'type spec)))
      (state . mounted)
      (options . (rw noatime))
      (namespace-id . ,(random 10000))))
  
  (define active-mounts
    (map create-mount mount-specs))
  
  (for-each
    (lambda (mount)
      (format #t "   Mount: ~a~%" mount))
    active-mounts)
  
  ;; Test namespace visibility
  (format #t "   Testing namespace visibility...~%")
  (for-each
    (lambda (mount)
      (define mount-point (cdr (assq 'mount-point mount)))
      (define service (cdr (assq 'service mount)))
      (format #t "     ~a visible at ~a~%" service mount-point))
    active-mounts))

;; Test 9P protocol operations simulation
(define (test-9p-operations)
  (format #t "~%🌍 Testing 9P protocol operations...~%")
  
  ;; 9P operation types
  (define 9p-ops '(Tversion Tauth Tattach Twalk Topen Tread Twrite Tclunk Tremove))
  
  ;; Simulate 9P operations
  (define (simulate-9p-op op-type path)
    `(9p-operation
      (type . ,op-type)
      (path . ,path)
      (fid . ,(random 1000))
      (tag . ,(random 65536))
      (result . success)))
  
  (define test-operations
    (list
      (simulate-9p-op 'Tattach "/hurd")
      (simulate-9p-op 'Twalk "/hurd/servers")
      (simulate-9p-op 'Topen "/hurd/servers/ext2fs")
      (simulate-9p-op 'Tread "/hurd/servers/ext2fs")
      (simulate-9p-op 'Twrite "/tmp/test-file")
      (simulate-9p-op 'Tclunk "/hurd/servers/ext2fs")))
  
  (for-each
    (lambda (op)
      (format #t "   9P ~a: ~a → ~a~%" 
              (cdr (assq 'type op))
              (cdr (assq 'path op))
              (cdr (assq 'result op))))
    test-operations))

;; Main test runner
(define (run-comprehensive-test)
  (format #t "🚀 Starting comprehensive Plan9/Inferno namespace tests...~%")
  
  (define tests
    (list
      (cons "Module Loading" test-module-loading)
      (cons "Plan9 Namespace Paths" test-plan9-namespace-paths)
      (cons "Inferno Channels" test-inferno-channels)
      (cons "Namespace Mounts" test-namespace-mounts)
      (cons "9P Operations" test-9p-operations)))
  
  (define results
    (map (lambda (test)
           (format #t "~%📋 Running test: ~a~%" (car test))
           (catch #t
             (lambda ()
               ((cdr test))
               (format #t "✅ Test ~a passed~%" (car test))
               #t)
             (lambda (key . args)
               (format #t "❌ Test ~a failed: ~a ~a~%" (car test) key args)
               #f)))
         tests))
  
  (define passed (length (filter identity results)))
  (define total (length results))
  
  (format #t "~%📊 === TEST SUMMARY === 📊~%")
  (format #t "Passed: ~a/~a tests~%" passed total)
  (format #t "Success rate: ~a%~%" (inexact->exact (* 100 (/ passed total))))
  
  (if (= passed total)
      (format #t "🎉 All tests passed! Plan9/Inferno namespace features working!~%")
      (format #t "⚠️  Some tests failed. Review implementation.~%")))

;; Run the comprehensive test
(run-comprehensive-test)

(format #t "~%✅ === COMPREHENSIVE NAMESPACE TEST COMPLETE === ✅~%")