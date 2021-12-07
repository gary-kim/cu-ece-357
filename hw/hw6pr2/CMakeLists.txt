cmake_minimum_required(VERSION 3.0.0)
project(hw6pr2 C ASM)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "--std=gnu11")
set(SOURCE_FILES test.c locks/spinlock.c locks/sem.c 3rdparty/tas.S)
set(INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/3rdparty ${CMAKE_CURRENT_LIST_DIR}/locks)

add_executable(test_nolock ${SOURCE_FILES})
target_include_directories(test_nolock PRIVATE ${INCLUDE_DIRS})

add_executable(spinlock ${SOURCE_FILES})
target_compile_definitions(spinlock PRIVATE USE_SPINLOCK)
target_include_directories(spinlock PRIVATE ${INCLUDE_DIRS})

add_executable(semaphore ${SOURCE_FILES})
target_compile_definitions(semaphore PRIVATE USE_SEM)
target_include_directories(semaphore PRIVATE ${INCLUDE_DIRS})
