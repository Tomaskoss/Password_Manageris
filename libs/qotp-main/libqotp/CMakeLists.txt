project(libqotp)

# Required Qt libraries
find_package(Qt6 COMPONENTS Core REQUIRED)

# Header and Source Files
set(headers
    "include/libqotp/qotp.h"
)
set(sources
    "src/hotp.cpp"
    "src/totp.cpp"
    "src/base32.cpp"
)

# Library Definition
add_library(${PROJECT_NAME} STATIC ${headers} ${sources})

# Linking Qt Core Library
target_link_libraries(${PROJECT_NAME} Qt6::Core)

# Include Directories
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Setting Properties
# Adds a 'd' postfix for debug builds (common practice on Windows)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

# Installation Rules
# Specifies where to install the library files
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Installation for Header Files
# Includes the header files in the installation
install(DIRECTORY include/
    DESTINATION include
)
