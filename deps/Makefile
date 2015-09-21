## This file is part of CoreArray.
##
## CoreArray is free software: you can redistribute it and/or modify it
## under the terms of the GNU Lesser General Public License Version 3 as
## published by the Free Software Foundation.
##
## CoreArray is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with CoreArray.
## If not, see <http://www.gnu.org/licenses/>.


## The command which runs the C compiler.
CC = gcc

## Debugging and optimization options for the C compiler.
CFLAGS = -O3 -fPIC

## The command which runs the C++ compiler.
CXX = g++

## Options for the C++ compiler.
CXXFLAGS = -O3 -fPIC

## Options for linker
LDFLAGS=

## Remove directory entries
RM = rm -rf


## Determine the filename extension
platform=$(shell uname)
ifeq ($(platform),Linux)
	LIBEXT=so
	LINKLIBS=-lpthread
else ifeq ($(platform),Darwin)
	LIBEXT=dylib
	LINKLIBS=-lpthread
else
	LIBEXT=dll
	LINKLIBS=
endif


## CoreArray library archive file
LIB_COREARRAY_A = libCoreArray.a

## CoreArray library object files
LIB_COREOBJS = CoreArray.o dAllocator.o dAny.o dBase.o dBitGDS.o \
	dEndian.o dFile.o dParallel.o dParallel_Ext.o dPlatform.o dSerial.o \
	dStream.o dStruct.o


## zlib library
LIB_ZIP_A = libZLIB.a

## ZLIB library object files
LIB_ZLIBOBJS = adler32.o compress.o crc32.o deflate.o infback.o \
	inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o


## lz4 library
LIB_LZ4_A = libLZ4.a

## LZ4 library object files
LIB_LZ4OBJS = lz4.o lz4hc.o lz4frame.o xxhash.o


## Julia interface
LIB_J_OBJS = J_CoreArray.o


## all jobs
all: $(LIB_COREOBJS) $(LIB_ZLIBOBJS) $(LIB_LZ4OBJS) $(LIB_J_OBJS)
	$(CXX) $(CXXFLAGS) -shared \
		$(LIB_COREOBJS) $(LIB_ZLIBOBJS) $(LIB_LZ4OBJS) $(LIB_J_OBJS) \
		$(LDFLAGS) $(LINKLIBS) -o libCoreArray.$(LIBEXT)


## Clean and remove files
clean:
	$(RM) *.o
	$(RM) libCoreArray.$(LIBEXT)




##########################################################################

##  Julia Interface  ##

J_CoreArray.o: J_CoreArray.cpp
	$(CXX) $(CXXFLAGS) -ICoreArray J_CoreArray.cpp -c -o $@

##  CoreArray Library  ##

CoreArray.o:
	$(CXX) $(CXXFLAGS) CoreArray/CoreArray.cpp -c -o $@

dAllocator.o:
	$(CXX) $(CXXFLAGS) CoreArray/dAllocator.cpp -c -o $@

dAny.o:
	$(CXX) $(CXXFLAGS) CoreArray/dAny.cpp -c -o $@

dBase.o:
	$(CXX) $(CXXFLAGS) CoreArray/dBase.cpp -c -o $@

dBitGDS.o:
	$(CXX) $(CXXFLAGS) CoreArray/dBitGDS.cpp -c -o $@

dEndian.o:
	$(CXX) $(CXXFLAGS) CoreArray/dEndian.cpp -c -o $@

dFile.o:
	$(CXX) $(CXXFLAGS) CoreArray/dFile.cpp -c -o $@

dParallel.o:
	$(CXX) $(CXXFLAGS) CoreArray/dParallel.cpp -c -o $@

dParallel_Ext.o:
	$(CC) $(CFLAGS) CoreArray/dParallel_Ext.c -c -o $@

dPlatform.o:
	$(CXX) $(CXXFLAGS) CoreArray/dPlatform.cpp -c -o $@

dSerial.o:
	$(CXX) $(CXXFLAGS) CoreArray/dSerial.cpp -c -o $@

dStream.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStream.cpp -c -o $@

dStruct.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStruct.cpp -c -o $@


##  ZLIB Library  ##

adler32.o:
	$(CC) $(CFLAGS) ZLIB/adler32.c -c -o $@

compress.o:
	$(CC) $(CFLAGS) ZLIB/compress.c -c -o $@

crc32.o:
	$(CC) $(CFLAGS) ZLIB/crc32.c -c -o $@

deflate.o:
	$(CC) $(CFLAGS) ZLIB/deflate.c -c -o $@

infback.o:
	$(CC) $(CFLAGS) ZLIB/infback.c -c -o $@

inffast.o:
	$(CC) $(CFLAGS) ZLIB/inffast.c -c -o $@

inflate.o:
	$(CC) $(CFLAGS) ZLIB/inflate.c -c -o $@

inftrees.o:
	$(CC) $(CFLAGS) ZLIB/inftrees.c -c -o $@

trees.o:
	$(CC) $(CFLAGS) ZLIB/trees.c -c -o $@

uncompr.o:
	$(CC) $(CFLAGS) ZLIB/uncompr.c -c -o $@

zutil.o:
	$(CC) $(CFLAGS) ZLIB/zutil.c -c -o $@


##  LZ4 Library  ##

lz4.o:
	$(CC) $(CFLAGS) LZ4/lz4.c -c -o $@

lz4hc.o:
	$(CC) $(CFLAGS) LZ4/lz4hc.c -c -o $@

lz4frame.o:
	$(CC) $(CFLAGS) LZ4/lz4frame.c -c -o $@

xxhash.o:
	$(CC) $(CFLAGS) LZ4/xxhash.c -c -o $@
