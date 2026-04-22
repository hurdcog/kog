;; GNU Guix development manifest.  To create development environment, run
;;
;;     guix shell
;;
;; or something like
;;
;;     guix shell --pure --manifest=manifest.scm ...

(specifications->manifest
 (list "ikiwiki"
       "perl"
       "perl-search-xapian"
       "perl-text-markdown"
       "perl-yaml-syck"
       "texinfo"))
