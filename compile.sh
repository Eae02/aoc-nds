#!/bin/bash
rm -R obj/* 2> /dev/null
set -e

COMPILER="$DEVKITARM/bin/arm-none-eabi-g++"
NDSTOOL="$DEVKITPRO/tools/bin/ndstool"
ARCH="-mthumb -mthumb-interwork"
CFLAGS="-Wall -g -O2 --std=c++20 -Wno-volatile -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -fno-rtti -fno-exceptions -I$DEVKITPRO/libnds/include -DARM9 $ARCH"
LFLAGS="-specs=ds_arm9.specs -g -Wl,-Map,nds.map $ARCH -L$DEVKITPRO/libnds/lib -lnds9"

mkdir -p src/inputs
for f in $(find inputs -name "*.txt"); do
	xxd -i $f > src/$f.h
done

echo "compiling..."
for f in $(find src -name "*.cpp"); do
	mkdir -p obj/$(dirname $f)
	$COMPILER $f $CFLAGS -c -o obj/$f.o &
done

wait

echo "linking..."
$COMPILER $(find obj -name "*.cpp.o") $LFLAGS -o aoc.elf

$NDSTOOL -c aoc.nds -9 aoc.elf
