#!/bin/bash

ARCH=aarch64
SYSROOT=$(pwd)/../sysroot

GCC_SRC_GIT_URL=https://bitbucket.org/hayashin/gcc.git
GCC_SRC_URL_BRANCH=osport/gcc-9.2.0
#GCC_SRC_URL_BRANCH=osport/gcc-8.3.0


TARBALLS="gmp-6.1.2.tar.bz2 mpc-1.1.0.tar.gz mpfr-4.0.2.tar.gz"

if [ ! -d ${SYSROOT} ]; then
    echo "Sysroot directory missing!" 2>&1
    exit 1
fi

if [ ! -d $(pwd)/gcc ]; then
    git clone ${GCC_SRC_GIT_URL} -b ${GCC_SRC_URL_BRANCH}
fi

for archive in ${TARBALLS}; do
    format=${archive##*.}
    case "$format" in
        gz)
            args=-zxf
            ;;
        bz2)
            args=-jxf
            ;;
        xz)
            args=-Jxf
            ;;
    esac

    gtar $args $(pwd)/tarballs/$archive

    linkname=${archive%%-*}
    rm -f gcc/$linkname
    cd $(pwd)/gcc && ln -s ../${archive%.*.*} $linkname && cd ..
done

mkdir -p gcc/build && cd gcc/build

CFLAGS_FOR_TARGET=-I${SYSROOT}/usr/include gcc_cv_c_no_fpie=no ac_cv_prog_XSLTPROC=no \
../configure --prefix=/opt/cross-${ARCH} --target=${ARCH}-solaris2.11 --disable-nls \
             --enable-languages=c,c++ --disable-shared -enable-c99 --enable-long-long \
	     --disable-libquadmath --disable-libmudflap --disable-libgomp \
	     --disable-decimal-float --disable-libatomic --disable-libitm \
	     --disable-libquadmath --disable-libsanitizer --disable-libvtv \
	     --disable-libcilkrts --with-system-zlib --enable-__cxa_atexit \
	     --enable-initfini-array --with-headers=${SYSROOT}/usr/include

gcc_cv_c_no_fpie=no ac_cv_prog_XSLTPROC=no \
gmake -j6

exit $?
