# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "SJwt\\CMakeFiles\\SJwt_autogen.dir\\AutogenUsed.txt"
  "SJwt\\CMakeFiles\\SJwt_autogen.dir\\ParseCache.txt"
  "SJwt\\SJwt_autogen"
  "SWidget\\CMakeFiles\\SWidget_autogen.dir\\AutogenUsed.txt"
  "SWidget\\CMakeFiles\\SWidget_autogen.dir\\ParseCache.txt"
  "SWidget\\SWidget_autogen"
  "ThirdParty\\QXlsx\\CMakeFiles\\QXlsx_autogen.dir\\AutogenUsed.txt"
  "ThirdParty\\QXlsx\\CMakeFiles\\QXlsx_autogen.dir\\ParseCache.txt"
  "ThirdParty\\QXlsx\\QXlsx_autogen"
  "ThirdParty\\QsLog\\CMakeFiles\\QsLog_autogen.dir\\AutogenUsed.txt"
  "ThirdParty\\QsLog\\CMakeFiles\\QsLog_autogen.dir\\ParseCache.txt"
  "ThirdParty\\QsLog\\QsLog_autogen"
  "UserPrivilegeManager-Client\\CMakeFiles\\UserPrivilegeManager-Client_autogen.dir\\AutogenUsed.txt"
  "UserPrivilegeManager-Client\\CMakeFiles\\UserPrivilegeManager-Client_autogen.dir\\ParseCache.txt"
  "UserPrivilegeManager-Client\\LoginRegister\\CMakeFiles\\LoginRegister_autogen.dir\\AutogenUsed.txt"
  "UserPrivilegeManager-Client\\LoginRegister\\CMakeFiles\\LoginRegister_autogen.dir\\ParseCache.txt"
  "UserPrivilegeManager-Client\\LoginRegister\\LoginRegister_autogen"
  "UserPrivilegeManager-Client\\SubPage\\CMakeFiles\\SubPage_autogen.dir\\AutogenUsed.txt"
  "UserPrivilegeManager-Client\\SubPage\\CMakeFiles\\SubPage_autogen.dir\\ParseCache.txt"
  "UserPrivilegeManager-Client\\SubPage\\SubPage_autogen"
  "UserPrivilegeManager-Client\\UserPrivilegeManager-Client_autogen"
  "UserPrivilegeManager-Server\\CMakeFiles\\UserPrivilegeManager-Server_autogen.dir\\AutogenUsed.txt"
  "UserPrivilegeManager-Server\\CMakeFiles\\UserPrivilegeManager-Server_autogen.dir\\ParseCache.txt"
  "UserPrivilegeManager-Server\\UserPrivilegeManager-Server_autogen"
  "Util\\CMakeFiles\\Util_autogen.dir\\AutogenUsed.txt"
  "Util\\CMakeFiles\\Util_autogen.dir\\ParseCache.txt"
  "Util\\Util_autogen"
  )
endif()
