#!/usr/bin/make -f

PREFIX ?= /usr/local
CFLAGS ?= -O3 -Wall
LIBDIR ?= lib

STRIP=strip
STRIPFLAGS=-s

cs_VERSION?=$(shell git describe --tags HEAD 2>/dev/null | sed 's/-g.*$$//;s/^v//' || echo "LV2")
###############################################################################
LV2DIR ?= $(PREFIX)/$(LIBDIR)/lv2
BUILDDIR=build/
LV2NAME=cs
LIB_EXT=.so

###############################################################################
# extract versions
LV2VERSION=$(cs_VERSION)
include git2lv2.mk


###############################################################################
# check for build-dependencies
ifeq ($(shell pkg-config --exists lv2 || echo no), no)
  $(error "LV2 SDK was not found")
endif

override CFLAGS += -fPIC -std=c99
override CFLAGS += `pkg-config --cflags lv2`

###############################################################################
# build target definitions
default: all

all: $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(BUILDDIR)$(LV2NAME)$(LIB_EXT)

$(BUILDDIR)manifest.ttl: manifest.ttl.in
	@mkdir -p $(BUILDDIR)
	sed "s/@LV2NAME@/$(LV2NAME)/;s/@LIB_EXT@/$(LIB_EXT)/" \
	  manifest.ttl.in > $(BUILDDIR)manifest.ttl

$(BUILDDIR)$(LV2NAME).ttl: $(LV2NAME).ttl.in
	@mkdir -p $(BUILDDIR)
	sed "s/@VERSION@/lv2:microVersion $(LV2MIC) ;lv2:minorVersion $(LV2MIN) ;/g" \
		$(LV2NAME).ttl.in > $(BUILDDIR)$(LV2NAME).ttl


$(BUILDDIR)$(LV2NAME)$(LIB_EXT): $(LV2NAME).c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	  -o $(BUILDDIR)$(LV2NAME)$(LIB_EXT) $(LV2NAME).c \
	  -shared $(LV2LDFLAGS) $(LDFLAGS) $(LOADLIBES)
	$(STRIP) $(STRIPFLAGS) $(BUILDDIR)$(LV2NAME)$(LIB_EXT)

###############################################################################
# install/uninstall/clean target definitions

install: all
	install -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m755 $(BUILDDIR)$(LV2NAME)$(LIB_EXT) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m644 $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(DESTDIR)$(LV2DIR)/$(BUNDLE)

uninstall:
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/manifest.ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME).ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME)$(LIB_EXT)
	-rmdir $(DESTDIR)$(LV2DIR)/$(BUNDLE)

clean:
	rm -f $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(BUILDDIR)$(LV2NAME)$(LIB_EXT) lv2syms
	-test -d $(BUILDDIR) && rmdir $(BUILDDIR) || true

.PHONY: clean all install uninstall
