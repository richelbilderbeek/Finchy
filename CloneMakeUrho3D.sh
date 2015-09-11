#!/bin/sh
if [ ! -d Urho3D ]
then
  git clone https://github.com/Urho3D/Urho3D
fi

cd Urho3D 
cmake .
make
./cmake_generic.sh
cd Source
cd ThirdParty
cd Bullet
make

cd ..
cd ..
cd ..
cd ..