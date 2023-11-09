#include "emulator.hpp"

#include <cstdio>

int main(i32 argc, char **argv) {
    if(argc < 2) {
        printf("No file provided.\n");
        return NO_FILE_PROVIDED;
    }

    emu::Emulator emulator;

    if(emulator.load_instructions(argv[1]) == SUCCESS) {
        emulator.execute();
    } else {
        printf("Poo\n");
    }

    return 0;
}