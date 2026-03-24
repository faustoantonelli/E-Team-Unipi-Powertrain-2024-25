cmake_minimum_required(VERSION 3.14)
project(ETeam_Powertrain_TC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch Google Test (Usa il tag specifico, è più stabile e veloce)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0 
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Target: Libreria di Controllo
# ATTENZIONE: Ho aggiunto la "d" a AdvancedSlip.cpp
add_library(control_logic 
    src/TractionControl.cpp
    src/VehicleDynamics.cpp
    src/ElectronicDifferential.cpp
    src/AdvancedSlip.cpp
    src/LaunchControl.cpp
)
target_include_directories(control_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 3. Target: Eseguibile dei Test
add_executable(unit_tests tests/unit_tests.cpp)
target_link_libraries(unit_tests GTest::gtest_main control_logic)

include(CTest)
add_test(NAME AllTests COMMAND unit_tests)