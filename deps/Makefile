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
LDFLAGS =
LDLIBS = -lz -llzma


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
JL_SHARE = $(shell julia -e 'print(joinpath(JULIA_HOME,Base.DATAROOTDIR,"julia"))')
CFLAGS   += $(shell $(JL_SHARE)/julia-config.jl --cflags)
CXXFLAGS += $(shell $(JL_SHARE)/julia-config.jl --cflags)
LDFLAGS  += $(shell $(JL_SHARE)/julia-config.jl --ldflags)
LDLIBS   += $(shell $(JL_SHARE)/julia-config.jl --ldlibs)


## Define lib macro and include
LIBMACRO = -DCOREARRAY_USE_ZLIB_EXT -DCOREARRAY_USE_LZMA_EXT -DCOREARRAY_NO_LZ4
LIBINCLUDE = -Iinclude
CFLAGS   += $(LIBMACRO) $(LIBINCLUDE)
CXXFLAGS += $(LIBMACRO) $(LIBINCLUDE)


## CoreArray library archive file
LIB_COREARRAY_A = libCoreArray.a

## CoreArray library object files
LIB_COREOBJS = CoreArray.o dAllocator.o dAny.o dBase.o dBitGDS.o \
	dEndian.o dFile.o dParallel.o dParallel_Ext.o dPlatform.o dSerial.o \
	dStream.o dStruct.o dVLIntGDS.o


## Julia interface
LIB_J_OBJS = jugds.o JCoreArray.o


## all jobs
all: $(LIB_COREOBJS) $(LIB_J_OBJS)
	$(CXX) $(CXXFLAGS) -shared \
		$(LIB_COREOBJS) $(LIB_J_OBJS) \
		$(LDFLAGS) $(LINKLIBS) $(LDLIBS) -o libCoreArray.$(LIBEXT)


## Clean and remove files
clean:
	$(RM) *.o
	$(RM) libCoreArray.$(LIBEXT)




##########################################################################

##  Julia Interface  ##

jugds.o: jugds.cpp
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

dSerial.o:
	$(CXX) $(CXXFLAGS) CoreArray/dSerial.cpp -c -o $@

dStream.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStream.cpp -c -o $@

dStruct.o:
	$(CXX) $(CXXFLAGS) CoreArray/dStruct.cpp -c -o $@

dVLIntGDS.o:
	$(CXX) $(CXXFLAGS) CoreArray/dVLIntGDS.cpp -c -o $@
