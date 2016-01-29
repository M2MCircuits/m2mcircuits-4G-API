#!/bin/bash
swig -python cMakerModem.i && gcc -DLINUX -I/usr/local/include/python2.7 -fpic -shared -c *.c -lpython2.7 && ld -shared *.o -o _MakerModem.so
