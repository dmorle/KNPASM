# CMakeList.txt : Top-level CMake project file
#
cmake_minimum_required (VERSION 3.8)

project ("knpasm")

include_directories ("include")

add_executable(
	knpasm
	"src/knpasm.c"
	"src/tokenizing.c"
	"src/knpcore.c"
	
	"include/knpasm/knpcore.h"
	"include/knpasm/tokenizing.h"
 )
