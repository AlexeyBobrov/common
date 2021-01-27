set_property(TARGET ${ProjectName} PROPERTY VERSION "${PROJECT_FULL_VERSION}")
set_property(TARGET ${ProjectName} PROPERTY SOVERSION "1")

# install rules
install(TARGETS ${ProjectName} 
  DESTINATION lib
  EXPORT ${ProjectName}Targets
  )

install(DIRECTORY inc/${ProjectName}/common
  inc/${ProjectName}/curl
  inc/${ProjectName}/error
  inc/${ProjectName}/http
  inc/${ProjectName}/logger
  inc/${ProjectName}/thread
  DESTINATION include/${ProjectName}
  )

# install
install(FILES 
  "${PROJECT_BINARY_DIR}/common_version.h"
  inc/${ProjectName}/config.h
  DESTINATION include/${ProjectName}
  )

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
include(CPack)

# install configuration targets
install(EXPORT ${ProjectName}Targets
  FILE ${ProjectName}Targets.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Config.cmake"
  INSTALL_DESTINATION "lib/cmake/${ProjectName}"
  )

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
  )

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Config.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

# install the version file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}ConfigVersion.cmake
  DESTINATION lib/cmake/${ProjectName}
  )

# generate the export targets for the build tree
# needs to be after the install(TARGETS) command
export(EXPORT ${ProjectName}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}Targets.cmake"
  )