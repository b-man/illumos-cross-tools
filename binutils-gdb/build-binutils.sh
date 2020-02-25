#!/bin/bash

ARCH=aarch64
BINUTILS_SRC_GIT_URL=https://bitbucket.org/hayashin/binutils-gdb.git
BINUTILS_SRC_BRANCH=osport/binutils-2.34

if [ ! -d $(pwd)/binutils-gdb ]; then
    git clone ${BINUTILS_SRC_GIT_URL} -b ${BINUTILS_SRC_BRANCH}
fi

mkdir -p binutils-gdb/build && cd binutils-gdb/build

../configure --prefix=/opt/cross-aarch64 \
            --disable-nls --with-sysroot \
            --target=${ARCH}-solaris2.11 \
            --enable-initfini-array --disable-gdb


gmake CFLAGS="-Wno-error=unused-value -Wno-error=sign-compare" -j6

exit ${?} 
