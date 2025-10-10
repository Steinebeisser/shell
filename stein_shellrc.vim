" Vim syntax file
" Language: Stein Shell Rc
" Maintainer: Paul Geisthardt
" Latest Revision: 2025-10-10

" To add new known keywords with known values
" Copy the chain pattern (Key -> Equals -> Value)
" Exmaple using nusic keyword with known values: Gareth Coker, Howard Jones
" and Nerdout
"
" (Set)
" Add the key match group to stein_shellrcSet
"
" (KEY)
" syntax match stein_shellrcMusicKey /music/ contained nextgroup=stein_shellrcMusicEquals skipwhite
" highlight link stein_shellrcMusicKey Type
"
" (EQUALS)
" syntax match stein_shellrcMusicEquals /=/ contained nextgroup=stein_shellrcMusicValue skipwhite
" highlight link stein_shellrcMusicEquals Operator
"
" (VALUE)
" Here because we have mutli character values we must use regex, else we could
" use keyword to match single word values
" syntax match stein_shellrcMusicValue /\v(Gareth Coker|Howard Jones|Nerdout)/ contained
" highlight link stein_shellrcMusicValue Constant
"
" Example Value with Keyword
" syntax keyword sten_shellrcMusicValue GarethCoker HowardJones Nerdout contained

if exists("b:current_syntax")
  finish
endif

" ----- base elements -----
syntax keyword stein_shellrcset set
      \ nextgroup=stein_shellrcmodifier
      \           ,stein_shellrccolorkey
      \           ,stein_shellrcpromptkey
      \           ,stein_shellrcmusickey
      \           ,stein_shellrctimekey
      \           ,stein_shellrcshowcmdpathkey
      \           ,stein_shellrcshowexpandedkey
      \           ,stein_shellrcshowexitkey
      \           ,stein_shellrcshowenvoverridekey
      \           ,stein_shellrcterminalmodekey
      \           ,stein_shellrchistoryfilekey
      \           ,stein_shellrchistorykey
      \           ,stein_shellrchistorylenkey
      \           ,stein_shellrchistoryfilepathkey
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

syntax match stein_shellrcaliasvalue /.*$/ contained
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

" ----- terminal key chain -----
syntax match stein_shellrcterminalmodekey /\<terminal_mode\>/ contained nextgroup=stein_shellrcterminalmodeequals skipwhite
highlight link stein_shellrcterminalmodekey Type

syntax match stein_shellrcterminalmodeequals /=/ contained nextgroup=stein_shellrcterminalmodevalue skipwhite
highlight link stein_shellrcterminalmodeequals Operator

syntax match stein_shellrcterminalmodevalue /\v(normal|vi|emacs)/ contained
highlight link stein_shellrcterminalmodevalue Constant

" ----- historyfile key chain -----
syntax match stein_shellrchistoryfilekey /\<enable_history_file\>/ contained nextgroup=stein_shellrchistoryfileequals skipwhite
highlight link stein_shellrchistoryfilekey Type

syntax match stein_shellrchistoryfileequals /=/ contained nextgroup=stein_shellrchistoryfilevalue skipwhite
highlight link stein_shellrchistoryfileequals Operator

syntax match stein_shellrchistoryfilevalue /\v(1|0|true|false)/ contained
highlight link stein_shellrchistoryfilevalue Constant

" ----- history key chain -----
syntax match stein_shellrchistorykey /\<enable_history\>/ contained nextgroup=stein_shellrchistoryequals skipwhite
highlight link stein_shellrchistorykey Type

syntax match stein_shellrchistoryequals /=/ contained nextgroup=stein_shellrchistoryvalue skipwhite
highlight link stein_shellrchistoryequals Operator

syntax match stein_shellrchistoryvalue /\v(1|0|true|false)/ contained
highlight link stein_shellrchistoryvalue Constant

" ----- historylen key chain -----
syntax match stein_shellrchistorylenkey /\<max_history_len\>/ contained nextgroup=stein_shellrchistorylenequals skipwhite
highlight link stein_shellrchistorylenkey Type

syntax match stein_shellrchistorylenequals /=/ contained nextgroup=stein_shellrchistorylenvalue skipwhite
highlight link stein_shellrchistorylenequals Operator

syntax match stein_shellrchistorylenvalue /\d\+/ contained
highlight link stein_shellrchistorylenvalue Constant


" ----- historyfilepath key chain -----
syntax match stein_shellrchistoryfilepathkey /\<history_file\>/ contained nextgroup=stein_shellrchistoryfilepathequals skipwhite
highlight link stein_shellrchistoryfilepathkey Type

syntax match stein_shellrchistoryfilepathequals /=/ contained nextgroup=stein_shellrchistoryfilepathvalue skipwhite
highlight link stein_shellrchistoryfilepathequals Operator

syntax match stein_shellrchistoryfilepathvalue /.*$/ contained
highlight link stein_shellrchistoryfilepathvalue special

" ----- comments -----
syntax match stein_shellrccomment /:.*$/ containedin=ALL
highlight link stein_shellrccomment Comment

let b:current_syntax = "stein_shellrc"
