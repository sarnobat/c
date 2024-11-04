gcc /opt/homebrew/Cellar/libexif/0.6.24/lib/libexif.dylib	-I/opt/homebrew/Cellar/libexif/0.6.24/include helloworld.c -o helloworld.m1.osx

find ~/trash/ -iname "*jpg" | xargs --delimiter '\n' --max-args=1 ./helloworld.m1.osx | grep -v readable | head
