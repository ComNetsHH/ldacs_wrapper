#!/bin/bash
cd avionic-rlc
git pull
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd ../glue-lib
git pull
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd ../mc-sotdma
git pull
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd ..
