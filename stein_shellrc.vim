" Vim syntax file
" Language: Stein Shell Rc
" Maintainer: Paul Geisthardt
" Latest Revision: 2025-09-28

if exists("b:current_syntax")
  finish
endif

" ----- base elements -----
syntax keyword stein_shellrcset set
      \ nextgroup=stein_shellrcmodifier
      \           ,stein_shellrccolorkey
      \           ,stein_shellrcpromptkey
      \ skipwhite
highlight link stein_shellrcset keyword


syntax region stein_shellrcstring start=+"+ end=+"+ contained
highlight link stein_shellrcstring string

" ----- alias chain -----
syntax match stein_shellrcmodifier /(alias)/ contained nextgroup=stein_shellrcaliasname skipwhite
highlight link stein_shellrcmodifier identifier

syntax match stein_shellrcaliasname /\w\+/ contained nextgroup=stein_shellrcaliasequals skipwhite
highlight link stein_shellrcaliasname define

syntax match stein_shellrcaliasequals /=/ contained nextgroup=stein_shellrcaliasvalue skipwhite
highlight link stein_shellrcaliasequals operator

syntax match stein_shellrcaliasvalue /.*$/ contained contains=stein_shellrcstring
highlight link stein_shellrcaliasvalue special

" ----- color keys chain -----
syntax match stein_shellrccolorkey /\v<(
      \info_color
      \|warn_color
      \|error_color
      \)>/ contained nextgroup=stein_shellrccolorequals skipwhite

highlight link stein_shellrccolorkey type

syntax match stein_shellrccolorequals /=/ contained nextgroup=stein_shellrccolorvalue skipwhite
highlight link stein_shellrccolorequals operator

syntax region stein_shellrccolorvalue start=/./ end=/$/ contained transparent contains=stein_shellrccolorname,stein_shellrcnumbervalue

syntax keyword stein_shellrccolorname
            \ black
            \ red
            \ green
            \ yellow
            \ blue
            \ magenta
            \ cyan
            \ white
            \ bright_black
            \ bright_red
            \ bright_green
            \ bright_yellow
            \ bright_blue
            \ bright_magenta
            \ bright_cyan
            \ bright_white
            \ contained
highlight link stein_shellrccolorname constant

syntax match stein_shellrcnumbervalue /\d\+/ contained
highlight link stein_shellrcnumbervalue number

" ----- prompt key chain -----
syntax match stein_shellrcpromptkey /\<prompt\>/ contained nextgroup=stein_shellrcpromptequals skipwhite
highlight link stein_shellrcpromptkey type

syntax match stein_shellrcpromptequals /=/ contained nextgroup=stein_shellrcpromptvalue skipwhite
highlight link stein_shellrcpromptequals operator

syntax region stein_shellrcpromptvalue start=/./ end=/$/ contained transparent contains=stein_shellrcstring

" ----- comments -----
syntax match stein_shellrccomment /:.*$/ containedin=ALL
highlight link stein_shellrccomment Comment

let b:current_syntax = "stein_shellrc"
