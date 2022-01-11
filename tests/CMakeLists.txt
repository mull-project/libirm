set (SOURCES
  Mutations/BinaryOperatorReplacementTests.cpp
  Mutations/CallRemovalTests.cpp
  Mutations/CallReplacementTests.cpp
  Mutations/CmpInstPredicateReplacementTests.cpp
  Mutations/ConstantReplacementTests.cpp
  Mutations/InstrinsicReplacementTests.cpp
  Mutations/NegateTruncReplacementTests.cpp
  Mutations/NegateXORReplacementTests.cpp
  Mutations/StoreValueReplacementTests.cpp
  Mutations/SwapBinaryOperandsTests.cpp
  Mutations/SwapInstructionWithOperandTests.cpp
)
add_executable(irm-tests EXCLUDE_FROM_ALL ${SOURCES})
target_link_libraries(irm-tests irm google-test)
target_compile_options(irm-tests PRIVATE ${LIBIRM_CMAKE_CXX_FLAGS})
target_include_directories(irm-tests SYSTEM PRIVATE ${LIBIRM_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})
target_include_directories(irm-tests PRIVATE ${CMAKE_CURRENT_LIST_DIR})