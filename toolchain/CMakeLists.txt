cmake_minimum_required(VERSION 3.15)
project(LocalToolchain NONE)

include(ExternalProject)

# --- Configuration ---
set(GCC_VERSION "13.2.0")
set(BINUTILS_VERSION "2.41")
set(TARGET_ARCH "x86_64-elf")

set(INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/install")
set(PREFIX "${INSTALL_DIR}")
set(PATH_VAR "$ENV{PATH}:${PREFIX}/bin")

ExternalProject_Add(build_binutils
    URL https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/binutils
    CONFIGURE_COMMAND <SOURCE_DIR>/configure 
        --target=${TARGET_ARCH} 
        --prefix=${PREFIX} 
        --with-sysroot 
        --disable-nls 
        --disable-werror
        --enable-targets=all
    BUILD_COMMAND make -j8
    INSTALL_COMMAND make install
)

ExternalProject_Add(build_gcc
    URL https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gcc
    DEPENDS build_binutils
    
    PATCH_COMMAND ./contrib/download_prerequisites
    
    CONFIGURE_COMMAND <SOURCE_DIR>/configure 
        --target=${TARGET_ARCH} 
        --prefix=${PREFIX} 
        --disable-nls 
        --disable-libssp
        --disable-threads
        --disable-libgomp
        --disable-libquadmath
        --enable-languages=c,c++
        --without-headers
    
    # We only build the compiler, not the target libraries (libgcc/libstdc++) yet
    BUILD_COMMAND make all-gcc all-target-libgcc all-target-libstdc++ -j8
    INSTALL_COMMAND make install-gcc install-target-libgcc install-target-libstdc++
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/toolchain.cmake" 
    "set(CMAKE_SYSTEM_NAME Generic)\n"
    "set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)\n"
    "set(CMAKE_C_COMPILER ${PREFIX}/bin/${TARGET_ARCH}-gcc)\n"
    "set(CMAKE_CXX_COMPILER ${PREFIX}/bin/${TARGET_ARCH}-g++)\n"
    "set(CMAKE_ASM_COMPILER ${PREFIX}/bin/${TARGET_ARCH}-gcc)\n"
)