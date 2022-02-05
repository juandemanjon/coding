find_package(Catch2 REQUIRED)

set(This calculatorTest)

add_executable(${This} calculator.cpp)
target_link_libraries(${This} PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${This})

include(CPack)