#===- ./Makefile -------------------------------------------*- Makefile -*--===#
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#

LEVEL := .

# Top-Level LLVM Build Stages:
#   1. Build lib/System and lib/Support, which are used by utils (tblgen).
#   2. Build utils, which is used by VMCore.
#   3. Build VMCore, which builds the Intrinsics.inc file used by libs.
#   4. Build libs, which are needed by llvm-config.
#   5. Build llvm-config, which determines inter-lib dependencies for tools.
#   6. Build tools, runtime, docs.
#
# When cross-compiling, there are some things (tablegen) that need to
# be build for the build system first.

# If "RC_ProjectName" exists in the environment, and its value is
# "llvmCore", then this is an "Apple-style" build; search for
# "Apple-style" in the comments for more info.  Anything else is a
# normal build.
ifneq ($(findstring llvmCore, $(RC_ProjectName)),llvmCore)  # Normal build (not "Apple-style").

ifeq ($(BUILD_DIRS_ONLY),1)
  DIRS := lib/System lib/Support utils
  OPTIONAL_DIRS :=
else
  DIRS := lib/System lib/Support utils lib/VMCore lib tools/llvm-shlib \
          tools/llvm-config tools runtime docs unittests
  OPTIONAL_DIRS := projects bindings
endif

ifeq ($(BUILD_EXAMPLES),1)
  OPTIONAL_DIRS += examples
endif

EXTRA_DIST := test unittests llvm.spec include win32 Xcode

include $(LEVEL)/Makefile.config

ifneq ($(ENABLE_SHARED),1)
  DIRS := $(filter-out tools/llvm-shlib, $(DIRS))
endif

ifeq ($(MAKECMDGOALS),libs-only)
  DIRS := $(filter-out tools runtime docs, $(DIRS))
  OPTIONAL_DIRS :=
endif

ifeq ($(MAKECMDGOALS),install-libs)
  DIRS := $(filter-out tools runtime docs, $(DIRS))
  OPTIONAL_DIRS := $(filter bindings, $(OPTIONAL_DIRS))
endif

ifeq ($(MAKECMDGOALS),tools-only)
  DIRS := $(filter-out runtime docs, $(DIRS))
  OPTIONAL_DIRS :=
endif

ifeq ($(MAKECMDGOALS),install-clang)
  DIRS := tools/clang/tools/driver tools/clang/lib/Headers \
          tools/clang/lib/Runtime tools/clang/docs
  OPTIONAL_DIRS :=
  NO_INSTALL = 1
endif

ifeq ($(MAKECMDGOALS),install-clambc)
  DIRS := projects/clamdriver tools/clang/lib/Headers
  OPTIONAL_DIRS :=
  NO_INSTALL = 1
endif

ifeq ($(MAKECMDGOALS),clang-only)
  DIRS := $(filter-out tools runtime docs unittests, $(DIRS)) tools/clang
  OPTIONAL_DIRS :=
endif

ifeq ($(MAKECMDGOALS),clambc-only)
  DIRS := lib/System lib/Support utils lib/VMCore lib/Bitcode lib/Analysis \
          lib/Transforms lib/Target lib/MC tools/llvm-config\
	  tools/clang/include tools/clang/lib\
	  projects/clamdriver projects/ifacegen
  OPTIONAL_DIRS :=
endif

ifeq ($(MAKECMDGOALS),unittests)
  DIRS := $(filter-out tools runtime docs, $(DIRS)) utils unittests
  OPTIONAL_DIRS :=
endif

# Use NO_INSTALL define of the Makefile of each directory for deciding
# if the directory is installed or not
ifeq ($(MAKECMDGOALS),install)
  OPTIONAL_DIRS := $(filter bindings, $(OPTIONAL_DIRS))
endif

# If we're cross-compiling, build the build-hosted tools first
ifeq ($(LLVM_CROSS_COMPILING),1)
all:: cross-compile-build-tools

clean::
	$(Verb) rm -rf BuildTools

cross-compile-build-tools:
	$(Verb) if [ ! -f BuildTools/Makefile ]; then \
          $(MKDIR) BuildTools; \
	  cd BuildTools ; \
	  unset CFLAGS ; \
	  unset CXXFLAGS ; \
	  $(PROJ_SRC_DIR)/configure --build=$(BUILD_TRIPLE) \
		--host=$(BUILD_TRIPLE) --target=$(BUILD_TRIPLE); \
	  cd .. ; \
	fi; \
        ($(MAKE) -C BuildTools \
	  BUILD_DIRS_ONLY=1 \
	  UNIVERSAL= \
	  ENABLE_OPTIMIZED=$(ENABLE_OPTIMIZED) \
	  ENABLE_PROFILING=$(ENABLE_PROFILING) \
	  ENABLE_COVERAGE=$(ENABLE_COVERAGE) \
	  DISABLE_ASSERTIONS=$(DISABLE_ASSERTIONS) \
	  ENABLE_EXPENSIVE_CHECKS=$(ENABLE_EXPENSIVE_CHECKS) \
	) || exit 1;
endif

# Include the main makefile machinery.
include $(LLVM_SRC_ROOT)/Makefile.rules

# Specify options to pass to configure script when we're
# running the dist-check target
DIST_CHECK_CONFIG_OPTIONS = --with-llvmgccdir=$(LLVMGCCDIR)

.PHONY: debug-opt-prof
debug-opt-prof:
	$(Echo) Building Debug Version
	$(Verb) $(MAKE)
	$(Echo)
	$(Echo) Building Optimized Version
	$(Echo)
	$(Verb) $(MAKE) ENABLE_OPTIMIZED=1
	$(Echo)
	$(Echo) Building Profiling Version
	$(Echo)
	$(Verb) $(MAKE) ENABLE_PROFILING=1

dist-hook::
	$(Echo) Eliminating files constructed by configure
	$(Verb) $(RM) -f \
	  $(TopDistDir)/include/llvm/Config/config.h  \
	  $(TopDistDir)/include/llvm/System/DataTypes.h

ifneq ($(TARGETS_TO_BUILD),ClamBC)
clambc-only: 
	$(MAKE) clambc-only TARGETS_TO_BUILD=ClamBC
else
clambc-only: all
endif

clang-only: all
tools-only: all
libs-only: all
install-clang: install
install-libs: install
install-clambc: clambc-only install

#------------------------------------------------------------------------
# Make sure the generated headers are up-to-date. This must be kept in
# sync with the AC_CONFIG_HEADER invocations in autoconf/configure.ac
#------------------------------------------------------------------------
FilesToConfig := \
  include/llvm/Config/config.h \
  include/llvm/Config/Targets.def \
  include/llvm/Config/AsmPrinters.def \
  include/llvm/Config/AsmParsers.def \
  include/llvm/Config/Disassemblers.def \
  include/llvm/System/DataTypes.h \
  tools/llvmc/plugins/Base/Base.td
FilesToConfigPATH  := $(addprefix $(LLVM_OBJ_ROOT)/,$(FilesToConfig))

all-local:: $(FilesToConfigPATH)
$(FilesToConfigPATH) : $(LLVM_OBJ_ROOT)/% : $(LLVM_SRC_ROOT)/%.in
	$(Echo) Regenerating $*
	$(Verb) cd $(LLVM_OBJ_ROOT) && $(ConfigStatusScript) $*
.PRECIOUS: $(FilesToConfigPATH)

# NOTE: This needs to remain as the last target definition in this file so
# that it gets executed last.
ifneq ($(BUILD_DIRS_ONLY),1)
all::
	$(Echo) '*****' Completed $(BuildMode)$(AssertMode) Build
ifeq ($(BuildMode),Debug)
	$(Echo) '*****' Note: Debug build can be 10 times slower than an
	$(Echo) '*****' optimized build. Use 'make ENABLE_OPTIMIZED=1' to
	$(Echo) '*****' make an optimized build. Alternatively you can
	$(Echo) '*****' configure with --enable-optimized.
endif
endif

check-llvm2cpp:
	$(Verb)$(MAKE) check TESTSUITE=Feature RUNLLVM2CPP=1

check-one:
	$(Verb)$(MAKE) -C test check-one TESTONE=$(TESTONE)

srpm: $(LLVM_OBJ_ROOT)/llvm.spec
	rpmbuild -bs $(LLVM_OBJ_ROOT)/llvm.spec

rpm: $(LLVM_OBJ_ROOT)/llvm.spec
	rpmbuild -bb --target $(TARGET_TRIPLE) $(LLVM_OBJ_ROOT)/llvm.spec

show-footprint:
	$(Verb) du -sk $(LibDir)
	$(Verb) du -sk $(ToolDir)
	$(Verb) du -sk $(ExmplDir)
	$(Verb) du -sk $(ObjDir)

build-for-llvm-top:
	$(Verb) if test ! -f ./config.status ; then \
	  ./configure --prefix="$(LLVM_TOP)/install" \
	    --with-llvm-gcc="$(LLVM_TOP)/llvm-gcc" ; \
	fi
	$(Verb) $(MAKE) tools-only

SVN = svn
SVN-UPDATE-OPTIONS =
AWK = awk
SUB-SVN-DIRS = $(AWK) '/\?\ \ \ \ \ \ / {print $$2}'   \
		| LC_ALL=C xargs $(SVN) info 2>/dev/null \
		| $(AWK) '/Path:\ / {print $$2}'

update:
	$(SVN) $(SVN-UPDATE-OPTIONS) update $(LLVM_SRC_ROOT)
	@ $(SVN) status $(LLVM_SRC_ROOT) | $(SUB-SVN-DIRS) | xargs $(SVN) $(SVN-UPDATE-OPTIONS) update

happiness: update all check unittests

.PHONY: srpm rpm update happiness

# declare all targets at this level to be serial:

.NOTPARALLEL:

else # Building "Apple-style."
# In an Apple-style build, once configuration is done, lines marked
# "Apple-style" are removed with sed!  Please don't remove these!
# Look for the string "Apple-style" in utils/buildit/build_llvm.
include $(shell find . -name GNUmakefile) # Building "Apple-style."
endif # Building "Apple-style."
