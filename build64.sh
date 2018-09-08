#!/bin/bash

/home/arter97/android/ndk/arm64/bin/aarch64-linux-android-clang++ \
    -DHAVE_CONFIG_H -I. -I./src -DHAVE_CONFIG_H -Wall \
    -flto -O3 -march=armv8-a -mtune=cortex-a57 -fPIC -Wl,-pie -pipe -s \
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
       src/splice64.S \
    -lz
