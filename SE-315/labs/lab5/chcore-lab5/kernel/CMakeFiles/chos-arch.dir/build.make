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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /chos

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /chos

# Include any dependencies generated for this target.
include kernel/CMakeFiles/chos-arch.dir/depend.make

# Include the progress variables for this target.
include kernel/CMakeFiles/chos-arch.dir/progress.make

# Include the compile flags for this target's objects.
include kernel/CMakeFiles/chos-arch.dir/flags.make

kernel/CMakeFiles/chos-arch.dir/head.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/head.S.o: kernel/head.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object kernel/CMakeFiles/chos-arch.dir/head.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/head.S.o -c /chos/kernel/head.S

kernel/CMakeFiles/chos-arch.dir/main.c.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/main.c.o: kernel/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object kernel/CMakeFiles/chos-arch.dir/main.c.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/chos-arch.dir/main.c.o   -c /chos/kernel/main.c

kernel/CMakeFiles/chos-arch.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/chos-arch.dir/main.c.i"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /chos/kernel/main.c > CMakeFiles/chos-arch.dir/main.c.i

kernel/CMakeFiles/chos-arch.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/chos-arch.dir/main.c.s"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /chos/kernel/main.c -o CMakeFiles/chos-arch.dir/main.c.s

kernel/CMakeFiles/chos-arch.dir/monitor.c.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/monitor.c.o: kernel/monitor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object kernel/CMakeFiles/chos-arch.dir/monitor.c.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/chos-arch.dir/monitor.c.o   -c /chos/kernel/monitor.c

kernel/CMakeFiles/chos-arch.dir/monitor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/chos-arch.dir/monitor.c.i"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /chos/kernel/monitor.c > CMakeFiles/chos-arch.dir/monitor.c.i

kernel/CMakeFiles/chos-arch.dir/monitor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/chos-arch.dir/monitor.c.s"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /chos/kernel/monitor.c -o CMakeFiles/chos-arch.dir/monitor.c.s

kernel/CMakeFiles/chos-arch.dir/common/tools.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/common/tools.S.o: kernel/common/tools.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building ASM object kernel/CMakeFiles/chos-arch.dir/common/tools.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/common/tools.S.o -c /chos/kernel/common/tools.S

kernel/CMakeFiles/chos-arch.dir/common/uart.c.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/common/uart.c.o: kernel/common/uart.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object kernel/CMakeFiles/chos-arch.dir/common/uart.c.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/chos-arch.dir/common/uart.c.o   -c /chos/kernel/common/uart.c

kernel/CMakeFiles/chos-arch.dir/common/uart.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/chos-arch.dir/common/uart.c.i"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /chos/kernel/common/uart.c > CMakeFiles/chos-arch.dir/common/uart.c.i

kernel/CMakeFiles/chos-arch.dir/common/uart.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/chos-arch.dir/common/uart.c.s"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /chos/kernel/common/uart.c -o CMakeFiles/chos-arch.dir/common/uart.c.s

kernel/CMakeFiles/chos-arch.dir/common/printk.c.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/common/printk.c.o: kernel/common/printk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object kernel/CMakeFiles/chos-arch.dir/common/printk.c.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/chos-arch.dir/common/printk.c.o   -c /chos/kernel/common/printk.c

kernel/CMakeFiles/chos-arch.dir/common/printk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/chos-arch.dir/common/printk.c.i"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /chos/kernel/common/printk.c > CMakeFiles/chos-arch.dir/common/printk.c.i

kernel/CMakeFiles/chos-arch.dir/common/printk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/chos-arch.dir/common/printk.c.s"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /chos/kernel/common/printk.c -o CMakeFiles/chos-arch.dir/common/printk.c.s

kernel/CMakeFiles/chos-arch.dir/mm/page_table.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/mm/page_table.S.o: kernel/mm/page_table.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building ASM object kernel/CMakeFiles/chos-arch.dir/mm/page_table.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/mm/page_table.S.o -c /chos/kernel/mm/page_table.S

kernel/CMakeFiles/chos-arch.dir/mm/memcpy.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/mm/memcpy.S.o: kernel/mm/memcpy.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building ASM object kernel/CMakeFiles/chos-arch.dir/mm/memcpy.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/mm/memcpy.S.o -c /chos/kernel/mm/memcpy.S

kernel/CMakeFiles/chos-arch.dir/mm/memmove.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/mm/memmove.S.o: kernel/mm/memmove.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building ASM object kernel/CMakeFiles/chos-arch.dir/mm/memmove.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/mm/memmove.S.o -c /chos/kernel/mm/memmove.S

kernel/CMakeFiles/chos-arch.dir/mm/memset.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/mm/memset.S.o: kernel/mm/memset.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building ASM object kernel/CMakeFiles/chos-arch.dir/mm/memset.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/mm/memset.S.o -c /chos/kernel/mm/memset.S

kernel/CMakeFiles/chos-arch.dir/mm/bzero.S.o: kernel/CMakeFiles/chos-arch.dir/flags.make
kernel/CMakeFiles/chos-arch.dir/mm/bzero.S.o: kernel/mm/bzero.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/chos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building ASM object kernel/CMakeFiles/chos-arch.dir/mm/bzero.S.o"
	cd /chos/kernel && /usr/bin/aarch64-linux-gnu-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/chos-arch.dir/mm/bzero.S.o -c /chos/kernel/mm/bzero.S

chos-arch: kernel/CMakeFiles/chos-arch.dir/head.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/main.c.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/monitor.c.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/common/tools.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/common/uart.c.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/common/printk.c.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/mm/page_table.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/mm/memcpy.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/mm/memmove.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/mm/memset.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/mm/bzero.S.o
chos-arch: kernel/CMakeFiles/chos-arch.dir/build.make

.PHONY : chos-arch

# Rule to build all files generated by this target.
kernel/CMakeFiles/chos-arch.dir/build: chos-arch

.PHONY : kernel/CMakeFiles/chos-arch.dir/build

kernel/CMakeFiles/chos-arch.dir/clean:
	cd /chos/kernel && $(CMAKE_COMMAND) -P CMakeFiles/chos-arch.dir/cmake_clean.cmake
.PHONY : kernel/CMakeFiles/chos-arch.dir/clean

kernel/CMakeFiles/chos-arch.dir/depend:
	cd /chos && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /chos /chos/kernel /chos /chos/kernel /chos/kernel/CMakeFiles/chos-arch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : kernel/CMakeFiles/chos-arch.dir/depend
