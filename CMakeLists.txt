cmake_minimum_required(VERSION 3.16)
project(QtCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required for Qt's meta-object compiler (moc), which generates the
# extra code behind Q_OBJECT, signals, and slots.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
# If you're on Qt5 instead of Qt6, replace the line above with:
# find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(QtCalculator
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/CalculatorEngine.h
    src/CalculatorEngine.cpp
)

target_link_libraries(QtCalculator PRIVATE Qt6::Widgets)

# --- Optional: Unit tests (see tests/test_calculator.cpp) ---
# Uncomment once you've added QTest to your Qt installation:
#
# find_package(Qt6 COMPONENTS Test REQUIRED)
# add_executable(CalculatorTests
#     tests/test_calculator.cpp
#     src/CalculatorEngine.h
#     src/CalculatorEngine.cpp
# )
# target_link_libraries(CalculatorTests PRIVATE Qt6::Test)
# enable_testing()
# add_test(NAME CalculatorTests COMMAND CalculatorTests)