# Set project name
set(TargetName cppsl)

set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(GLOB_RECURSE TARGET_HEADERS ${TARGET_INCLUDE_FOLDER}/*.hpp)
file(GLOB_RECURSE TARGET_SOURCES *.cpp)

# Add executable
add_library(${TargetName} ${TARGET_SOURCES} ${TARGET_HEADERS})

# Add SOVERSION
set_target_properties(${TargetName} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

# Find dependencies
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Add include directories
target_include_directories(
   ${TargetName}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "$<BUILD_INTERFACE:${TARGET_INCLUDE_FOLDER}>"
   "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

# Retain include structure when installing
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/container DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/file DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/log DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/net DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/process DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/threading DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/time DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})
install(DIRECTORY ${TARGET_INCLUDE_FOLDER}/${TargetName}/utility DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName})

target_link_libraries(${TargetName} PRIVATE Threads::Threads fmt::fmt spdlog::spdlog ${PROCPS_LINK_LIBRARIES})

if (BUILD_SHARED_LIBS)
  # define export targets and file paths
  set(EXPORT_TARGETS ${TargetName})
  set(EXPORT_TARGETS_DIR ${CMAKE_INSTALL_LIBDIR}/${TargetName})
  set(EXPORT_TARGETS_FILE ${TargetName}Targets.cmake)
  set(EXPORT_TARGETS_FILE_PATH ${EXPORT_TARGETS_DIR}/${EXPORT_TARGETS_FILE})

  # install build information for applications
  install(
     TARGETS ${EXPORT_TARGETS}
     EXPORT ${TargetName}Targets
     DESTINATION ${EXPORT_TARGETS_DIR}
     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TargetName}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

  install(
     EXPORT ${TargetName}Targets
     FILE ${EXPORT_TARGETS_FILE}
     DESTINATION ${EXPORT_TARGETS_DIR}
  )

  include(CMakePackageConfigHelpers)
  # generate the config file that includes the exports
  configure_package_config_file(
     ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
     "${CMAKE_CURRENT_BINARY_DIR}/${TargetName}Config.cmake"
     INSTALL_DESTINATION "${EXPORT_TARGETS_DIR}"
  )
  write_basic_package_version_file(
     "${CMAKE_CURRENT_BINARY_DIR}/${TargetName}ConfigVersion.cmake"
     VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
     COMPATIBILITY AnyNewerVersion
  )
  install(
     FILES ${CMAKE_CURRENT_BINARY_DIR}/${TargetName}Config.cmake
     ${CMAKE_CURRENT_BINARY_DIR}/${TargetName}ConfigVersion.cmake
     DESTINATION ${EXPORT_TARGETS_DIR}
  )

  # export build information for non-install
  export(
     EXPORT ${TargetName}Targets
     FILE ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_TARGETS_FILE}
  )
endif ()
