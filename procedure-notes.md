### Notes of my very first try to do this once again (2020713T215800ZCEST)

Link to the multilib LFS fork I am using: http://www.linuxfromscratch.org/~thomas/multilib/prologue/organization.html


Thanks you so much to the creators and maintainers of this version! :)



#### Part II - Preparing for the Build

##### Chapter 2 - Preparing the host system

###### Subchapter 2.2 - Host System Requirements

* Run script that checks whether or not requirements are fullfilled
bash version-check.sh

###### Subchapter 2.5 - Creating a File System on the Partition

mkfs -v -t ext4 /dev/<xxx>

###### Subchapter 2.6 - Setting The $LFS Variable

export LFS=/mnt/lfs

###### Subchapter 2.7 - Mounting the New Partition

mkdir -pv $LFS
mount -v -t ext4 /dev/<xxx> $LFS

if more paritions like /home are wanted:

mkdir -v $LFS/home
mount -v -t ext4 /dev/<yyy> $LFS/home

##### Chapter 3 - Packages and Patches

###### Subchapter 3.1 - Introduction

mkdir -v $LFS/sources
chmod -v a+wt $LFS/sources
wget --input-file=wget-list --continue --directory-prefix=$LFS/sources
pushd $LFS/sources
  md5sum -c md5sums
popd

##### Chapter 4 - Final Preparations

###### Subchapter 4.2 - Creating the Minimal directory layout in LFS filesystem

mkdir -pv $LFS/{usr,lib,var,etc,bin,sbin}
case $(uname -m) in
  x86_64) mkdir -pv $LFS/lib64 ;;
esac

mkdir -pv $LFS/lib32

mkdir -pv $LFS/tools

###### Subchapter 4.3 - Adding the LFS User

groupadd lfs
useradd -s /bin/bash -g lfs -m -k /dev/null lfs
passwd lfs # SLEEP
chown -v lfs $LFS/{usr,lib,var,etc,bin,sbin,tools}
case $(uname -m) in
  x86_64) chown -v lfs $LFS/lib64 ;;
esac

chown -v lfs $LFS/lib32
chown -v lfs $LFS/sources

su - lfs # SHELL CHANGE

###### Subchapter 4.4 - Setting Up the Environment

cat > ~/.bash_profile << "EOF"
exec env -i HOME=$HOME TERM=$TERM PS1='\u:\w\$ ' /bin/bash
EOF

cat > ~/.bashrc << "EOF"
set +h
umask 022
LFS=/mnt/lfs
LC_ALL=POSIX
LFS_TGT=x86_64-lfs-linux-gnu
LFS_TGT32=i686-lfs-linux-gnu
PATH=/usr/bin
if [ ! -L /bin ]; then PATH=/bin:$PATH; fi
PATH=$LFS/tools/bin:$PATH
export LFS LC_ALL LFS_TGT LFS_TGT32 PATH
EOF

source ~/.bash_profile

###### Subchapter 4.5 - About SBUs
export MAKEFLAGS='-j$(nproc)'

##### Chapter 5 - Compiling a Cross-Toolchain

###### Subchapter 5.2 Binutils-2.34 - Pass 1
cd $LFS/sources
tar xf binutils-2.34.tar.gz && cd binutils-2.34 && mkdir -v build && cd build
../configure --prefix=$LFS/tools       \
             --with-sysroot=$LFS        \
             --target=$LFS_TGT          \
             --disable-nls              \
             --disable-werror           \
             --enable-multilib
make
make install
cd ../../
rm -rf binutils-2.34

###### Subchapter 5.3 - GCC-10.1.0 - Pass 1

tar xf gcc-10.1.0.tar.xz && cd gcc-10.1.0
tar -xf ../mpfr-4.0.2.tar.xz
mv -v mpfr-4.0.2 mpfr
tar -xf ../gmp-6.2.0.tar.xz
mv -v gmp-6.2.0 gmp
tar -xf ../mpc-1.1.0.tar.gz
mv -v mpc-1.1.0 mpc
tar -xf ../isl-0.22.1.tar.xz
mv -v isl-0.22.1 isl

sed -e '/m64=/s/lib64/lib/' \
    -e '/m32=/s/m32=.*/m32=..\/lib32$(call if_multiarch,:i386-linux-gnu)/' \
    -i.orig gcc/config/i386/t-linux64
    
mkdir -v build && cd build

mlist=m64,m32
../configure                                       \
    --target=$LFS_TGT                              \
    --prefix=$LFS/tools                            \
    --with-glibc-version=2.11                      \
    --with-sysroot=$LFS                            \
    --with-newlib                                  \
    --without-headers                              \
    --enable-initfini-array                        \
    --disable-nls                                  \
    --disable-shared                               \
    --enable-multilib --with-multilib-list=$mlist  \
    --disable-decimal-float                        \
    --disable-threads                              \
    --disable-libatomic                            \
    --disable-libgomp                              \
    --disable-libquadmath                          \
    --disable-libssp                               \
    --disable-libvtv                               \
    --disable-libstdcxx                            \
    --enable-languages=c,c++