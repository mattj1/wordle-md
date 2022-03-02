#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")

project("Wordle-MD")

add_executable(out src/main.c src/string.c src/res_generated.c src/res_tiles_generated.s src/anim.c src/common.h src/graphics.c src/anim_guess.c
       src/debug.S src/title.c src/menu.c src/instructions.c)
target_include_directories(out PUBLIC "/Users/matt/mars/m68k-elf/lib/gcc/m68k-elf/10.3.0/include")
target_include_directories(out PUBLIC "c:/msys64/home/Matt/mars/m68k-elf/lib/gcc/m68k-elf/10.3.0/include")
target_include_directories(out PUBLIC "c:\\msys64\\home\\Matt\\mars\\m68k-elf\\include")

