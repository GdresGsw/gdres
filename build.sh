#!bin/bash

BIN=bin
LIB=lib
BUILD=build

if [ ! -d $BIN ];then
    mkdir $BIN
else
    rm -rf $BIN/*
fi

if [ ! -d $LIB ];then
    mkdir $LIB
else
    rm -rf $LIB/*
fi

if [ ! -d $BUILD ];then
    mkdir $BUILD
else
    rm -rf $BUILD/*
fi

cd $BUILD

ret_str=`uname  -a`
ret=`expr match "$ret_str" "MING"`
if [ ! $ret -eq 0 ];then

    # if [ ! -d $BIN ];then
    #     mkdir $BIN
    # else
    #     rm -rf $BIN/*
    # fi

    # if [ ! -d $LIB ];then
    #     mkdir $LIB
    # else
    #     rm -rf $LIB/*
    # fi

    cmake -G "MinGW Makefiles" ..
    mingw32-make.exe

else
    cmake ..
    make
fi


# cd ../$BIN
# ./threadTest



