#!bin/bash

BIN=bin
LIB=lib
BUILD=build

#注意：如果系统匹配失败可根据失败后的输出内容调整以下字符串内容再重新执行
mac="Darwin"
linux="Linux"
windows="MINGW64"

#清空文件夹
clearDir(){
    if [ ! -d $1 ];then
        mkdir $1
    else
        rm -rf $1/*
    fi 
}

clearDir $BIN;
clearDir $LIB;
clearDir $BUILD;

cd $BUILD

ret=`uname  -a`

if [[ $ret =~ $windows ]];then
    cmake -G "MinGW Makefiles" ..
    mingw32-make.exe
elif [[ $ret =~ $linux ]];then
    cmake ..
    make
else
    echo $ret
    exit 1
fi


#直接执行可执行文件
cd ../$BIN
./mytest

