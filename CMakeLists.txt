cmake_minimum_required(VERSION 3.5)

project(PasswordManageris VERSION 0.1 LANGUAGES CXX)

set(PROJECT_FOLDER "D:\\Password_Manageris")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(OPENSSL_ROOT_DIR "${PROJECT_FOLDER}\\libs\\openSSL_build")
set(qotp_DIR "${PROJECT_FOLDER}\\libs\\qotp-main\\qotp_build\\CMakeFiles")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Core )
find_package(OpenSSL REQUIRED)




# find_package (qotp REQUIRED)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PasswordManageris
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        managerwindow.h managerwindow.cpp managerwindow.ui
        pictures.qrc

    )




# Define target properties for Android with Qt 6 as:
#    set_property(TARGET PasswordManageris APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(PasswordManageris SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PasswordManageris
            ${PROJECT_SOURCES}
        )
    endif()
endif()



    include_directories("${PROJECT_FOLDER}\\libs\\argon2")
    include_directories("${PROJECT_FOLDER}\\libs\\argon2\\include")
    link_directories("${PROJECT_FOLDER}\\libs\\argon2")
    target_link_libraries(PasswordManageris PRIVATE "${PROJECT_FOLDER}\\libs\\argon2\\libargon2.a")



    target_link_libraries(PasswordManageris PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(PasswordManageris PRIVATE Qt6::Sql)
    target_link_libraries(PasswordManageris PRIVATE Qt6::Core)


    target_link_libraries(PasswordManageris PRIVATE ${qotp_LIBRARIES})
    include_directories("${PROJECT_FOLDER}\\libs\\qotp-main\\install\\include")
    include_directories("${PROJECT_FOLDER}\\libs\\qotp-main\\install")
    link_directories("${PROJECT_FOLDER}\\libs\\qotp-main\\install\\lib")
    target_link_libraries(PasswordManageris PRIVATE "${PROJECT_FOLDER}\\libs\\qotp-main\\install\\lib\\liblibqotp.a")

    include_directories("${PROJECT_FOLDER}\\libs/openSSL_build\\include")

    link_directories("${PROJECT_FOLDER}\\libs\\openSSL_build\\lib")

    target_link_libraries(PasswordManageris PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_link_libraries(PasswordManageris PRIVATE ${OPENSSL_LIBRARIES})


    include_directories("${PROJECT_FOLDER}\\libs\\QtSmtpMime")
    include_directories("${PROJECT_FOLDER}\\libs\\QtSmtpMime\\include\\smtpmime")
    link_libraries("${PROJECT_FOLDER}\\libs\\QtSmtpMime\\bin")
    target_link_libraries(PasswordManageris PRIVATE "${PROJECT_FOLDER}\\libs\\QtSmtpMime\\bin\\libQtSmtpMime.dll")


    target_compile_features(PasswordManageris PRIVATE cxx_range_for)



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PasswordManageris)
endif()
set_target_properties(PasswordManageris PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PasswordManageris
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PasswordManageris)
endif()

