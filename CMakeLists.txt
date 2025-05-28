cmake_minimum_required(VERSION 3.10)
project(audiophile C)

# Include your own headers
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# Find GTK3 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include GTK3 includes and libs
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add OpenSSL includes (you can add them to your include path)
include_directories(${OPENSSL_INCLUDE_DIR})

# Add executable
add_executable(${PROJECT_NAME}
    main.c
    src/wav.c
    src/dct.c
    src/cipher.c
    src/aes.c
    src/lzw.c
    src/huffman.c
    src/spectrum.c
    include/miniaudio.h
    include/wav.h
    include/dct.h
    include/cipher.h
    include/aes.h
    include/lzw.h
    include/huffman.h
    include/spectrum.h
)

# Link libraries: GTK3, math lib, and OpenSSL libs
target_link_libraries(${PROJECT_NAME}
    ${GTK3_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    m
)
