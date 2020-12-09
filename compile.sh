#!/bin/bash
rm -R obj/* 2> /dev/null
set -e

COMPILER="$DEVKITARM/bin/arm-none-eabi-g++"
NDSTOOL="$DEVKITPRO/tools/bin/ndstool"
ARCH="-mthumb -mthumb-interwork"
CFLAGS="-Wall -g -O2 --std=c++20 -Wno-volatile -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -fno-rtti -fno-exceptions -I$DEVKITPRO/libnds/include -DARM9 $ARCH"
LFLAGS="-specs=ds_arm9.specs -g -Wl,-Map,nds.map $ARCH -L$DEVKITPRO/libnds/lib -lfilesystem -lfat -lnds9"

mkdir -p src/res
$DEVKITPRO/tools/bin/grit res/bkg.png -fts -osrc/res/bkg

if [[ -d "inp" ]]; then
	CFLAGS="$CFLAGS -DEMBEDDED_INPUTS"
	mkdir -p src/inp
	echo "" > src/inputs.h
	for f in $(find inp -name "*.txt"); do
		xxd -i $f >> src/inputs.h
	done
fi

echo "compiling..."
for f in $(find src -name "*.cpp"); do
	mkdir -p obj/$(dirname $f)
	$COMPILER $f $CFLAGS -c -o obj/$f.o &
done

$COMPILER -x assembler-with-cpp -g -mthumb -mthumb-interwork -c src/res/bkg.s -o obj/src/bkg.o

wait

echo "linking..."
$COMPILER $(find obj -name "*.o") $LFLAGS -o aoc.elf

$NDSTOOL -c aoc.nds -9 aoc.elf
