cmake_minimum_required(VERSION 2.8)

project(smolar)

add_definitions(-DSINGLE)
#add_definitions(-DDOUBLE)
#add_definitions(-DINTEGER)
add_definitions(-D_GNU_SOURCE)

# Path where to search for VAPOR visualizer libraries
set(VAPOR_LIBS /opt/vapor/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math -O3 -mtune=native -std=c99 -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -O3 -mtune=native -std=c++11 -fopenmp")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -std=c99 -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 -fopenmp")

LINK_DIRECTORIES(${VAPOR_LIBS})

find_library(vdf REQUIRED)
find_library(common REQUIRED)
find_library(proj REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/core/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/data2vdf)
include_directories(${CMAKE_CURRENT_LIST_DIR}/generators/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/multigrid/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/timing)

add_library(core STATIC
	core/include/msmolar.h
	core/include/patterns.h
	core/include/pvelocity.h
	core/include/smolar_int.h
	core/include/smolar_pass.h
	core/include/uu.h
	core/include/vv.h
	core/include/ww.h
	core/src/msmolar.c
	core/src/msmolar_gather.c
	core/src/msmolar_scatter.c
	core/src/pvelocity.c
	core/src/smolar.c
	core/src/smolar_pass.c
	core/src/uu.c
	core/src/vv.c
	core/src/ww.c)

add_library(data2vdf STATIC
	data2vdf/raw2vdf.cpp
	data2vdf/vdfcreate.cpp)

add_library(generators STATIC
	generators/include/gengauss2d.h
	generators/include/gengauss3d.h
	generators/include/genring2d.h
	generators/include/genringf2d.h
	generators/src/gengauss2d.c
	generators/src/gengauss3d.c
	generators/src/genring2d.c
	generators/src/genringf2d.c)

add_library(multigrid STATIC
	multigrid/include/gather.h
	multigrid/include/grid.h
	multigrid/include/overhead.h
	multigrid/include/scatter.h
	multigrid/include/subcpy.h
	multigrid/makefile
	multigrid/makefile.in
	multigrid/src/gather.c
	multigrid/src/grid.c
	multigrid/src/overhead.c
	multigrid/src/scatter.c
	multigrid/src/subcpy.c)

add_library(timing STATIC
	timing/timing.c
	timing/timing.h)

add_executable(ring2d
	tests/init.c
	tests/init.h
	tests/ring2d/test.c)

target_link_libraries(ring2d
	core data2vdf generators multigrid timing
	vdf udunits2 netcdf expat common proj)

add_executable(ring3d
	tests/init.c
	tests/init.h
	tests/ring3d/test.c)

target_link_libraries(ring3d
	core data2vdf generators multigrid timing
	vdf udunits2 netcdf expat common proj)

configure_file(cbarm.gs cbarm.gs COPYONLY)

