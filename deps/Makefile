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
CC ?= gcc
## The command which runs the C++ compiler.
CXX ?= g++

## Debugging and optimization options for the C compiler.
CFLAGS = -O2
## Options for the C++ compiler.
CXXFLAGS = -O2

## Options for linker
LDFLAGS =
LDLIBS =


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


## julia program
CFLAGS   += $(JL_CFLAGS)
CXXFLAGS += $(JL_CXXFLAGS)
LDFLAGS  += $(JL_LDFLAGS)
LDLIBS   += $(JL_LDLIBS)


## Define lib macro and include
LIBMACRO =
LIBINCLUDE = -Iinclude
CFLAGS   += $(LIBMACRO) $(LIBINCLUDE)
CXXFLAGS += $(LIBMACRO) $(LIBINCLUDE)


## CoreArray library archive file
LIB_COREARRAY_A = libCoreArray.a

## CoreArray library object files
LIB_COREOBJS = CoreArray.o dAllocator.o dAny.o dBase.o dBitGDS.o \
	dEndian.o dFile.o dParallel.o dParallel_Ext.o dPlatform.o dRealGDS.o \
	dSerial.o dSparse.o dStrGDS.o dStream.o dStruct.o dVLIntGDS.o

## zlib
ZLIB_OBJS = adler32.o compress.o crc32.o deflate.o infback.o inffast.o \
	inflate.o inftrees.o trees.o uncompr.o zutil.o

## lz4
LZ4_OBJS = lz4.o lz4hc.o lz4frame.o xxhash.o

## Lib
LIBXZ = liblzma.a

## Julia interface
LIB_J_OBJS = jugds.o JCoreArray.o


## all jobs
all: $(LIB_COREOBJS) $(ZLIB_OBJS) $(LZ4_OBJS) $(LIBXZ) $(LIB_J_OBJS)
	$(CXX) $(CXXFLAGS) -shared \
		$(LIB_COREOBJS) $(ZLIB_OBJS) $(LZ4_OBJS) $(LIBXZ) $(LIB_J_OBJS) \
		$(LDFLAGS) $(LINKLIBS) $(LDLIBS) -o libCoreArray.$(LIBEXT)


## Clean and remove files
clean:
	$(RM) *.o
	$(RM) libCoreArray.$(LIBEXT)




##########################################################################

##  Julia Interface  ##

jugds.o: jugds.cpp
	echo "$(CXXFLAGS)"
	echo "$(JL_SHARE)"
	$(CXX) $(CXXFLAGS) -ICoreArray jugds.cpp -c -o $@

JCoreArray.o: JCoreArray.cpp
	$(CXX) $(CXXFLAGS) -ICoreArray JCoreArray.cpp -c -o $@


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

dRealGDS.o:
	$(CXX) $(CXXFLAGS) CoreArray/dRealGDS.cpp -c -o $@

dSerial.o:
	$(CXX) $(CXXFLAGS) CoreArray/dSerial.cpp -c -o $@

dSparse.o:
	$(CXX) $(CXXFLAGS) CoreArray/dSparse.cpp -c -o $@

dStrGDS.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStrGDS.cpp -c -o $@

dStream.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStream.cpp -c -o $@

dStruct.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStruct.cpp -c -o $@

dVLIntGDS.o:
	$(CXX) $(CXXFLAGS) CoreArray/dVLIntGDS.cpp -c -o $@


##  zlib  ##

adler32.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/adler32.c -c -o $@

compress.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/compress.c -c -o $@

crc32.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/crc32.c -c -o $@

deflate.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/deflate.c -c -o $@

infback.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/infback.c -c -o $@

inffast.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/inffast.c -c -o $@

inflate.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/inflate.c -c -o $@

inftrees.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/inftrees.c -c -o $@

trees.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/trees.c -c -o $@

uncompr.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/uncompr.c -c -o $@

zutil.o:
	$(CC) $(CFLAGS) -IZLIB ZLIB/zutil.c -c -o $@


##  lz4  ##

lz4.o:
	$(CC) $(CFLAGS) -ILZ4 LZ4/lz4.c -c -o $@

lz4hc.o:
	$(CC) $(CFLAGS) -ILZ4 LZ4/lz4hc.c -c -o $@

lz4frame.o:
	$(CC) $(CFLAGS) -ILZ4 LZ4/lz4frame.c -c -o $@

xxhash.o:
	$(CC) $(CFLAGS) -ILZ4 LZ4/xxhash.c -c -o $@
