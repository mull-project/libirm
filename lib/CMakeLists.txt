set (SOURCES
  BinaryOperatorReplacement.cpp
  CallReplacement.cpp
  CmpInstPredicateReplacement.cpp
  ConstantReplacement.cpp
  ConstValueConstructor.cpp
  IntrinsicReplacement.cpp
  StoreValueReplacement.cpp
  VoidCallRemoval.cpp
)

add_library(irm ${SOURCES})
target_compile_options(irm PRIVATE ${LIBIRM_CMAKE_CXX_FLAGS})
target_include_directories(irm PRIVATE ${LIBIRM_INCLUDE_DIRS})
target_include_directories(irm SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
if (TARGET LLVM)
  target_link_libraries(irm PRIVATE LLVM)
else()
  target_link_libraries(irm PRIVATE LLVMCore)
endif()