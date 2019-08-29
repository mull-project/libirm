set (SOURCES
  Mutations/Dummy.cpp
)

add_library(irm ${SOURCES})
target_compile_options(irm PUBLIC ${LIBIRM_CMAKE_CXX_FLAGS})
target_include_directories(irm PUBLIC ${LIBIRM_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
if (TARGET LLVM)
  target_link_libraries(irm PUBLIC LLVM)
else()
  target_link_libraries(irm PUBLIC LLVMCore)
endif()