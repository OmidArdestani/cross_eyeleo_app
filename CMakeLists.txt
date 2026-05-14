cmake_minimum_required(VERSION 3.19)
project(CrossEyeLeoApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(CROSSEYELEO_BUILD_APP "Build the main application" ON)
option(CROSSEYELEO_BUILD_TESTS "Build tests" OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Svg)

if(CROSSEYELEO_BUILD_APP)
    set(APP_SOURCES
        src/main.cpp
        src/app.cpp
        src/settingsmanager.cpp
        src/statemachine.cpp
        src/taskscheduler.cpp
        src/activitymonitor.cpp
        src/trayicon.cpp
        src/windows/settingswindow.cpp
        src/windows/pausewindowbase.hpp
        src/windows/bigpausewindow.cpp
        src/windows/minipausewindow.cpp
        src/windows/exerciseanimationwidget.cpp
        src/windows/notificationwindow.cpp
        src/windows/beforepausewindow.cpp
        src/exercises/exercisemanager.cpp
        resources/app.qrc
    )

    qt_add_executable(CrossEyeLeoApp
        WIN32 MACOSX_BUNDLE
        ${APP_SOURCES}
    )

    if(APPLE)
        target_sources(CrossEyeLeoApp PRIVATE src/platform/mac_utils.mm)
    endif()

    target_include_directories(CrossEyeLeoApp PRIVATE src)

    # APP_VERSION can be supplied from the command line (e.g. by CI using the
    # git tag: -DAPP_VERSION=1.2.0).  Fall back to the project version declared
    # above for local / dev builds.
    if(NOT DEFINED APP_VERSION OR APP_VERSION STREQUAL "")
        set(APP_VERSION "${PROJECT_VERSION}")
    endif()
    target_compile_definitions(CrossEyeLeoApp PRIVATE
        APP_VERSION="${APP_VERSION}"
    )

    target_link_libraries(CrossEyeLeoApp PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Svg
    )

    if(APPLE)
        find_library(APPKIT_LIBRARY AppKit REQUIRED)
        target_link_libraries(CrossEyeLeoApp PRIVATE ${APPKIT_LIBRARY})
    endif()

    if(WIN32)
        target_link_libraries(CrossEyeLeoApp PRIVATE user32)
    elseif(UNIX AND NOT APPLE)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(XSCRNSAVER xscrnsaver QUIET)
            if(XSCRNSAVER_FOUND)
                target_link_libraries(CrossEyeLeoApp PRIVATE ${XSCRNSAVER_LIBRARIES})
                target_include_directories(CrossEyeLeoApp PRIVATE ${XSCRNSAVER_INCLUDE_DIRS})
                target_compile_definitions(CrossEyeLeoApp PRIVATE HAVE_XSCRNSAVER)
            endif()
        endif()
        find_package(X11 QUIET)
        if(X11_FOUND)
            target_link_libraries(CrossEyeLeoApp PRIVATE ${X11_LIBRARIES})
            target_include_directories(CrossEyeLeoApp PRIVATE ${X11_INCLUDE_DIR})
        endif()
    endif()

    set_target_properties(CrossEyeLeoApp PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.crosseyeleo.app"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_NAME "CrossEyeLeoApp"
    )

    include(GNUInstallDirs)
    install(TARGETS CrossEyeLeoApp
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    set(CPACK_PACKAGE_NAME "CrossEyeLeoApp")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_VENDOR "CrossEyeLeo")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform eye exercise reminder")
    set(CPACK_PACKAGE_CONTACT "omid.ardestani@crosseyeleo.example.com")
    if(WIN32)
        set(CPACK_GENERATOR "ZIP")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
    else()
        set(CPACK_GENERATOR "DEB;TGZ")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CrossEyeLeo")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6gui6, libqt6widgets6, libqt6multimedia6, libqt6svg6")
    endif()
    include(CPack)
endif()

if(CROSSEYELEO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
