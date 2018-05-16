#!/bin/sh 

echo "start build ..."

mkdir build

cd build

cmake ..

make -j3

cd ..

echo "OK"

