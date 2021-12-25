;;; glasm-mode.el --- Major Mode for editing BASM Assembly Code -*- lexical-binding: t -*-

(defconst glasm-mode-syntax-table
  (with-syntax-table (copy-syntax-table)
    (modify-syntax-entry ?\; "<")
    (modify-syntax-entry ?\n ">")
    (modify-syntax-entry ?\" "\"")
    (modify-syntax-entry ?\' "\"")
    (syntax-table))
  "Syntax table for `glasm-mode'.")

(eval-and-compile
  (defconst glasm-instructions
    '("nop" "push" "pushf" "pusht" "push0"
      "push1" "pushn1" "pushu0" "pushu1" "pushf0"
      "pushf1" "pushfn1" "pop" "dup" "sdup" "jmp"
      "jt" "jf" "je" "jne" "rjmp"
      "rjt" "rjf" "rje" "rjne" "call"
      "ret" "swap" "sswap" "not" "and" "or" "xor"
	  "add" "sub" "mul" "div" "neg" "eq" "neq"
      "lt" "lte" "gt" "gte" "inc" "read" "write"
      "dec" "scani" "scanu" "scanf" "scanc"
      "scanb" "scans" "print" "println" "prints"
      "printsln" "alloc" "realloc" "free" "readi"
      "readu" "readf" "readc" "readb" "writei"
      "writeu" "writef" "writec" "writeb" "i2u"
      "i2f" "i2c" "u2i" "u2f" "u2c"
      "u2b" "f2i" "f2u" "f2c" "c2i"
      "c2u" "c2f" "b2u" "n2b" "halt")))

(defconst glasm-highlights
  `(("%[[:word:]_]+" . font-lock-preprocessor-face)
    ("[[:word:]_]+\\:" . font-lock-constant-face)
    (,(regexp-opt glasm-instructions 'symbols) . font-lock-keyword-face)))

;;;###autoload
(define-derived-mode glasm-mode prog-mode "glasm"
  "Major Mode for editing glasm Assembly Code."
  (setq font-lock-defaults '(glasm-highlights))
  (set-syntax-table glasm-mode-syntax-table))

;;;###autoload
(add-to-list 'auto-mode-alist '("\\.glasm\\'" . glasm-mode))

(provide 'glasm-mode)

;;; glasm-mode.el ends here
