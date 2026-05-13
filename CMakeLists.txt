cmake_minimum_required(VERSION 3.24)
project(cross_eyeleop_app VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CROSSEYELEOP_BUILD_APP "Build Qt desktop app" ON)
option(CROSSEYELEOP_BUILD_TESTS "Build tests" ON)

add_library(cross_eyeleop_core
    src/domain/policies/BreakSchedulePolicy.cpp
    src/domain/services/SessionStateMachine.cpp
    src/application/services/SessionCoordinator.cpp
)

target_include_directories(cross_eyeleop_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(CROSSEYELEOP_BUILD_APP)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    include(GNUInstallDirs)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    add_executable(cross_eyeleop_app
        src/main.cpp
        src/infrastructure/settings/QSettingsRepository.cpp
        src/infrastructure/timing/QtHeartbeat.cpp
        src/presentation/viewmodels/MainViewModel.cpp
        src/presentation/windows/MainWindow.cpp
    )

    target_link_libraries(cross_eyeleop_app PRIVATE Qt6::Widgets cross_eyeleop_core)

    install(TARGETS cross_eyeleop_app
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION .
    )

    qt_generate_deploy_app_script(
        TARGET cross_eyeleop_app
        FILENAME_VARIABLE cross_eyeleop_deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${cross_eyeleop_deploy_script})
endif()

if(CROSSEYELEOP_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(domain_policy_tests
        tests/domain/SessionCoordinatorTests.cpp
    )

    target_link_libraries(domain_policy_tests PRIVATE cross_eyeleop_core)
    add_test(NAME domain_policy_tests COMMAND domain_policy_tests)
endif()

set(CPACK_PACKAGE_NAME "cross-eyeleop-app")
set(CPACK_PACKAGE_VENDOR "cross_eyeleop_app")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP;TGZ")
include(CPack)
