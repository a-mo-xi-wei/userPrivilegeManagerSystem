cmake_minimum_required(VERSION 3.2)

project(QsLog)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core  REQUIRED)


set(QsLog_SOURCES 
    QsLogDest.cpp 
    QsLog.cpp 
    QsLogDestConsole.cpp 
    QsLogDestFile.cpp 
    QsLogDestFunctor.cpp
)

set(QsLog_HEADERS 
    QsLogDest.h 
    QsLog.h 
    QsLogDestConsole.h 
    QsLogLevel.h 
    QsLogDestFile.h 
    QsLogDisableForThisFile.h 
    QsLogDestFunctor.h
)

add_library(QsLog STATIC ${QsLog_SOURCES} ${QsLog_HEADERS} )

target_include_directories(QsLog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(QsLog PUBLIC Qt${QT_VERSION_MAJOR}::Core)

