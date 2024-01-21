#!/bin/sh

#----------------------------------------------------------------------------
# DESCRIPTION		
# DATE				2024
# AUTHOR			ss401533@gmail.com                                           
#----------------------------------------------------------------------------
# template found at ~/.vim/sh_header.temp

set -o errexit

cat <<EOF | batcat --plain --paging=never --language sh --theme TwoDark
gcc -Iinclude test.c -o test.mac
EOF

