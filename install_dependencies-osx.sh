#!/bin/sh
sudo port install cmake
sudo port install mercurial
sudo port install portaudio +universal

if [ -d src/core/sdl2/SDL2-OSX ]; then
    cd src/core/sdl2/SDL2-OSX
    hg update
    cd -
else
    cd src/core/sdl2
    hg clone http://hg.libsdl.org/SDL SDL2-OSX
    cd -
fi

xcodebuild -project 'src/core/sdl2/SDL2-OSX/Xcode/SDL/SDL.xcodeproj' -configuration 'Release' -target 'Static Library' CONFIGURATION_BUILD_DIR='../../lib'
