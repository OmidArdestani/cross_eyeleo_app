cmake_minimum_required(VERSION 3.19)
project(CrossEyeLeopApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(CROSSEYELEOP_BUILD_APP "Build the main application" ON)
option(CROSSEYELEOP_BUILD_TESTS "Build tests" OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Svg)

if(CROSSEYELEOP_BUILD_APP)
    set(APP_SOURCES
        src/main.cpp
        src/app.cpp
        src/settingsmanager.cpp
        src/statemachine.cpp
        src/taskscheduler.cpp
        src/activitymonitor.cpp
        src/trayicon.cpp
        src/windows/settingswindow.cpp
        src/windows/bigpausewindow.cpp
        src/windows/minipausewindow.cpp
        src/windows/notificationwindow.cpp
        src/windows/beforepausewindow.cpp
        src/exercises/exercisemanager.cpp
        resources/app.qrc
    )

    qt_add_executable(CrossEyeLeopApp
        WIN32 MACOSX_BUNDLE
        ${APP_SOURCES}
    )

    target_include_directories(CrossEyeLeopApp PRIVATE src)

    target_link_libraries(CrossEyeLeopApp PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Svg
    )

    if(WIN32)
        target_link_libraries(CrossEyeLeopApp PRIVATE user32)
    elseif(UNIX AND NOT APPLE)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(XSCRNSAVER xscrnsaver QUIET)
            if(XSCRNSAVER_FOUND)
                target_link_libraries(CrossEyeLeopApp PRIVATE ${XSCRNSAVER_LIBRARIES})
                target_include_directories(CrossEyeLeopApp PRIVATE ${XSCRNSAVER_INCLUDE_DIRS})
                target_compile_definitions(CrossEyeLeopApp PRIVATE HAVE_XSCRNSAVER)
            endif()
        endif()
        find_package(X11 QUIET)
        if(X11_FOUND)
            target_link_libraries(CrossEyeLeopApp PRIVATE ${X11_LIBRARIES})
            target_include_directories(CrossEyeLeopApp PRIVATE ${X11_INCLUDE_DIR})
        endif()
    endif()

    set_target_properties(CrossEyeLeopApp PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.crosseyeleop.app"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_NAME "CrossEyeLeopApp"
    )

    include(GNUInstallDirs)
    install(TARGETS CrossEyeLeopApp
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    set(CPACK_PACKAGE_NAME "CrossEyeLeopApp")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_VENDOR "CrossEyeLeop")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform eye exercise reminder")
    set(CPACK_PACKAGE_CONTACT "contact@crosseyeleop.example.com")
    if(WIN32)
        set(CPACK_GENERATOR "ZIP")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
    else()
        set(CPACK_GENERATOR "DEB;TGZ")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CrossEyeLeop")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6gui6, libqt6widgets6, libqt6multimedia6")
    endif()
    include(CPack)
endif()

if(CROSSEYELEOP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
