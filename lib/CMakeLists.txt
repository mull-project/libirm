set (SOURCES
  ${CMAKE_SOURCE_DIR}/include/irm/Mutations/BinaryOperatorReplacement.h
)

add_library(irm INTERFACE)
target_compile_options(irm INTERFACE ${LIBIRM_CMAKE_CXX_FLAGS})
target_include_directories(irm INTERFACE ${LIBIRM_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
if (TARGET LLVM)
  target_link_libraries(irm INTERFACE LLVM)
else()
  target_link_libraries(irm INTERFACE LLVMCore)
endif()