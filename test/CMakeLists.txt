find_package(Catch2 REQUIRED)

# enable testing
enable_testing()

# Add the test sources
file(GLOB SOURCE_FILES *.cpp)

# For each source file a test target is created
foreach (SOURCE_PATH ${SOURCE_FILES})

  get_filename_component(TargetName ${SOURCE_PATH} NAME_WE)

  # Add test target
  add_executable(${TargetName} ${SOURCE_PATH})

  # add compiler definition
  target_include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/include)
  target_include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

  ## link with library
  #  target_link_libraries(${TargetName} gtest gtest_main)
  target_link_libraries(${TargetName} PRIVATE Catch2::Catch2 cppsl)

  # Add the test to CTest
  add_test(NAME ${TargetName} COMMAND ${TargetName})

endforeach ()