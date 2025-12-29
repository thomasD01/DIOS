#!/bin/bash

set -e

sudo apt install build-essential nasm mtools qemu-system-x86 libgmp3-dev libmpc-dev libmpfr-dev bison flex texinfo libisl-dev

make toolchain
