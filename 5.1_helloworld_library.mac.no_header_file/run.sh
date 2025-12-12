#!/bin/sh

#----------------------------------------------------------------------------
# DESCRIPTION			
# DATE				2025
# AUTHOR			ss401533@gmail.com
#----------------------------------------------------------------------------
# template found at ~/.vim/sh_header.temp

set -o errexit
echo `date +%s::`"$0" >> ~/db.git/command_history.txt >> ~/db.git/command_history_timestamped.txt

cat <<EOF | batcat --style=plain --paging=never --language sh --theme TwoDark
C call library function without header (just declare the function but don't implement it)
C opengl code sample)

do something similar to the openGL callback from Imperial
EOF

