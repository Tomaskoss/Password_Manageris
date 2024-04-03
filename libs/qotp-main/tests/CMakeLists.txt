# Required Qt libraries
find_package(Qt6 COMPONENTS Test Core REQUIRED)

# Macro to add a test executable, link libraries, and set properties
#
# Usage:
#   add_qotp_test(NAME <test_name> SOURCE <source_file.cpp>)
#
# Parameters:
# - NAME: The name of the test target.
# - SOURCE: The source file for the test executable.
#
macro(add_qotp_test)
    set(options "")
    set(oneValueArgs NAME SOURCE)
    set(multiValueArgs "")
    cmake_parse_arguments(QOTP_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT QOTP_TEST_NAME OR NOT QOTP_TEST_SOURCE)
        message(FATAL_ERROR "add_qotp_test called without required arguments")
    endif()

    add_executable(${QOTP_TEST_NAME} ${QOTP_TEST_SOURCE})
    target_link_libraries(${QOTP_TEST_NAME} Qt6::Test Qt6::Core libqotp)
    add_test(NAME ${QOTP_TEST_NAME} COMMAND ${QOTP_TEST_NAME})
    set_target_properties(${QOTP_TEST_NAME} PROPERTIES AUTOMOC ON)
    install(TARGETS ${QOTP_TEST_NAME} RUNTIME DESTINATION bin)
endmacro()

# Tests
add_qotp_test(NAME test_hotp SOURCE test_hotp.cpp)
add_qotp_test(NAME test_totp SOURCE test_totp.cpp)
