#!/bin/bash

cmake .
if [[ $? -eq 0 ]]
then
	make
fi

if [[ $? -eq 0 ]]
then
	make
fi

echo "Deleting intermediate files"
rm -rf CMakeFiles cmake_install.cmake CMakeCache.txt Makefile
echo "Files have been deleted"
