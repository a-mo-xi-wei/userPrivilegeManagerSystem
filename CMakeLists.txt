cmake_minimum_required(VERSION 3.10)

project(UserPrivilegeManagerSystem VERSION 1.0 LANGUAGES CXX)

#Util模块
add_subdirectory(Util)

#SWidget模块
add_subdirectory(SWidget)

#SJwt模块
add_subdirectory(SJwt)

#日志模块
add_subdirectory(ThirdParty/QsLog)

#excel模块
add_subdirectory(ThirdParty/QXlsx)

#客户端
add_subdirectory(UserPrivilegeManager-Client)

#服务器
add_subdirectory(UserPrivilegeManager-Server)