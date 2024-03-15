cmake_minimum_required(VERSION 3.5)

project(PasswordManageris VERSION 0.1 LANGUAGES CXX)

set(PROJECT_FOLDER "D:\\Password_Manageris")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)
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
        registerwindow.h registerwindow.cpp registerwindow.ui
        managerwindow.h managerwindow.cpp managerwindow.ui
        encryptindialog.h encryptindialog.cpp encryptindialog.ui pictures.qrc


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


  include_directories("C:\\Program\ Files\\MySQL\\MySQL\ Server\ 8.3\\lib")
  include_directories("C:\\Program\ Files\\MySQL\\MySQL\ Server\ 8.3\\include")
  include_directories("C:\\QT\\6.6.2\\Src\\qtbase\\src\\plugins\\sqldrivers\\plugins\\sqldrivers\\")
  include_directories("${PROJECT_FOLDER}\\libs\\argon2")
  include_directories("${PROJECT_FOLDER}\\libs\\argon2\\include")

  link_directories("${PROJECT_FOLDER}\\libs\\argon2\\")
  target_link_libraries(PasswordManageris PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
  target_link_libraries(PasswordManageris PRIVATE Qt6::Sql)
  target_link_libraries(PasswordManageris PRIVATE Qt6::Core)
  target_link_libraries(PasswordManageris PRIVATE "${PROJECT_FOLDER}\\libs\\argon2\\libargon2.a")

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

