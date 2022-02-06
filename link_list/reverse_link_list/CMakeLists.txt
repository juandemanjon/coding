find_package(Catch2 REQUIRED)

set(This reverse_link_listTest)

add_executable(${This} reverse_link_list.cpp)
target_link_libraries(${This} PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${This})

include(CPack)