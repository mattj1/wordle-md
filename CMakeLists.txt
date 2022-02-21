#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")

project("Wordle-MD")

add_executable(out src/main.c src/string.c src/joy.c src/vdp.c)
target_include_directories(out PUBLIC "inc")
target_include_directories(out PUBLIC "/Users/mattj/mars/m68k-elf/lib/gcc/m68k-elf/10.3.0/include")
