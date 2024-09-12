cmake_minimum_required(VERSION 3.10)
project(Samples)

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_LIBRARY_ARCHITECTURE x64)
  else()
    set(CMAKE_LIBRARY_ARCHITECTURE x86)
  endif()
endif()

find_package(LeapSDK 4.0.0 PATHS "${CMAKE_CURRENT_LIST_DIR}/..")

function(c_sample_executable NAME)
  add_executable(${NAME} ${NAME}.c ExampleConnection.c ExampleConnection.h ${ARGN})
  target_include_directories(${NAME} PUBLIC ${Leap_ROOT_DIR}/include)
  target_link_libraries(${NAME} LeapSDK::LeapC)
  if(ANDROID)
    target_link_libraries(${NAME} -pthread -lm)
  elseif(UNIX AND NOT APPLE)
    target_link_libraries(${NAME} -lpthread)
  endif()
  add_custom_command(TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:LeapSDK::LeapC,IMPORTED_LOCATION_RELEASE> $<TARGET_FILE_DIR:${NAME}>)
endfunction()

c_sample_executable(RecordPlaybackSample)




