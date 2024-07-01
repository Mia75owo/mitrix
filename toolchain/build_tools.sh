#!/usr/bin/env sh
set -x # show cmds
set -e # fail globally


GCC_VERSION="gcc-12.4.0"
BINUTILS_VERSION="binutils-2.42"

GCC_URL="https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz"
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/$BINUTILS_VERSION.tar.gz"

export PREFIX="$HOME/opt/cross"
export TARGET="i686-elf"
export PATH="$PREFIX/bin:$PATH"

export GNUMAKEFLAGS=-j$(nproc)


# cd into script directory
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd "$SCRIPTPATH"


wget -nc "$BINUTILS_URL"
tar xpvf binutils-*.tar.gz

wget -nc "$GCC_URL"
tar xpvf gcc-*.tar.gz

# Build binutils
mkdir -p binutils
cd binutils

../$BINUTILS_VERSION/configure --target="$TARGET" --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

cd ..

# Build GCC
mkdir -p gcc
cd gcc

../$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

cd ..
