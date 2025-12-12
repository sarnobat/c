#!/bin/sh

#----------------------------------------------------------------------------
# DESCRIPTION		
# DATE				2025
# AUTHOR			ss401533@gmail.com                                           
#----------------------------------------------------------------------------
# template found at ~/.vim/sh_header.temp

set -o errexit
echo "$0" >> ~/db.git/command_history.txt | ts >> ~/db.git/command_history_timestamped.txt

cat <<'EOF' | batcat --style=plain --paging=never --language sh --theme TwoDark
gcc -g `pkg-config --cflags --libs gtk+-3.0` -o hello_world.mac.m1 hello_world.c
EOF

