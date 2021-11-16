#!/bin/bash

cd /mnt/lfs/sources/gcc
echo "Script is saying pwd is now: " 
pwd
#cat gcc/limitx.h gcc/glimits.h gcc/limity.h > `dirname $($LFS_TGT-gcc -print-libgcc-file-name)`/install-tools/include/limits.h
cat gcc/limitx.h gcc/glimits.h gcc/limity.h > $(find /mnt/lfs/tools -type d -name "install-tools" | grep -v libexec)/include/limits.h
