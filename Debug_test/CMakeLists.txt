cmake_minimum_required(VERSION 2.8)

set( PROJECT_NAME Debug_node )
project(${PROJECT_NAME})



set(CMAKE_BUILD_TYPE "Debug") #Release

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -std=gnu++11")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g"
#指定输出可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin) 
#指定库文件输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../lib)    
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../lib)
#分别设置了Debug版本和Release版本下库文件的后缀名.
set(CMAKE_DEBUG_POSTFIX "_d")    
set(CMAKE_RELEASE_POSTFIX "_r") 
#分别设置了Debug版本和Release版本下可执行文件的后缀名.
set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "_d")    
set_target_properties(${TARGET_NAME} PROPERTIES RELEASE_POSTFIX "_r")

# Set Compiler，指定编译为32位程序
IF (WIN32)  
SET(CMAKE_C_FLAGS "-m64")
SET(CMAKE_CXX_FLAGS "-m64")
ELSEIF (UNIX) 
SET(CMAKE_C_FLAGS "-m64")
SET(CMAKE_CXX_FLAGS "-m64")
ENDIF () 

aux_source_directory(./pathcontrol SRC_FILES)
aux_source_directory(./MPC SRC_FILES)
aux_source_directory(./APP SRC_FILES)
aux_source_directory(../common/udpinterface SRC_FILES)
aux_source_directory(../common/CJson SRC_Cjson_FILES)
aux_source_directory(../common SRC_FILES)
aux_source_directory(../common/readconfig SRC_Cjson_FILES)


#添加头文件目录
include_directories(
     ../common/
     ../common/udpinterface/
    )
#link_directories("F:\Work\AAGG\Debug_test\MPC")
link_directories("D:/Project/model/adeall_AAGG/Debug_test/MPC")
#添加连接库路径
list( APPEND PROJECK_LINKS_LIBS
    pthread
    libmpc_solver
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
    "keyboard.cpp"
    ${SRC_FILES}
    ${SRC_BASIC_FILES}
    ${SRC_Cjson_FILES}
    )

#最后一步,为可执行文件链接所有库
target_link_libraries(${PROJECT_NAME} ${PROJECK_LINKS_LIBS})

