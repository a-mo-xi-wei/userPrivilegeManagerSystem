cmake_minimum_required(VERSION 3.10)

project(UserPrivilegeManager-Client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Widgets Network UiTools)

file(GLOB USERPRIVILEGE_CLIENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.CPP)

add_subdirectory(LoginRegister)

add_subdirectory(SubPage)

add_executable(${PROJECT_NAME} ${USERPRIVILEGE_CLIENT_SOURCES} "ResourceClient.qrc")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::UiTools LoginRegister SWidget SubPage)