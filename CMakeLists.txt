cmake_minimum_required(VERSION 3.17)
project(ip_filter VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

set(CPACK_PACKAGE_CONTACT mugutdinov@yandex.ru)
set(CPACK_GENERATOR DEB)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
include(CPack)