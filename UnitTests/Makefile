#
#  Makefile
#  SQLCPPBridge
#
#  Created by Roman Makhnenko on 23/06/2018
#  Copyright © 2018, DataArt.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#      * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#      * Neither the name of the DataArt nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL DataArt BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Determine the platform
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin) 
	OSNAME := MacOS
	ifeq ($(DEVPATH),)
		DEVPATH=$(shell xcode-select --print-path)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
	endif
endif
ifeq ($(UNAME_S), Linux)
	OSNAME := Linux
endif
ifneq (,$(findstring MINGW64,$(UNAME_S)))
	OSNAME := MinGW64
endif

# CC
ifeq ($(UNAME_S),Darwin)
	CC := clang++
	LIBTOOL := libtool
else
	CC := g++
	LIBTOOL := libtool
endif

# Folders
INCDIR := include
SRCDIR := src
INCDIR_SB := ../SQLBridge/include
SRCDIR_SB := ../SQLBridge/src
INCDIR_GTEST := Lib/$(OSNAME)/googletest/include
INCDIR_CASES := ../Cases
SRCDIR_CASES := ../Cases

BUILDDIR := build
TARGETDIR := out

# Targets
TARGET := $(TARGETDIR)/Tests

# Code Lists
# Normal CPP files
SRCEXT := cpp
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES_CASES := $(wildcard $(SRCDIR_CASES)/*.$(SRCEXT))
SOURCES_SB := $(wildcard $(SRCDIR_SB)/*.$(SRCEXT))
OBJECTS := $(patsubst %.o,build/%.o,$(notdir $(patsubst $(SRCDIR)/%.cpp,%.o,$(SOURCES))))
OBJECTS += $(patsubst %.o,build/%.o,$(notdir $(patsubst $(SRCDIR_CASES)/%.$(SRCEXT),%.o,$(SOURCES_CASES))))
OBJECTS += $(patsubst %.o,build/%.o,$(notdir $(patsubst $(SRCDIR_SB)/%.$(SRCEXT),%.o,$(SOURCES_SB))))


# Platform Specific Compiler Flags
CFLAGS := -c
LIB := -lsqlite3 -lgtest
LDFLAGS := -LLib/$(OSNAME)/googletest/lib
ifeq ($(UNAME_S), Darwin)
	INC := -I $(INCDIR) -I $(INCDIR_SB) -I $(INCDIR_GTEST) -I $(INCDIR_CASES)
	CFLAGS += -std=c++14 -stdlib=libc++ -O2 -fPIC -mmacosx-version-min=10.7 -isysroot $(DEVPATH)
	LDFLAGS += -stdlib=libc++
else
	CFLAGS += -std=gnu++14 -O2 -fpermissive
	INC := -I $(INCDIR) -I $(INCDIR_SB) -I $(INCDIR_CASES) -I $(INCDIR_GTEST)
	LIB += -lpthread -lm
endif

# Linking
$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking..."
	@echo "  Linking $(TARGET)..."; $(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIB) 
	@echo "Running..."; $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -c -o $@ $<
#ifneq ($(UNAME_S), Linux)
$(BUILDDIR)/%.o: $(SRCDIR_SB)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -c -o $@ $<
#endif

$(BUILDDIR)/%.o: $(SRCDIR_CASES)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	@echo "  Cleaning $(TARGET)..."; $(RM) -r $(OBJECTS) $(TARGET)

.PHONY: clean
