;
; rewrite.scm -- Example of stream rewriting
;
; The FileSysStream sensory node generates a stream of StringValues
; that correspond to files in a file system. This stream can be
; rewritten to generate altered forms of the streaming data. One of the
; most basic tasks is to capture that stream into Atoms, so that it can
; be recorded in the AtomSpace.
;
; This example shows how to convert a directory listing, originally
; appearing as a stream, to a set of tagged ItemNodes. As such, the
; resulting contents of the AtomSpce can be thought of as a "model" of
; the "outside world" (the file system), and that model can be examined
; ("pondered") without requiring a second look at the external world.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec) (opencog sensory))

; Create and open a file system stream. Aim it at the `/tmp` directory,
; which will usually have something in it on most Linux systems.
; Change as desired.
(cog-execute!
   (SetValue
      (Anchor "xplor") (Predicate "fsys")
      (Open (Type 'FileSysStream) (Sensory "file:///tmp"))))

; The stream is anchored to a "well known location". Create a handle
; for that location, so we can minimize typing for this demo. (Real apps
; would just access the appropriate location directly.)
(define fs-handle (ValueOf (Anchor "xplor") (Predicate "fsys")))

; The FileSysStream supports several basic Unix-style "commands".
; Properly speaking, these are sensory data sources; but its OK to
; thiink of them as being "commands". The `ls` command is just a
; directory listing.
(cog-execute! (Write fs-handle (Item "ls")))

; Rewrites are performed by a Filter; the filter applies a rule to the
; input stream. Below is a rewrite rule that doesn't do anything: it
; just takes the input, and passes it through. It's a no-op.
; Careful reading shows its not quite a no-op: it will filter out any
; data that is not a StringValue. But since all of the input is
; StringValues anyway ... it becomes a no-op.
(define no-op-rewrite
	(Filter
		(Rule
			; Variable declaration for the rule. One of more variables
			; can be specified.
			(Variable "$string-url")

			; Input selector. The incoming data must match this pattern,
			; in order for the rule to be applied. In this case, the
			; pattern just indicates that the incoming data must be of
			; the type `StringValue`.
			(LinkSignature (Type 'StringValue)
				(Variable "$string-url"))

			; The term rewrite. The matched pattern will be rewritten
			; according to this pattern.
			(Variable "$string-url"))

	; The source of the stream of data, to which the filter above
	; will be applied. As before, this is just a directory listing.
	(Write fs-handle (Item "ls"))))

; Try the above, and see:
(cog-execute! no-op-rewrite)

; Similar to above, but use the rewrite rule to convert the StringValue
; into an ItemNode, and then wraps the ItemNode with an EdgeLink, so
; that a later search of the AtomSpace will find the Atoms.
(define copy-in
	(Filter
		(Rule
			(Variable "$string-url")
			(LinkSignature (Type 'StringValue)
				(Variable "$string-url"))

			; The rewrite.
			(Edge
				(Predicate "URL")
				(StringOfLink (Type 'ItemNode)
					(Variable "$string-url"))))
	 (Write fs-handle (Item "ls"))))

; Try it.
(cog-execute! copy-in)

; The directory entries are now searchable as conventional atoms.
(define query
	(Meet
		(TypedVariable (Variable "$someplace") (Type 'ItemNode))
		(Edge (Predicate "URL") (Variable "$someplace"))))

(cog-execute! query)
