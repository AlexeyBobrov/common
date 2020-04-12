include(vcpkg_common_functions)

vcpkg_from_git(
  OUT_SOURCE_PATH SOURCE_PATH
  URL https://github.com/AlexeyBobrov/common
  REF 363f2c04baca416e744e5a347df988bb29ef2ff1 
  )


vcpkg_configure_cmake(
  SOURCE_PATH ${SOURCE_PATH}
  PREFER_NINJA
  )

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/ctlite)
set(common_DIR lib/cmake/ctlite)


file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

# Handle copyright
configure_file(${SOURCE_PATH}/License.txt ${CURRENT_PACKAGES_DIR}/share/ctlite/copyright COPYONLY)

vcpkg_copy_pdbs()
