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