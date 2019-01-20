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

# CC
ifeq ($(UNAME_S),Darwin)
	CC := clang++
	LIBTOOL := libtool
else
	CC := g++
	LIBTOOL := libtool
endif

# Folders
INCDIR := SQLBridge/include
SRCDIR := SQLBridge/src
BUILDDIR := build
TARGETDIR := lib

# Targets
ifdef ARCH
	TARGET := $(TARGETDIR)/sqlcppbridge-$(TOS)-$(ARCH).a
else
	TARGET := $(TARGETDIR)/libsqlcppbridge.a
endif

# Code Lists
# Normal CPP files
SRCEXT := cpp
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst %.o,build/%.o,$(notdir $(patsubst $(SRCDIR)/%.cpp,%.o,$(SOURCES))))

# Shared Compiler Flags
CFLAGS := -c
INC := -I $(INCDIR)

ifdef ARCH
	CFLAGS += -arch $(ARCH) -isysroot $(DEVPATH)
	ifeq ($(TOS),ios)
		CFLAGS += -mios-version-min=10.0
	endif
	ifeq ($(TOS),osx)
		CFLAGS += -mmacosx-version-min=10.7
	endif
endif

# Platform Specific Compiler Flags
ifeq ($(UNAME_S),Linux)
	CFLAGS += -std=gnu++14 -O2
	LINK_CMD := ar -cr $(TARGET) $(OBJECTS)
else
	CFLAGS += -std=c++14 -stdlib=libc++ -O2 -fembed-bitcode
	LDFLAGS := -static
	LINK_CMD :=  $(LIBTOOL) $(LDFLAGS) -o $(TARGET) $(OBJECTS)
endif

# Linking
$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking..."
	@echo "  Linking $(TARGET)..."; $(LINK_CMD)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	@echo "  Cleaning $(TARGET)..."; $(RM) -r $(OBJECTS) $(TARGET)

install:
	@echo "Installing ...";
	@sudo cp $(TARGET) /usr/lib/;
	@sudo mkdir /usr/include/sqlbridge
	@sudo cp $(INCDIR)/* /usr/include/sqlbridge/
	

.PHONY: clean install

