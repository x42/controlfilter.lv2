#!/usr/bin/make -f

OPTIMIZATIONS ?= -msse -msse2 -mfpmath=sse -ffast-math -fomit-frame-pointer -O3 -fno-finite-math-only
PREFIX ?= /usr/local
CFLAGS ?= $(OPTIMIZATIONS) -Wall

PKG_CONFIG?=pkg-config
STRIP?=strip
STRIPFLAGS?=-s

controlfilter_VERSION?=$(shell git describe --tags HEAD 2>/dev/null | sed 's/-g.*$$//;s/^v//' || echo "LV2")
###############################################################################

LV2DIR ?= $(PREFIX)/lib/lv2
LOADLIBES=-lm
LV2NAME=controlfilter
BUNDLE=controlfilter.lv2
BUILDDIR=build/
targets=

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
  LV2LDFLAGS=-dynamiclib
  LIB_EXT=.dylib
  STRIPFLAGS=-u -r -arch all -s lv2syms
  targets+=lv2syms
  EXTENDED_RE=-E
else
  LV2LDFLAGS=-Wl,-Bstatic -Wl,-Bdynamic
  LIB_EXT=.so
  EXTENDED_RE=-r
endif

ifneq ($(XWIN),)
  CC=$(XWIN)-gcc
  STRIP=$(XWIN)-strip
  LV2LDFLAGS=-Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed
  LIB_EXT=.dll
  override LDFLAGS += -static-libgcc -static-libstdc++
endif

targets+=$(BUILDDIR)$(LV2NAME)$(LIB_EXT)

###############################################################################
# extract versions
LV2VERSION=$(controlfilter_VERSION)
include git2lv2.mk

###############################################################################
# check for build-dependencies
ifeq ($(shell $(PKG_CONFIG) --exists lv2 || echo no), no)
  $(error "LV2 SDK was not found")
endif

ifeq ($(XWIN),)
override CFLAGS += -fPIC -fvisibility=hidden
endif
override CFLAGS += -std=c99 `$(PKG_CONFIG) --cflags lv2` -DVERSION="\"$(controlfilter_VERSION)\""

###############################################################################
# build target definitions
default: all

all: $(BUILDDIR)manifest.ttl $(BUILDDIR)presets.ttl $(BUILDDIR)$(LV2NAME).ttl $(targets)

FILTERS := $(wildcard filters/*.c)

lv2syms:
	echo "_lv2_descriptor" > lv2syms

filters.c: $(FILTERS)
	echo "#include \"src/ttl.h\"" > filters.c
	i=0; for file in $(FILTERS); do \
		echo "/* --- */" >> filters.c; \
		echo "#define CSC_FILTER(FNX) CSC_FLT($$i, FNX)" >> filters.c; \
		echo "#define INIT_FN(NAME) XINIT_FN(NAME)" >> filters.c; \
		echo "#define PROC_FN(NAME) XPROC_FN(NAME)" >> filters.c; \
		echo "#include \"$${file}\"" >> filters.c; \
		echo "CSC_FILTER(CSC_NAME)" >> filters.c; \
		echo "#undef CSC_FILTER" >> filters.c; \
		echo "#undef INIT_FN" >> filters.c; \
		echo "#undef PROC_FN" >> filters.c; \
		echo "#undef CSC_NAME" >> filters.c; \
		i=`expr $$i + 1`; \
		done;
	echo "/* --- */" >> filters.c; \
	echo "#define LOOP_DESC(FN) \\" >> filters.c;
	i=0; for file in $(FILTERS); do \
		echo "FN($$i) \\" >> filters.c; \
		i=`expr $$i + 1`; \
		done;
	echo >> filters.c;

$(BUILDDIR)manifest.ttl: lv2ttl/manifest.ttl.in src/ttl.h filters.c Makefile lv2ttl/presets.ttl.in presets/*.ttl
	@mkdir -p $(BUILDDIR)
	cat lv2ttl/manifest.ttl.in > $(BUILDDIR)manifest.ttl
	$(CC) -E -I. -DCSC_MANIFEST filters.c \
		| grep -v '^\#' \
		| sed "s/HTTPP/http:\//g;s/HASH/#/g;s/@LV2NAME@/$(LV2NAME)/g;s/@LIB_EXT@/$(LIB_EXT)/g" \
		| uniq \
		>> $(BUILDDIR)manifest.ttl
	for file in presets/*.ttl; do head -n 3 $$file >> $(BUILDDIR)manifest.ttl; echo "rdfs:seeAlso <presets.ttl> ." >> $(BUILDDIR)manifest.ttl; done

$(BUILDDIR)presets.ttl: lv2ttl/presets.ttl.in presets/*.ttl
	@mkdir -p $(BUILDDIR)
	cat lv2ttl/presets.ttl.in > $(BUILDDIR)presets.ttl
	cat presets/*.ttl >> $(BUILDDIR)presets.ttl

$(BUILDDIR)$(LV2NAME).ttl: lv2ttl/$(LV2NAME).ttl.in src/ttl.h filters.c
	@mkdir -p $(BUILDDIR)
	cat lv2ttl/$(LV2NAME).ttl.in > $(BUILDDIR)$(LV2NAME).ttl
	$(CC) -E -I. -DCSC_TTF filters.c \
		| grep -v '^\#' \
		| sed 's/HTTPP/http:\//g;s/@VERSION@/lv2:microVersion $(LV2MIC) ;lv2:minorVersion $(LV2MIN) ;/g' \
		| uniq \
		>> $(BUILDDIR)$(LV2NAME).ttl

$(BUILDDIR)$(LV2NAME)$(LIB_EXT): src/$(LV2NAME).c filters.c
	@mkdir -p $(BUILDDIR)
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) \
	  -o $(BUILDDIR)$(LV2NAME)$(LIB_EXT) src/$(LV2NAME).c \
	  -shared $(LV2LDFLAGS) $(LDFLAGS) $(LOADLIBES)
	$(STRIP) $(STRIPFLAGS) $(BUILDDIR)$(LV2NAME)$(LIB_EXT)

###############################################################################
# install/uninstall/clean target definitions

install: all
	install -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m755 $(BUILDDIR)$(LV2NAME)$(LIB_EXT) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m644 $(BUILDDIR)manifest.ttl $(BUILDDIR)$(LV2NAME).ttl $(BUILDDIR)presets.ttl $(DESTDIR)$(LV2DIR)/$(BUNDLE)

uninstall:
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/manifest.ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/presets.ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME).ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME)$(LIB_EXT)
	-rmdir $(DESTDIR)$(LV2DIR)/$(BUNDLE)

clean:
	rm -f $(BUILDDIR)manifest.ttl $(BUILDDIR)presets.ttl $(BUILDDIR)$(LV2NAME).ttl $(BUILDDIR)$(LV2NAME)$(LIB_EXT) lv2syms filters.c
	-test -d $(BUILDDIR) && rmdir $(BUILDDIR) || true

.PHONY: clean all install uninstall
