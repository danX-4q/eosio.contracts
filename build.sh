#! /bin/bash

printf "\t=========== Building eosio.contracts ===========\n\n"

RED='\033[0;31m'
NC='\033[0m'

CORES=`getconf _NPROCESSORS_ONLN`
if [ -d build ]; then
	pushd build &> /dev/null &&
	make -j${CORES} &&
	popd &> /dev/null || { exit 1; }
else 
	mkdir -p build &&
	pushd build &> /dev/null &&
	cmake ../ &&
	make -j${CORES} &&
	popd &> /dev/null || { exit 1; }
fi
