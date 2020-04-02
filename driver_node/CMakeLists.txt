cmake_minimum_required(VERSION 2.8)

set( PROJECT_NAME Driver_node )
project(${PROJECT_NAME})



set(CMAKE_BUILD_TYPE "Debug") #Release

set(CMAKE_CXX_FLAGS "-std=c++11 -std=gnu++11")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g"
# Set Compiler，指定编译为32位程序
SET(CMAKE_C_FLAGS "-m32")
SET(CMAKE_CXX_FLAGS "-m32")
aux_source_directory(./DriverApp SRC_FILES)
aux_source_directory(../common/udpinterface SRC_FILES)
aux_source_directory(../common/CJson SRC_Cjson_FILES)
aux_source_directory(../common SRC_Cjson_FILES)
aux_source_directory(../common/windows SRC_Cjson_FILES)
#添加头文件目录
include_directories(
     ../common/
     ../common/udpinterface/
     ../common/windows
    )
#添加连接库路径
list( APPEND PROJECK_LINKS_LIBS
    pthread
    ) 

IF (WIN32)  
    MESSAGE(STATUS "Now is windows")  
    list( APPEND PROJECK_LINKS_LIBS
    ws2_32) 
ELSEIF (APPLE)  
    MESSAGE(STATUS "Now is Apple systens.")  
ELSEIF (UNIX)  
    MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin") 
ENDIF () 
 
#为工程添加源文件,,第一个为工程名
add_executable( ${PROJECT_NAME}
    "main.cpp"
    ${SRC_FILES}
    ${SRC_Cjson_FILES}
    )

#最后一步,为可执行文件链接所有库
target_link_libraries(${PROJECT_NAME} ${PROJECK_LINKS_LIBS})
