scriptencoding utf-8
set nocompatible               " be iMproved
filetype off
set rtp+=$VIM/vimfiles/bundle/vundle/
call vundle#rc('$VIM/vimfiles/bundle/') 
Bundle 'gmarik/vundle'  
filetype plugin indent on  
  
" original repos on github<br>Bundle 'mattn/zencoding-vim'  
Bundle 'drmingdrmer/xptemplate'  
   
" vim-scripts repos  
Bundle 'L9'  
Bundle 'FuzzyFinder'  
Bundle 'bufexplorer.zip'  
"Bundle 'taglist.vim'  
Bundle 'Mark'  
Bundle 'The-NERD-tree'  
Bundle 'matrix.vim'  
Bundle 'closetag.vim'  
Bundle 'The-NERD-Commenter'  
Bundle 'matchit.zip'  
Bundle 'AutoComplPop'  
Bundle 'jsbeautify'  
Bundle 'YankRing.vim'  
Bundle 'bling/vim-airline'
Bundle 'easymotion/vim-easymotion'
Bundle 'MattesGroeger/vim-bookmarks'
Bundle 'tomasr/molokai'
Bundle 'scrooloose/syntastic' 
Bundle 'kien/ctrlp.vim'
"Bundle 'project.vim'
Bundle 'project.tar.gz'
Bundle 'majutsushi/tagbar'
"Bundle 'szw/vim-tags'
Bundle 'MarcWeber/vim-addon-mw-utils'
Bundle 'tomtom/tlib_vim'
Bundle 'garbas/vim-snipmate'
Bundle 'honza/vim-snippets'

Bundle 'tComment'
Bundle "Yggdroot/indentLine"
Bundle "tpope/vim-commentary"


filetype plugin indent on     " required!  


nmap s <Plug>(easymotion-s2)
"map  / <Plug>(easymotion-sn)
"omap / <Plug>(easymotion-tn)
"map  n <Plug>(easymotion-next)
"map  N <Plug>(easymotion-prev)
let g:EasyMotion_smartcase = 1

nnoremap S diw"0Pb


"------------------------------------------------------------
"bookmark
"------------------------------------------------------------
"highlight BookmarkSign ctermbg=NONE ctermfg=160
"highlight BookmarkLine ctermbg=194 ctermfg=NONE
highlight link BookmarkSign CtrlPdark
highlight link BookmarkLine CtrlPdark

"highlight link BookmarkSign  Todo



let g:bookmark_sign = '♥'
let g:bookmark_highlight_lines = 1
let g:bookmark_sign = '>>'
let g:bookmark_annotation_sign = '##'
let g:bookmark_highlight_lines = 1
"let g:bookmark_auto_close = 1
"------------------------------------------------------------

" CtrlP
"------------------------------------------------------------
let g:ctrlp_map = '<c-p>'
let g:ctrlp_cmd = 'CtrlP'
let g:ctrlp_working_path_mode = 'ra'
"let g:ctrlp_custom_ignore = '\v[\/]\.(git|hg|svn)$'
let g:ctrlp_custom_ignore = {
	\ 'dir':  '\v[\/]\.(git|hg|svn)$',
	\ 'file': '\v\.(exe|so|dll|lib)$',
	\ 'link': 'SOME_BAD_SYMBOLIC_LINKS',
	\ }
set wildignore+=*\\tmp\\*,*.swp,*.zip,*.exe,*.dll,*.lib
let g:ctrlp_user_command = 'dir %s /-n /b /s /a-d'  " Windows
"------------------------------------------------------------

"------------------------------------------------------------
let g:molokai_original = 1
let g:rehash256 = 1
let g:molokai_original = 1
"------------------------------------------------------------

let $PROJECT_HOME='C:\Users\prudence_lin\Documents\00_work'


" set status line
set laststatus=2
" enable powerline-fonts
let g:airline_powerline_fonts = 1
let g:Powerline_symbols = 'fancy'
set encoding=utf-8
set t_Co=256
"set fillchars+=stl:\ ,stlnc:\
"let g:Powerline_mode_V="V·LINE"
"let g:Powerline_mode_cv="V·BLOCK"
"let g:Powerline_mode_S="S·LINE"
"let g:Powerline_mode_cs="S·BLOCK"
"set guifont=Liberation_Mono_for_Powerline:h10 

"set encoding=utf-8
"let g:airline_powerline_fonts = 1
"set guifont=Fira\ Mono\ Medium\ for\ Powerline:h9

    set encoding=utf-8
    set guifont=Consolas\ for\ Powerline\ FixedD:h9
	 "set guifont=Consolas_for_Powerline_FixedD:h9
    let g:Powerline_symbols="fancy"

  if !exists('g:airline_symbols')
    let g:airline_symbols = {}
  endif

let g:airline_symbols = {}
let g:airline_left_sep = "\u2b80" "use double quotes here
let g:airline_left_alt_sep = "\u2b81"
let g:airline_right_sep = "\u2b82"
let g:airline_right_alt_sep = "\u2b83"
let g:airline_symbols.branch = "\u2b60"
let g:airline_symbols.readonly = "\u2b64"
let g:airline_symbols.linenr = "\u2b61"

" enable tabline
let g:airline#extensions#tabline#enabled = 1
" set left separator
let g:airline#extensions#tabline#left_sep = ' '
" set left separator which are not editting
let g:airline#extensions#tabline#left_alt_sep = '|'
" show buffer number
let g:airline#extensions#tabline#buffer_nr_show = 1

  let g:airline#extensions#tagbar#enabled = 1
 " let g:airline#extensions#tagbar#flags = ''  (default)
  let g:airline#extensions#tagbar#flags = 'f'
 " let g:airline#extensions#tagbar#flags = 'p'


  "let g:airline#extensions#ctrlp#color_template = 'insert' (default)
  "let g:airline#extensions#ctrlp#color_template = 'normal'
  "let g:airline#extensions#ctrlp#color_template = 'visual'
  "let g:airline#extensions#ctrlp#color_template = 'replace'
  let g:airline#extensions#ctrlp#show_adjacent_modes = 1


autocmd StdinReadPre * let s:std_in=1
"autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif


nmap <F5> :TagbarToggle<CR>
map <F6> :BufExplorer<CR>
nmap <silent> <F7> <Plug>ToggleProject
map <F8> :NERDTreeToggle<CR>




autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTreeType") && b:NERDTreeType == "primary") | q | endif

set nocompatible
source $VIMRUNTIME/vimrc_example.vim
source $VIMRUNTIME/mswin.vim
behave mswin
colorscheme koehler 
"set guifont=Monospace\10,courier_new:h10
set tabstop=4
set shiftwidth=4
"set guifont=Courier\ New/12
"set guifont=Monospace\ 12,Courier New\12
set syntax=c
set nu


"set guifont=Monaco:h9
"set gfw=YaHei\ Consolas\ Hybrid:h9
"set guifont=Consolas:h10
"set guifont=YaHei\ Consolas\ Hybrid:h12
"set guifont=Yahei\ Mono:h12



set diffexpr=MyDiff()
function MyDiff()
  let opt = '-a --binary '
  if &diffopt =~ 'icase' | let opt = opt . '-i ' | endif
  if &diffopt =~ 'iwhite' | let opt = opt . '-b ' | endif
  let arg1 = v:fname_in
  if arg1 =~ ' ' | let arg1 = '"' . arg1 . '"' | endif
  let arg2 = v:fname_new
  if arg2 =~ ' ' | let arg2 = '"' . arg2 . '"' | endif
  let arg3 = v:fname_out
  if arg3 =~ ' ' | let arg3 = '"' . arg3 . '"' | endif
  let eq = ''
  if $VIMRUNTIME =~ ' '
    if &sh =~ '\<cmd'
      let cmd = '""' . $VIMRUNTIME . '\diff"'
      let eq = '"'
    else
      let cmd = substitute($VIMRUNTIME, ' ', '" ', '') . '\diff"'
    endif
  else
    let cmd = $VIMRUNTIME . '\diff'
  endif
  silent execute '!' . cmd . ' ' . opt . arg1 . ' ' . arg2 . ' > ' . arg3 . eq
endfunction

"--------------------------------------------------------------------------------
" Highlight all instances of word under cursor, when idle.
" Useful when studying strange source code.
" Type z/ to toggle highlighting on/off.
nnoremap z/ :if AutoHighlightToggle()<Bar>set hls<Bar>endif<CR>
function! AutoHighlightToggle()
  let @/ = ''
  if exists('#auto_highlight')
    au! auto_highlight
    augroup! auto_highlight
    setl updatetime=4000
    echo 'Highlight current word: off'
    return 0
  else
    augroup auto_highlight
      au!
      au CursorHold * let @/ = '\V\<'.escape(expand('<cword>'), '\').'\>'
    augroup end
    setl updatetime=1500
    echo 'Highlight current word: ON'
    return 1
  endif
endfunction
"--------------------------------------------------------------------------------

