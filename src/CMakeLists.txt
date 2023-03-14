CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(Fingerprint)

#set(CMAKE_C_FLAGS "-g -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(INC_DIR /usr/include/libxml2)  
set(LINK_DIR /usr/lib/x86_64-linux-gnu)
include_directories(${INC_DIR})
link_directories(${LINK_DIR})

#file(GLOB_RECURSE files ${SRC_DIR}/*.c)

set(CMAKE_CXX_COMPILER "gcc")
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#set(LINK_DIR )

AUX_SOURCE_DIRECTORY(./ FILES_LIST)
AUX_SOURCE_DIRECTORY(./config FILES_CONFIG)
AUX_SOURCE_DIRECTORY(./filter FILES_FILTER)
AUX_SOURCE_DIRECTORY(./net FILES_NET)
AUX_SOURCE_DIRECTORY(./pcap FILES_PCAP)
AUX_SOURCE_DIRECTORY(./xml2 FILES_XML2)
AUX_SOURCE_DIRECTORY(./scanner FILES_SCANNER)
AUX_SOURCE_DIRECTORY(./comm FILES_COMM)
AUX_SOURCE_DIRECTORY(./yara FILES_YARA)
AUX_SOURCE_DIRECTORY(./assets FILES_ASSETS)

list(APPEND FILES_LIST ${FILES_CONFIG})
list(APPEND FILES_LIST ${FILES_FILTER})
list(APPEND FILES_LIST ${FILES_NET})
list(APPEND FILES_LIST ${FILES_PCAP})
list(APPEND FILES_LIST ${FILES_XML2})
list(APPEND FILES_LIST ${FILES_SCANNER})
list(APPEND FILES_LIST ${FILES_COMM})
list(APPEND FILES_LIST ${FILES_YARA})
list(APPEND FILES_LIST ${FILES_ASSETS})

ADD_EXECUTABLE(Fingerprint ${FILES_LIST})

#CREATE LIB
add_library(finger SHARED ${FILES_LIST})
add_library(Finger_static STATIC ${FILES_LIST})

#set static lib name
set_target_properties(Finger_static PROPERTIES OUTPUT_NAME "finger")

#static and shared lib 
set_target_properties(finger PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(Finger_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#set version
set_target_properties(finger PROPERTIES VERSION 2.0 SOVERSION 1)

target_link_libraries(Fingerprint libxml2.so libpcap.so yara)

