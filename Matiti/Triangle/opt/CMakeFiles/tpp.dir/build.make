# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/banerjee/ParticleFracturing/Matiti/Triangle

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/banerjee/ParticleFracturing/Matiti/Triangle/opt

# Include any dependencies generated for this target.
include CMakeFiles/tpp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tpp.dir/flags.make

CMakeFiles/tpp.dir/src/main.cpp.o: CMakeFiles/tpp.dir/flags.make
CMakeFiles/tpp.dir/src/main.cpp.o: ../src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/banerjee/ParticleFracturing/Matiti/Triangle/opt/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/tpp.dir/src/main.cpp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/tpp.dir/src/main.cpp.o -c /home/banerjee/ParticleFracturing/Matiti/Triangle/src/main.cpp

CMakeFiles/tpp.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tpp.dir/src/main.cpp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/banerjee/ParticleFracturing/Matiti/Triangle/src/main.cpp > CMakeFiles/tpp.dir/src/main.cpp.i

CMakeFiles/tpp.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tpp.dir/src/main.cpp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/banerjee/ParticleFracturing/Matiti/Triangle/src/main.cpp -o CMakeFiles/tpp.dir/src/main.cpp.s

CMakeFiles/tpp.dir/src/main.cpp.o.requires:
.PHONY : CMakeFiles/tpp.dir/src/main.cpp.o.requires

CMakeFiles/tpp.dir/src/main.cpp.o.provides: CMakeFiles/tpp.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/tpp.dir/build.make CMakeFiles/tpp.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/tpp.dir/src/main.cpp.o.provides

CMakeFiles/tpp.dir/src/main.cpp.o.provides.build: CMakeFiles/tpp.dir/src/main.cpp.o

# Object files for target tpp
tpp_OBJECTS = \
"CMakeFiles/tpp.dir/src/main.cpp.o"

# External object files for target tpp
tpp_EXTERNAL_OBJECTS =

tpp: CMakeFiles/tpp.dir/src/main.cpp.o
tpp: CMakeFiles/tpp.dir/build.make
tpp: libTriangle++.so
tpp: CMakeFiles/tpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable tpp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tpp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tpp.dir/build: tpp
.PHONY : CMakeFiles/tpp.dir/build

CMakeFiles/tpp.dir/requires: CMakeFiles/tpp.dir/src/main.cpp.o.requires
.PHONY : CMakeFiles/tpp.dir/requires

CMakeFiles/tpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tpp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tpp.dir/clean

CMakeFiles/tpp.dir/depend:
	cd /home/banerjee/ParticleFracturing/Matiti/Triangle/opt && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/banerjee/ParticleFracturing/Matiti/Triangle /home/banerjee/ParticleFracturing/Matiti/Triangle /home/banerjee/ParticleFracturing/Matiti/Triangle/opt /home/banerjee/ParticleFracturing/Matiti/Triangle/opt /home/banerjee/ParticleFracturing/Matiti/Triangle/opt/CMakeFiles/tpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tpp.dir/depend

