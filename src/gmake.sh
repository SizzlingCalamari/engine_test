#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
  ./premake5_osx gmake
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
  echo "windows? don't use this"
  exit 1
else
  ./premake5 gmake
fi
