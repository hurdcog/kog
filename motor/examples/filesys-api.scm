;
; filesys-api.scm -- File metadata demo
;
; The FileSysStream sensory node generates a stream of StringValues
; that correspond to files in a file system. It can generate listings
; of filenames, filetypes and other file metadata. This demo provides
; a short low-level review of the available metadata. It's "low level"
; in that it does not deal with wiring issues; it just illustrates
; the "connectors" or "legs" on the component.
;
; The `filesys-api.py` file shows roughly the same demo, but in python.
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

; Current working directory (current location)
(cog-execute! (Write fs-handle (Item "pwd")))

; Change directory. Argument should be one of the strings returned
; by the `ls` command, above.
(cog-execute! (Write fs-handle (Link (Item "cd") (Item "file:///tmp/.X11-unix"))))

; Indicate if the entry is a regular or special file. Special files
; include directories, fifos, block and char devices, sockets an so on.
(cog-execute! (Write fs-handle (Item "special")))

; File birth time.
(cog-execute! (Write fs-handle (Item "btime")))

; File modification time.
(cog-execute! (Write fs-handle (Item "mtime")))

; File size, in bytes.
(cog-execute! (Write fs-handle (Item "filesize")))
