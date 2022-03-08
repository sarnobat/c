gcc /Volumes/numerous/usr/local/homebrew/Cellar/libexif/0.6.23/lib/libexif.12.dylib -I/Volumes/numerous/usr/local/homebrew/Cellar/libexif/0.6.23/include/  photographer.c -o photographer

find ~/trash/ -iname "*jpg" | xargs --delimiter '\n' --max-args=1 ./photographer | grep -v readable
