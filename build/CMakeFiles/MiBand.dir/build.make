# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_SOURCE_DIR = /home/user/MiBand

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/MiBand/build

# Include any dependencies generated for this target.
include CMakeFiles/MiBand.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MiBand.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MiBand.dir/flags.make

CMakeFiles/MiBand.dir/main.cpp.o: CMakeFiles/MiBand.dir/flags.make
CMakeFiles/MiBand.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/MiBand/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MiBand.dir/main.cpp.o"
	/opt/remi-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++ --sysroot=/opt/remi-sdk/sysroots/aarch64-poky-linux  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MiBand.dir/main.cpp.o -c /home/user/MiBand/main.cpp

CMakeFiles/MiBand.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MiBand.dir/main.cpp.i"
	/opt/remi-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++ --sysroot=/opt/remi-sdk/sysroots/aarch64-poky-linux $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/MiBand/main.cpp > CMakeFiles/MiBand.dir/main.cpp.i

CMakeFiles/MiBand.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MiBand.dir/main.cpp.s"
	/opt/remi-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++ --sysroot=/opt/remi-sdk/sysroots/aarch64-poky-linux $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/MiBand/main.cpp -o CMakeFiles/MiBand.dir/main.cpp.s

# Object files for target MiBand
MiBand_OBJECTS = \
"CMakeFiles/MiBand.dir/main.cpp.o"

# External object files for target MiBand
MiBand_EXTERNAL_OBJECTS =

MiBand: CMakeFiles/MiBand.dir/main.cpp.o
MiBand: CMakeFiles/MiBand.dir/build.make
MiBand: CMakeFiles/MiBand.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/MiBand/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MiBand"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MiBand.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MiBand.dir/build: MiBand

.PHONY : CMakeFiles/MiBand.dir/build

CMakeFiles/MiBand.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MiBand.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MiBand.dir/clean

CMakeFiles/MiBand.dir/depend:
	cd /home/user/MiBand/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/MiBand /home/user/MiBand /home/user/MiBand/build /home/user/MiBand/build /home/user/MiBand/build/CMakeFiles/MiBand.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MiBand.dir/depend

