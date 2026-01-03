#include "shell.hpp"
#include "stdio.h"

Shell::Shell() {
    this->command_count = 0;
}

void Shell::init() {
  printf("Shell Initialized (C++ Object)!\n");
}

void Shell::update() {
    // Just show we are alive for now
    this->command_count++;
    // kprintf("Shell Tick: %d\n", this->command_count);
}