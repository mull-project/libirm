# Do nothing if the LLVMCore target exists
if (TARGET LLVMCore)
  return()
endif()

if (NOT PATH_TO_LLVM)
  message("-- Looking for LLVM")
  find_package(LLVM CONFIG)
  if (LLVM_FOUND)
    message("-- Found LLVM ${LLVM_VERSION}")
    return()
  else()
  message(FATAL_ERROR " 
 You can specify full path to your LLVM installation or to the source code.
 Examples:
     cmake -G \"${CMAKE_GENERATOR}\" -DPATH_TO_LLVM=/opt/llvm-3.9.0 ${CMAKE_SOURCE_DIR}
     cmake -G \"${CMAKE_GENERATOR}\" -DPATH_TO_LLVM=/opt/llvm/source ${CMAKE_SOURCE_DIR}
")
  endif()
endif()

if (NOT IS_ABSOLUTE ${PATH_TO_LLVM})
  # Convert relative path to absolute path
  get_filename_component(PATH_TO_LLVM
    "${PATH_TO_LLVM}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
endif()

set (BUILD_AGAINST_PRECOMPILED_LLVM TRUE)
if (EXISTS ${PATH_TO_LLVM}/CMakeLists.txt)
  set (BUILD_AGAINST_PRECOMPILED_LLVM FALSE)
endif()

if (${BUILD_AGAINST_PRECOMPILED_LLVM})
  set (search_paths
    ${PATH_TO_LLVM}
    ${PATH_TO_LLVM}/lib/cmake
    ${PATH_TO_LLVM}/lib/cmake/llvm
    ${PATH_TO_LLVM}/lib/cmake/clang
    ${PATH_TO_LLVM}/share/clang/cmake/
    ${PATH_TO_LLVM}/share/llvm/cmake/
    )

  find_package(LLVM REQUIRED CONFIG PATHS ${search_paths} NO_DEFAULT_PATH)

  if (APPLE)
    if (LLVM_VERSION_MAJOR GREATER_EQUAL 12)
      # Precompiled LLVM 12 and 13 for macOS contains a hardcoded dependency on a very
      # specific version of libcurses:
      #
      #   set_target_properties(LLVMSupport PROPERTIES
      #     INTERFACE_LINK_LIBRARIES "m;ZLIB::ZLIB;/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.1.sdk/usr/lib/libcurses.tbd;LLVMDemangle"
      #   )
      #
      # So we are monkey-patching it here
      set_target_properties(LLVMSupport PROPERTIES
        INTERFACE_LINK_LIBRARIES "z;curses;m;LLVMDemangle")
    endif()
  endif()
else()
  macro(get_llvm_version_component input component)
    string(REGEX MATCH "${component} ([0-9]+)" match ${input})
    if (NOT match)
      message(FATAL_ERROR "Cannot find LLVM version component '${component}'")
    endif()
    set (${component} ${CMAKE_MATCH_1})
  endmacro()

  file(READ ${PATH_TO_LLVM}/CMakeLists.txt LLVM_CMAKELISTS)
  get_llvm_version_component("${LLVM_CMAKELISTS}" LLVM_VERSION_MAJOR)
  get_llvm_version_component("${LLVM_CMAKELISTS}" LLVM_VERSION_MINOR)
  get_llvm_version_component("${LLVM_CMAKELISTS}" LLVM_VERSION_PATCH)
  set (LLVM_VERSION ${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH})

  if (LIBIRM_BUILD_32_BITS)
    set (LLVM_BUILD_32_BITS ON CACHE BOOL "Forcing LLVM to be built for 32 bits as well" FORCE)
  endif()
  set (LLVM_ENABLE_PROJECTS "llvm" CACHE BOOL "Don't build anything besides LLVM core" FORCE)
  set (LLVM_TARGETS_TO_BUILD "host" CACHE STRING "Don't build " FORCE)

  add_subdirectory(${PATH_TO_LLVM} llvm-build-dir)

  # Normally, include paths provided by LLVMConfig.cmake
  # In this case we can 'steal' them from real targets
  get_target_property(LLVM_INCLUDE_DIRS LLVMSupport INCLUDE_DIRECTORIES)
  list(REMOVE_DUPLICATES LLVM_INCLUDE_DIRS)
endif()