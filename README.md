# ProgUniv
ProgUniv is intended to be a Universal Chip programmer device.

Currently it supports bit-banging several ATMEL/Microchip 8-bit AVR chips through the old parallel port (LPT) and requires no clock source, i.e., it allows recovering bricked ATMELs without a crystal.

It should compile and work on Linux, Windows 32-bit and 64-bits.

To compile natively, do:
libtoolize
aclocal
autoconf
automake --add-missing
./configure
make
