scm
;
; seme-process.scm
;
; Perform seme processing
;
; Copyright (C) 2009 Linas Vepstas <linasvepstas@gmail.com>
;
; --------------------------------------------------------------------
; 

; do-seme-processing -- ad-hoc routine under development.
;
; Process parsed text through the prepositional-triples code.
;
; This will run the preposition-triple rules through the forward
; chainer, and then go through the results, updating the 
; CountTruthValue associated with each, and then storing the 
; updated count in the OpenCog persistence backend.
;

(define (do-seme-processing)

	; Get the new input sentences, and run them through the triples processing code.
	; But do it one at a time.
	(define (do-one-sentence sent)
		(attach-sents-for-triple-processing (list sent))
		(system "echo start work an a triple")
		(system "date")
		(create-triples)
		(dettach-sents-from-triple-anchor)
		(for-each 
			(lambda (x) 
				(system (string-join (list "echo done triple: \"" (object->string x) "\"")))
			)
	      (get-new-triples)
  		)
	   (delete-result-triple-links)
	)

	(for-each do-one-sentence (get-new-parsed-sentences))

	;  (get-new-triples)
	;    (fetch-related-triples)
	;(delete-sentences)

)


; XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;
;
; process-rule -- apply an ImplicationLink
;
; Given an ImplicationLink, apply the implication on the atom space.
; This may generate a list of atoms. Take that list, and manually
; store it in the database.
;
(define (xxxprocess-rule rule)
	(define triple-list (cog-outgoing-set (cog-ad-hoc "do-implication" rule)))

	; Increment count by 1 on each result.
	(for-each (lambda (atom) (cog-atom-incr atom 1)) triple-list)
	; (system "date")
	; (system "echo Done running one implication\n")

	; Store each resultant atom.
	(for-each (lambda (atom) (cog-ad-hoc "store-atom" atom)) triple-list)
)

.
exit
