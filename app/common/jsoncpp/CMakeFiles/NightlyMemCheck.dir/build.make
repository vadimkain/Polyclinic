# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mbukhta/Documents/Polyclinic

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mbukhta/Documents/Polyclinic/build

# Utility rule file for NightlyMemCheck.

# Include any custom commands dependencies for this target.
include common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/compiler_depend.make

# Include the progress variables for this target.
include common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/progress.make

common/jsoncpp/CMakeFiles/NightlyMemCheck:
	cd /home/mbukhta/Documents/Polyclinic/build/common/jsoncpp && /usr/bin/ctest -D NightlyMemCheck

NightlyMemCheck: common/jsoncpp/CMakeFiles/NightlyMemCheck
NightlyMemCheck: common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build.make
.PHONY : NightlyMemCheck

# Rule to build all files generated by this target.
common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build: NightlyMemCheck
.PHONY : common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build

common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/clean:
	cd /home/mbukhta/Documents/Polyclinic/build/common/jsoncpp && $(CMAKE_COMMAND) -P CMakeFiles/NightlyMemCheck.dir/cmake_clean.cmake
.PHONY : common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/clean

common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/depend:
	cd /home/mbukhta/Documents/Polyclinic/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mbukhta/Documents/Polyclinic /home/mbukhta/Documents/Polyclinic/common/jsoncpp /home/mbukhta/Documents/Polyclinic/build /home/mbukhta/Documents/Polyclinic/build/common/jsoncpp /home/mbukhta/Documents/Polyclinic/build/common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : common/jsoncpp/CMakeFiles/NightlyMemCheck.dir/depend

