cmake_minimum_required(VERSION 3.0.0)
project(hw6pr2 C ASM)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "--std=gnu11")
set(HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/spinlock.h ${CMAKE_CURRENT_LIST_DIR}/3rdparty/tas.h)

add_executable(spinlock spinlock_test.c spinlock.c 3rdparty/tas.S)
target_compile_definitions(spinlock PRIVATE USE_LOCK)
target_include_directories(spinlock PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/3rdparty)

add_executable(spinlock_nolock spinlock_test.c spinlock.c 3rdparty/tas.S)
target_include_directories(spinlock_nolock PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/3rdparty)
