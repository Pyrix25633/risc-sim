.RECIPEPREFIX = >
CC = g++
CFLAGS = -ljsoncpp -lSDL2main -lSDL2 -lSDL2_image
WININCLUDES = -I include -I libs/SDL2/include -I C:/C++
WINCFLAGS = -L libs/SDL2/lib -lSDL2main -lSDL2 -lSDL2_image -L libs/jsoncpp/build-shared -ljsoncpp
DEBUGFLAGS = -c src/*.cpp -std=c++14 -m64 -g -I include
RELEASEFLAGS = -c src/*.cpp -std=c++14 -m64 -O3 -I include
VERSION = 0.1.0
NAME = risc-sim

default:
> make debug

win:
> make debug-win

remove:
> rm *.o

debug:
> clear
> make build-debug
> make run-debug

debug-win:
> clear
> make build-debug-win
> make run-debug-win

release:
> clear
> make build-release
> make run-release

release-win:
> clear
> make build-release-win
> make run-release-win

build-debug:
> $(CC) $(DEBUGFLAGS) && $(CC) *.o -o bin/debug/debug $(CFLAGS)

build-debug-win:
> $(CC) $(DEBUGFLAGS) $(WININCLUDES) && $(CC) *.o -o bin/debug/debug.exe $(WINCFLAGS)

build-release:
> $(CC) $(RELEASEFLAGS) && $(CC) *.o -o bin/release/$(NAME)-$(VERSION) -s $(CFLAGS)

build-release-win:
> $(CC) $(RELEASEFLAGS) $(WININCLUDES) && $(CC) *.o -o bin/release/$(NAME)-$(VERSION).exe -s $(WINCFLAGS)

run-debug:
> ./bin/debug/debug

run-debug-win:
> ./bin/debug/debug.exe

run-release:
> ./bin/release/$(NAME)-$(VERSION)

run-release-win:
> ./bin/release/$(NAME)-$(VERSION).exe