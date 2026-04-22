; source: https://stackoverflow.com/a/9825272

; run: emacs --load indent.el --batch scheme_files

(defun my/indent-file (fPath)
  (let ((buffer (find-file fPath)))
    (message (concat "indenting file " fPath))
    (indent-region (point-min) (point-max))
    (save-buffer)
    (kill-buffer buffer)))

(mapcar 'my/indent-file argv)
