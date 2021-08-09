" Code stolen from Tsoding's bm
" Addr: https://github.com/tsoding/bm/blob/master/tools/basm.vim

" Vim syntax file
" Language: glasm

" Usage Instructions
" Put this file in .vim/syntax/glasm.vim
" and add in your .vimrc file the next line:
" autocmd BufRead,BufNewFile *.glasm set filetype=glasm

if exists("b:current_syntax")
  finish
endif

syntax keyword glasmTodos TODO XXX FIXME NOTE

" Language keywords
syntax keyword glasmKeywords nop pop dup halt
syntax keyword glasmKeywords push pusht pushf push0 push1 pushn1
syntax keyword glasmKeywords pushu0 pushu1 pushf0 pushf1 pushfn1
syntax keyword glasmKeywords jmp jt jf je jne
syntax keyword glasmKeywords rjmp rjt rjf rje rjne
syntax keyword glasmKeywords call ret swap
syntax keyword glasmKeywords and or xor not shl shr
syntax keyword glasmKeywords andb orb xorb notb
syntax keyword glasmKeywords add sub mul div neg
syntax keyword glasmKeywords eq neq lt lte gt gte
syntax keyword glasmKeywords inc dec
syntax keyword glasmKeywords alloc realloc free
syntax keyword glasmKeywords readi writei scani
syntax keyword glasmKeywords readu writeu scanu
syntax keyword glasmKeywords readf writef scanf
syntax keyword glasmKeywords readc writec scanc
syntax keyword glasmKeywords readb writeb scanb
syntax keyword glasmKeywords scans
syntax keyword glasmKeywords print println prints printsln
syntax keyword glasmKeywords i2u u2i u2b b2u
syntax keyword glasmKeywords i2f f2i i2b b2i n2b

syntax keyword glasmLoopKeywords from to do

" Comments
syntax region glasmCommentLine start=";" end="$"   contains=glasmTodos
syntax region glasmDirective start="%" end=" "

syntax match glasmLabel		"[a-zA-Z_][a-zA-Z0-9_]*:"he=e-1

" Numbers
syntax match glasmDecInt display "\<[0-9][0-9_]*\%([u]*\)"
syntax match glasmHexInt display "\<0[xX][0-9a-fA-F][0-9_a-fA-F]*"
syntax match glasmFloat  display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)"

" Strings
syntax region glasmString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region glasmString start=/\v'/ skip=/\v\\./ end=/\v'/

" Set highlights
highlight default link glasmTodos Todo
highlight default link glasmKeywords Identifier
highlight default link glasmCommentLine Comment
highlight default link glasmDirective PreProc
highlight default link glasmLoopKeywords PreProc
highlight default link glasmDecInt Number
highlight default link glasmHexInt Number
highlight default link glasmFloat Float
highlight default link glasmString String
highlight default link glasmLabel Label

let b:current_syntax = "glasm"
