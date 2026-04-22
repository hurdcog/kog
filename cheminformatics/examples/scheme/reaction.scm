;
; reaction.scm
;
; Simple example of using the graph rewrites to emulate chmical
; reactions. This will search the AtomSpace for anything matching
; the query, and then create a new molecule whenever it finds the
; matching pattern.
;
; To run this example, start the guile shell in the `examples` directory
; (the directory holding this file!) and then say
;   `(load "reaction.scm")`
; It will print the results.

(use-modules (opencog) (opencog cheminformatics))
(use-modules (opencog exec))

; This defines an esterification reaction.
(define esterification
	(BindLink
		; Variable declaration
		(VariableList

			; Typed variables, specifying specific atoms.
			(TypedVariable (Variable "$carboxyH1") (Type 'H))
			(TypedVariable (Variable "$carboxyO1") (Type 'O))
			(TypedVariable (Variable "$carboxyC1") (Type 'C))
			(TypedVariable (Variable "$carboxyO2") (Type 'O))

			(TypedVariable (Variable "$hydroxH1") (Type 'H))
			(TypedVariable (Variable "$hydroxO1") (Type 'O))

			; Untyped variables that will match to anything
			(Variable "carboxy moiety")
			(Variable "hydroxy moiety")
			(Glob "rest of carboxy")
			(Glob "rest of hydroxy")
		)
		; Premise: Functional groups found in some educts
		(AndLink
			; ------------------------------------------------------
			; Look for carboxyl group
			(Molecule
				(DB (Variable "$carboxyC1") (Variable "$carboxyO2"))
				(SB (Variable "$carboxyC1") (Variable "$carboxyO1"))
				(SB (Variable "$carboxyO1") (Variable "$carboxyH1"))
				(SB (Variable "$carboxyC1") (Variable "carboxy moiety"))

				; Globs match one or more bonds.  To match zero,
				; change the lower bound by declaring it like this:
				; (TypedVariable (Glob "rest of carboxy")
				;     (Interval (Number 0) (Number -1)))
				(Glob "rest of carboxy")
			)

			; The above will happily match `$carboxyO1` and `carboxy moiety`
			; to the same atom. But we don't want that, so demand that
			; they be distinct.
			(Not (Identical (Variable "$carboxyO1") (Variable "carboxy moiety")))

			; ------------------------------------------------------
			; Look for hydroxyl group
			(Molecule
				(SB (Variable "$hydroxO1") (Variable "$hydroxH1"))
				(SB (Variable "hydroxy moiety") (Variable "$hydroxO1"))
				(Glob "rest of hydroxy")
			)

			; As before, the above will happily stuff the hydrogen into
			; the moiety slot (thus using it twice). Prevent that by
			; insisting these two must differ.
			(Not (Identical (Variable "$hydroxH1") (Variable "hydroxy moiety")))

			; -----
			; And another constraint: the above pattern is so simple, it
			; will find hydroxyl groups everywhere. In particular, it will
			; find the OH in the carboxyl. So make sure that doesn't happen
			; by insisting that the OH's be fiferent from one-another.
			; Force them to be distinct.
			(Not (Identical (Variable "$carboxyO1") (Variable "$hydroxO1")))
			(Not (Identical (Variable "$carboxyH1") (Variable "$hydroxH1")))

			; A different way to say the same thing.
			; Either the two Not's above, or the one Not below, is
			; sufficient to disambiguate the OH.
			(Not (Identical
				(SB (Variable "$hydroxO1") (Variable "$hydroxH1"))
				(SB (Variable "$carboxyO1") (Variable "$carboxyH1"))))
		)
		; Clause: Formation of products
		(AndLink
			; Produce ester
			(Molecule
				(DB (Variable "$carboxyC1") (Variable "$carboxyO2"))
				(SB (Variable "$carboxyC1") (Variable "$carboxyO1"))

				(SB (Variable "$carboxyC1") (Variable "carboxy moiety"))
				(Glob "rest of carboxy")

				(SB (Variable "$carboxyO1") (Variable "hydroxy moiety"))
				(Glob "rest of hydroxy")
			)
			; Produce water
			(Molecule
				(SB (Variable "$hydroxO1") (Variable "$carboxyH1"))
				(SB (Variable "$hydroxO1") (Variable "$hydroxH1"))
			)
		)
	)
)

; ------------------------------------------------
; Populate the AtomSpace with some contents.
;
; Carboxyl group
(Molecule
	(DB (C "the carboxyl carb") (O "oxy two"))
	(SB (C "the carboxyl carb") (O "oxy one"))
	(SB (O "oxy one") (H "carboxyl proton"))
	(SB (C "the carboxyl carb") (Fe "carbox R"))
	; Some nonsense moiety, for pattern matching only.
	(SB (Fe "carbox R") (Ni "more carbox junk"))
)

; A hydroxyl group
(Molecule
	(SB (O "hydroxyl oxy") (H "hydroxyl proton"))

	; Another nonsense moiety, for pattern matching
	(SB (C "hydroxyl carbon") (O "hydroxyl oxy"))
	(SB (C "hydroxyl carbon") (Zn "hydrox R"))
	(SB (Zn "hydrox R") (Cu "junk hydrox moiety"))
)

; Perform the reaction
(define products (cog-execute! esterification))

(format #t "The reaction products are: ~A\n" products)

; ------------------------------------------------
; The end.
; That's all, folks!
