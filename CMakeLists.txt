cmake_minimum_required(VERSION 3.17)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(ip_filter VERSION ${PROJECT_VESRSION})

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

set(CPACK_PACKAGE_CONTACT mugutdinov@yandex.ru)
set(CPACK_GENERATOR DEB)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
include(CPack)