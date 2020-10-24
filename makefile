MAKEFILE_BUILDER = make -f makefile_builder.mk --no-print-directory


all: example

CPP_FILES := example_part1.cpp example_part2.cpp example_part3.cpp

example:
	@$(MAKEFILE_BUILDER) CPP_TO_COMPILE="$(CPP_FILES) MemoryLeakLogger.cpp" EXE_TARGET="example" CC="gcc" CXX="g++" LL="g++"


clean:
	@$(MAKEFILE_BUILDER) clean
