find_package(Qt6 REQUIRED COMPONENTS Core Test Widgets)

add_executable(test_settingsmanager test_settingsmanager.cpp
    ../src/settingsmanager.cpp)
target_include_directories(test_settingsmanager PRIVATE ../src)
target_link_libraries(test_settingsmanager PRIVATE Qt6::Core Qt6::Test Qt6::Widgets)
add_test(NAME test_settingsmanager COMMAND test_settingsmanager)
set_tests_properties(test_settingsmanager PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(test_statemachine test_statemachine.cpp
    ../src/statemachine.cpp)
target_include_directories(test_statemachine PRIVATE ../src)
target_link_libraries(test_statemachine PRIVATE Qt6::Core Qt6::Test Qt6::Widgets)
add_test(NAME test_statemachine COMMAND test_statemachine)
set_tests_properties(test_statemachine PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
