
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("speexdroid")

enable_language(C)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
add_definitions(-DEXPORT= -DFLOATING_POINT -DUSE_SMALLFT)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include_directories(src/main/cpp/include)

aux_source_directory(src/main/cpp/libspeex/ LIB_SPEEX_DIR)
aux_source_directory(src/main/cpp/libspeexdsp/ LIB_SPEEXDSP_DIR)

add_library( # Sets the name of the library.
        speex

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${LIB_SPEEX_DIR})

add_library( # Sets the name of the library.
        speexdsp

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${LIB_SPEEXDSP_DIR})

add_library( # Sets the name of the library.
        speexdroid

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/echo_canceller.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        speexdroid

        speex
        speexdsp

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
