#!/bin/sh

#----------------------------------------------------------------------------
# DESCRIPTION		
# DATE				2024
# AUTHOR			ss401533@gmail.com                                           
#----------------------------------------------------------------------------
# template found at ~/.vim/sh_header.temp

set -o errexit
echo "$0" | tee -a ~/db.git/command_history.txt | ts >> ~/db.git/command_history_timestamped.txt

cat <<EOF | batcat --style=plain --paging=never --language sh --theme TwoDark
works
	/Volumes/git/github/c_helloworld/6_exif_library
EOF

