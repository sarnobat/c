# gcc /Volumes/numerous/usr/local/homebrew/Cellar/libexif/0.6.23/lib/libexif.12.dylib -I/Volumes/numerous/usr/local/homebrew/Cellar/libexif/0.6.23/include/  photographer.c -o photographer
gcc /opt/homebrew/Cellar/libexif/0.6.24/lib/libexif.dylib	-I/opt/homebrew/Cellar/libexif/0.6.24/include photographer.c -o photographer.m1.osx

find ~/trash/ -iname "*jpg" | xargs --delimiter '\n' --max-args=1 ./photographer.m1.osx | grep -v readable
