#!/bin/bash

builddir='Build'
debugdir='Debug'
function linkfile {
    echo 'link file'
    [[ -f ../16M.txt ]] && ln -s ../16M.txt
    [[ -f ../1M.txt ]] && ln -s ../1M.txt
    [[ -f ../index.html ]] && ln -s ../index.html
    [[ -f ../sw.conf ]] && ln -s ../sw.conf
}
case $1 in 
    'build')
	[[ ! -f $builddir ]] && mkdir $builddir
	cd $builddir
	linkfile
	cmake .. && cmake --build .
	;;
    'debug')
	[[ ! -f $debugdir ]] && mkdir $debugdir
	cmake -H. -BDebug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
	cd $debugdir
	cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake -build .
	linkfile
	cmake .. && cmake --build .
	;;
esac

