cmake_minimum_required(VERSION 3.0.0)
project(hw6pr2 C ASM)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "--std=gnu11")
set(SOURCE_FILES locks/spinlock.c locks/sem.c)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SOURCE_FILES ${SOURCE_FILES} 3rdparty/tas64.S)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SOURCE_FILES ${SOURCE_FILES} 3rdparty/tas.S)
endif()

set(INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/3rdparty ${CMAKE_CURRENT_LIST_DIR}/locks)

add_executable(test_nolock test.c ${SOURCE_FILES})
target_include_directories(test_nolock PRIVATE ${INCLUDE_DIRS})

add_executable(spinlock test.c ${SOURCE_FILES})
target_compile_definitions(spinlock PRIVATE USE_SPINLOCK)
target_include_directories(spinlock PRIVATE ${INCLUDE_DIRS})

add_executable(semaphore test.c ${SOURCE_FILES})
target_compile_definitions(semaphore PRIVATE USE_SEM)
target_include_directories(semaphore PRIVATE ${INCLUDE_DIRS})

add_executable(shell shell.c ${SOURCE_FILES})
target_compile_definitions(shell PRIVATE USE_SEM)
target_include_directories(shell PRIVATE ${INCLUDE_DIRS})
