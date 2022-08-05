#!/bin/bash
cd $1
for f in ../patches/*.patch
do
  echo "Applying $f"
  patch -p1 < $f
done
cd ..
