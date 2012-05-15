NAME = smolar

include makefile.in
include makefile.header

OUTDIR = bin
STORE = .build
DOCS = docs

CORE 		:= core
MULTIGRID	:= multigrid
TIMING		:= timing
SMOLAR		:= smolar
GENERATORS	:= generators
DATA2VDF	:= data2vdf

DIRS 		:= $(CORE) $(MULTIGRID) $(TIMING) $(GENERATORS) $(DATA2VDF)
STATIC_LIBS 	:= $(DIRS)
STATIC 		:= $(addsuffix .a, $(STATIC_LIBS))
SHARED 		:= $(addsuffix .so, $(SMOLAR))

TESTSROOT	:= tests
RING2D		:= ring2d
MRING2D		:= mring2d
RING3D		:= ring3d
MRING3D		:= mring3d

TESTS 		:= $(RING2D) $(MRING2D) $(RING3D) $(MRING3D)

ifeq ($(HAVE_SINGLE), 1)
PRECISION = SINGLE
endif

ifeq ($(HAVE_DOUBLE), 1)
PRECISION = DOUBLE
endif

ifeq ($(HAVE_INTEGER), 1)
PRECISION = INTEGER
endif

DEFINES += -D$(PRECISION) -D_GNU_SOURCE

all: $(STATIC) $(SHARED)
tests: $(TESTS)
apps: $(APPS)
doxy: dirs
	$(DOXYGEN) $(NAME).doxy
	make -C $(DOCS)/latex

###################################################################################################
#
# 	CORE - THE SMOLAR NUMERICAL SCHEME
#
###################################################################################################

CORE_SOURCES := $(wildcard $(CORE)/src/*.c)
CORE_INCLUDES := -I$(CORE)/include/ -I$(MULTIGRID)/include/

###################################################################################################
#
# 	MULTIGRID - THE GRID DECOMPOSITION ENGINE
#
###################################################################################################

MULTIGRID_SOURCES := $(wildcard $(MULTIGRID)/src/*.c)
MULTIGRID_INCLUDES := -I$(MULTIGRID)/include/

###################################################################################################
#
# 	TIMING - THE HIGH RESOLUTION TIMER
#
###################################################################################################

TIMING_SOURCES := $(wildcard $(TIMING)/*.c)
TIMING_INCLUDES :=

###################################################################################################
#
# 	GENERATORS - THE TEST DATA GENERATORS
#
###################################################################################################

GENERATORS_SOURCES := $(wildcard $(GENERATORS)/src/*.c)
GENERATORS_INCLUDES := -I$(GENERATORS)/include

ifeq ($(TOOLCHAIN), gnu)
GENERATORS_DEPLIBS := -lm
endif

###################################################################################################
#
# 	DATA2VDF - convert binary data to UCAR Vapor compatible format
#
###################################################################################################

DATA2VDF_SOURCES := $(wildcard $(DATA2VDF)/*.cpp)
DATA2VDF_INCLUDES := -I$(DATA2VDF)

DATA2VDF_DEPLIBS := -L/opt/vapor/lib -lvdf -lnetcdf -lexpat -lcommon -lproj

###################################################################################################
#
# 	SMOLAR SHARED LIBRARY
#
###################################################################################################

SMOLAR_EMBED := -l$(CORE) -l$(MULTIGRID) -l$(TIMING)
SMOLAR_DEPLIBS := -lrt

###################################################################################################
#
# 	SMOLAR TESTS
#
###################################################################################################

TEST_INCLUDES := -I$(GENERATORS)/include
TEST_DEPLIBS := -L$(OUTDIR) -lsmolar -lgenerators -ldata2vdf $(DATA2VDF_DEPLIBS)

include makefile.footer

