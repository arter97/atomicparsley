#!/bin/bash

/home/arter97/android/ndk/arm32/bin/arm-linux-androideabi-clang++ \
    -DHAVE_CONFIG_H -I. -I./src -DHAVE_CONFIG_H -Wall \
    -O3 -march=armv7-a -mtune=cortex-a15.cortex-a7 -mfloat-abi=softfp -mfpu=neon -fPIC -Wl,-pie -pipe -s \
    -o AtomicParsley \
       src/util.cpp \
       src/arrays.cpp \
       src/iconv.cpp \
       src/parsley.cpp \
       src/extracts.cpp \
       src/sha1.cpp \
       src/uuid.cpp \
       src/id3v2.cpp \
       src/metalist.cpp \
       src/CDtoc.cpp \
       src/compress.cpp \
       src/main.cpp \
       src/errno.cpp \
       src/splice.S \
    -lz
