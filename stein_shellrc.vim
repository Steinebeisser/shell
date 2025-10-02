" Vim syntax file
" Language: Stein Shell Rc
" Maintainer: Paul Geisthardt
" Latest Revision: 2025-10-02

if exists("b:current_syntax")
  finish
endif

" ----- base elements -----
syntax keyword stein_shellrcset set
      \ nextgroup=stein_shellrcmodifier
      \           ,stein_shellrccolorkey
      \           ,stein_shellrcpromptkey
      \           ,stein_shellrctimekey
      \           ,stein_shellrcshowcmdpathkey
      \           ,stein_shellrcshowexpandedkey
      \           ,stein_shellrcshowexitkey
      \           ,stein_shellrcshowenvoverridekey
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


syntax match stein_shellrcmodifier /(env)/ contained nextgroup=stein_shellrcenvname skipwhite

syntax match stein_shellrcenvname /\w\+/ contained nextgroup=stein_shellrcenvquals skipwhite
highlight link stein_shellrcenvname define

syntax match stein_shellrcenvquals /=/ contained nextgroup=stein_shellrcenvvalue skipwhite
highlight link stein_shellrcenvquals operator

syntax match stein_shellrcenvvalue /.*$/ contained
highlight link stein_shellrcenvvalue special

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

" ----- timeout key chain -----
syntax match stein_shellrctimekey /\<timeout\>/ contained nextgroup=stein_shellrctimequals skipwhite
highlight link stein_shellrctimekey Type

syntax match stein_shellrctimequals /=/ contained nextgroup=stein_shellrctimevalue skipwhite
highlight link stein_shellrctimequals Operator

" Match the number, including negative numbers
syntax match stein_shellrctimevalue /\v-?\d+/ contained
highlight link stein_shellrctimevalue Number

" ----- show_cmd_path key chain -----
syntax match stein_shellrcshowcmdpathkey /\<show_cmd_path\>/ contained nextgroup=stein_shellrcshowcmdpathequals skipwhite
highlight link stein_shellrcshowcmdpathkey Type

syntax match stein_shellrcshowcmdpathequals /=/ contained nextgroup=stein_shellrcshowcmdpathvalue skipwhite
highlight link stein_shellrcshowcmdpathequals Operator

" allowed values: 1, 0, true, false
syntax match stein_shellrcshowcmdpathvalue /\v(1|0|true|false)/ contained
highlight link stein_shellrcshowcmdpathvalue Constant

" ----- show_expanded_alias key chain -----
syntax match stein_shellrcshowexpandedkey /\<show_expanded_alias\>/ contained nextgroup=stein_shellrcshowexpandedequals skipwhite
highlight link stein_shellrcshowexpandedkey Type

syntax match stein_shellrcshowexpandedequals /=/ contained nextgroup=stein_shellrcshowexpandedvalue skipwhite
highlight link stein_shellrcshowexpandedequals Operator

syntax match stein_shellrcshowexpandedvalue /\v(1|0|true|false)/ contained
highlight link stein_shellrcshowexpandedvalue Constant

" ----- exit_code key chain -----
syntax match stein_shellrcshowexitkey /\<show_exit_code\>/ contained nextgroup=stein_shellrcshowexitequals skipwhite
highlight link stein_shellrcshowexitkey Type

syntax match stein_shellrcshowexitequals /=/ contained nextgroup=stein_shellrcshowexitvalue skipwhite
highlight link stein_shellrcshowexitequals Operator

syntax match stein_shellrcshowexitvalue /\v(1|0|true|false)/ contained
highlight link stein_shellrcshowexitvalue Constant

" ----- allow_env_override key chain -----
syntax match stein_shellrcshowenvoverridekey /\<allow_env_override\>/ contained nextgroup=stein_shellrcshowenvoverrideequals skipwhite
highlight link stein_shellrcshowenvoverridekey Type

syntax match stein_shellrcshowenvoverrideequals /=/ contained nextgroup=stein_shellrcshowenvoverridevalue skipwhite
highlight link stein_shellrcshowenvoverrideequals Operator

syntax match stein_shellrcshowenvoverridevalue /\v(1|0|true|false)/ contained
highlight link stein_shellrcshowenvoverridevalue Constant

" ----- comments -----
syntax match stein_shellrccomment /:.*$/ containedin=ALL
highlight link stein_shellrccomment Comment

let b:current_syntax = "stein_shellrc"
