# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

find_package(yat CONFIG REQUIRED)

# --------------------------------------------------------------------------
# Collect sources and includes
# --------------------------------------------------------------------------

file(GLOB_RECURSE sources src/*.cpp)
set(includedirs
    include/
    sdk/include/
)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------

add_library(limaspectrumone ${sources})

target_include_directories(limaspectrumone
    PUBLIC 
        ${includedirs}
)


target_link_directories(limaspectrumone
    PUBLIC
        sdk/lib/
)

target_link_libraries(limaspectrumone 
    PUBLIC
        limacore
        yat::yat
    PRIVATE
        ni4882
)

limatools_set_library_soversion(limaspectrumone "VERSION")

# --------------------------------------------------------------------------
# Compile definitions
# --------------------------------------------------------------------------

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" SPECTRUMONE_VERSION_RAW)
string(STRIP "${SPECTRUMONE_VERSION_RAW}" SPECTRUMONE_VERSION)

target_compile_definitions(limaspectrumone
    PUBLIC
        SPECTRUMONE_NAME=SpectrumOne
        SPECTRUMONE_VERSION=${SPECTRUMONE_VERSION}
        SPECTRUMONE_ENABLED
)

message(STATUS "Camera enabled: SpectrumOne ${SPECTRUMONE_VERSION}")

target_compile_definitions(limaspectrumone
    PRIVATE
        YAT_ENABLE_LOG
        SOLEIL_YAT_STREAM
)

# --------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------

install(
    TARGETS limaspectrumone 
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY sdk/bin/
    DESTINATION bin 
    FILES_MATCHING PATTERN "*.dll"
)
