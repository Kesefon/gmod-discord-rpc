#! /bin/sh
rm libs/ discord-rpc/build/ -r
cd discord-rpc/
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../libs -DUSE_STATIC_CRT=ON -DBUILD_EXAMPLES=OFF -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32
cmake --build . --config Release --target install
cd ../../
g++ -m32 -fPIC GRPCTest.cpp libs/lib/libdiscord-rpc.a -shared -o gmcl_fluffydiscord_linux.dll -Igmod-module-base/include/GarrysMod/Lua -Ilibs/include -fpermissive