set (SOURCES
  BinaryOperatorReplacement.cpp
  CallReplacement.cpp
  CmpInstPredicateReplacement.cpp
  ConstantReplacement.cpp
  ConstValueConstructor.cpp
  IntrinsicReplacement.cpp
  NegateTrunkReplacement.cpp
  NegateXORReplacement.cpp
  StoreValueReplacement.cpp
  SwapBinaryOperands.cpp
  SwapInstructionWithOperand.cpp
  VoidCallRemoval.cpp
)

add_library(irm ${SOURCES})
target_compile_options(irm PRIVATE ${LIBIRM_CMAKE_CXX_FLAGS})
target_include_directories(irm PUBLIC ${LIBIRM_INCLUDE_DIR})
target_include_directories(irm SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
if (TARGET LLVM)
  target_link_libraries(irm PRIVATE LLVM)
else()
  target_link_libraries(irm PRIVATE LLVMCore)
endif()

install(TARGETS irm
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(DIRECTORY ${LIBIRM_INCLUDE_DIR}/irm
  DESTINATION include
)