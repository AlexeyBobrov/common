include(vcpkg_common_functions)

vcpkg_from_git(
  OUT_SOURCE_PATH SOURCE_PATH
  URL https://github.com/AlexeyBobrov/common
  REF ddd897bf0530fd529bbc6fbc5bf1ed5c59dacce9
  )


vcpkg_configure_cmake(
  SOURCE_PATH ${SOURCE_PATH}
  PREFER_NINJA
  )

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/cmntype)
set(common_DIR lib/cmake/cmntype)


file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

# Handle copyright
configure_file(${SOURCE_PATH}/License.txt ${CURRENT_PACKAGES_DIR}/share/cmntype/copyright COPYONLY)

vcpkg_copy_pdbs()
