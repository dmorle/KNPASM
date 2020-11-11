# CMakeList.txt : Top-level CMake project file
#
cmake_minimum_required (VERSION 3.8)

project ("knpasm")

include_directories ("include")

add_executable(
	knpasm
	"src/main.c"
	"src/knputil.c"
	"src/knpexpr.c"
	"src/knpassembler.c"
	"src/knpcpu.c"
	
	"include/knpasm/knpcore.h"
	"include/knpasm/knputil.h"
	"include/knpasm/knpexpr.h"
)
