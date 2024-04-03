cmake_minimum_required(VERSION 3.20)

project(qotp)

# Option for enabling testing
option(WITH_TESTING "Build the tests." ON)

# Set the install prefix only if it hasn't been specified by the user
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Default install path" FORCE)
endif()

# Conditionally enable testing
if(WITH_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(libqotp)
