# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/greundzo/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.6817.18/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/greundzo/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.6817.18/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/greundzo/so1920

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/greundzo/so1920/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/so1920.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/so1920.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/so1920.dir/flags.make

CMakeFiles/so1920.dir/student.c.o: CMakeFiles/so1920.dir/flags.make
CMakeFiles/so1920.dir/student.c.o: ../student.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/greundzo/so1920/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/so1920.dir/student.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/so1920.dir/student.c.o   -c /home/greundzo/so1920/student.c

CMakeFiles/so1920.dir/student.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/so1920.dir/student.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/greundzo/so1920/student.c > CMakeFiles/so1920.dir/student.c.i

CMakeFiles/so1920.dir/student.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/so1920.dir/student.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/greundzo/so1920/student.c -o CMakeFiles/so1920.dir/student.c.s

# Object files for target so1920
so1920_OBJECTS = \
"CMakeFiles/so1920.dir/student.c.o"

# External object files for target so1920
so1920_EXTERNAL_OBJECTS =

so1920: CMakeFiles/so1920.dir/student.c.o
so1920: CMakeFiles/so1920.dir/build.make
so1920: CMakeFiles/so1920.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/greundzo/so1920/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable so1920"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/so1920.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/so1920.dir/build: so1920

.PHONY : CMakeFiles/so1920.dir/build

CMakeFiles/so1920.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/so1920.dir/cmake_clean.cmake
.PHONY : CMakeFiles/so1920.dir/clean

CMakeFiles/so1920.dir/depend:
	cd /home/greundzo/so1920/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/greundzo/so1920 /home/greundzo/so1920 /home/greundzo/so1920/cmake-build-debug /home/greundzo/so1920/cmake-build-debug /home/greundzo/so1920/cmake-build-debug/CMakeFiles/so1920.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/so1920.dir/depend

