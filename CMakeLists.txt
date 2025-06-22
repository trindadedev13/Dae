cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME dae)
project(${PROJECT_NAME} LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(DEFINED ENV{INSTALL})
  set(INSTALL TRUE)
else()
  set(INSTALL FALSE)
endif()


if(DEFINED ENV{ASAN})
  message(STATUS "AddressSanitizer enabled")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()


if(INSTALL)
  include(GNUInstallDirs)
endif()

set(LIB_SOURCES
    src/error.c
    src/file.c
    src/interpreter.c
    src/hashmap.c
    src/lexer.c
    src/node.c
    src/parser.c
    src/parser_native_funcs.c
    src/scope_stack.c
    src/string.c
    src/vector.c)

set(HEADERS
    include/dae/error.h
    include/dae/file.h
    include/dae/hashmap.h
    include/dae/inline.h
    include/dae/interpreter.h
    include/dae/lexer.h
    include/dae/node.h
    include/dae/parser.h
    include/dae/parser_native_funcs.h
    include/dae/scope_stack.h
    include/dae/string.h
    include/dae/vector.h)

set(EXE_SOURCES
    src/main.c)

add_library(${PROJECT_NAME}_lib STATIC
    ${LIB_SOURCES}
    ${HEADERS})

target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(Dae::DaeC ALIAS ${PROJECT_NAME}_lib)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Dae::DaeC)

if(INSTALL)
  # install cli executable
  install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  # install lib
  install(TARGETS ${PROJECT_NAME}_lib
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # install headers
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h")

  # exports targets
  install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE Dae::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()