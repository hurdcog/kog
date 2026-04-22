;
; crawler-interactive.scm -- Interactive crawler demo
;
; This is a trimmed-down version of the demo in `crawler.scm`, with
; most of the commentary and explanation removed. It's much shorter,
; which perhaps makes it easier to understand.
;
; The easiest way of running this demo is to say
;    guile -l crawler-interactive.scm
; This will drop you into the guile REPL with this file already loaded.
; Then go to the bottom of this file, and start the looper, as shown.
;
(use-modules (opencog) (opencog exec) (opencog sensory))

; Define the basic recursive directory explorer.
(define explore-dirs
	(Rule
		(Variable "$string-url")

		; Accept only directories
		(LinkSignature (Type 'LinkValue)
			(Variable "$string-url")
			(StringOf (Type 'StringValue) (Node "dir")))

		; Rewrite the directory into a SensoryNode,
		; then open it. The get the special-file tags on it.
		(Write
			(Open (Type 'FileSysStream)
				(StringOfLink (Type 'SensoryNode)
					(Variable "$string-url")))
			(Item "special"))))

; Create the anchor point on which the directory looper will
; hang it's data.
(define looper-loc (ValueOf (Anchor "crawler") (Predicate "looper")))

; Define the looper itself. Calling cog-execute! on this will step
; one level deeper into the filesystem hierarchy.
(define looper
	(SetValue
		(Anchor "crawler") (Predicate "looper")
		(Concatenate
			(Filter explore-dirs looper-loc))))

; Prime looper anchor point with initial URL data.
(define (set-initial-root fsys-root)
	(SetValue (Anchor "crawler") (Predicate "looper")
		(Write
			(Open (Type 'FileSysStream) fsys-root)
			(List (Item "special") fsys-root))))

; ... and actually set it.
(cog-execute! (set-initial-root (Sensory "file:///etc")))

; A predicate, which, when cog-evaluate! -ed tells us if there's
; more directory data to be explored.
(Define (DefinedPredicate "keep going?")
	(GreaterThan (SizeOf looper-loc) (Number 0)))

;----------------------------------------------------
; Create an xterm for keyboard I/O.
(define term-loc (ValueOf (Anchor "crawler") (Predicate "term")))
(cog-execute!
	(SetValue (Anchor "crawler") (Predicate "term")
	(Open (Type 'TerminalStream))))

; Define a rule that outputs files to the xterm stream.
(define report-files
	(Rule
		(Variable "$string-url")

		; Accept only regular files
		(LinkSignature (Type 'LinkValue)
			(Variable "$string-url")
			(StringOf (Type 'StringValue) (Node "reg")))

		; Write them to the terminal
		(Write term-loc (LinkSignature (Type 'LinkValue)
			(Node "Found a file: ")
			(Variable "$string-url") (Node "\n")))))

;----------------------------------------------------
; Specify a location where terminal input will be held.
(define input-loc
	(ValueOf (Anchor "crawler") (Predicate "in-words")))

; Define a large, complex interactive loop.
(Define
	(DefinedPredicate "Interactive shell")
	(SequentialAnd
		(True (Filter report-files looper-loc))
		(True (Write term-loc (Node "------------------------\n")))
		(True (Write term-loc (Node
			"Usage:\n\tHit eenter to continue\n\tcd file:///some/dir to change directory\n\tAnything else to exit loop\n")))

		; Tokenize the input
		(True (SetValue (Anchor "crawler") (Predicate "in-words")
				(Split term-loc)))
		(Cond
			(StreamEqual input-loc (Link))
			(True (Write term-loc (Node "You hit enter\n")))
			(StreamEqual input-loc (Link (Item "cd") (Type 'StringValue)))
			(True
				(Write term-loc (Node "Going to change dir to "))
				(Write term-loc (ElementOf (Number 1) input-loc))
				(Write term-loc (Node "\n"))
				(SetValue (Anchor "crawler") (Predicate "looper")
					(Write
						(Open (Type 'FileSysStream)
							(ElementOf (Number 1) input-loc))
						(List (Item "special")
							(ElementOf (Number 1) input-loc))))
			)
			(False (Write term-loc (Node "Goodbye!\n"))))
		(True looper)
		(DefinedPredicate "Interactive shell")
	))

; Set the initial location for the looper.
(cog-execute! (set-initial-root (Sensory "file:///etc")))

; Run the interactive shell.
; (cog-evaluate! (DefinedPredicate "Interactive shell"))

;;;------------------------------------------------------------------
