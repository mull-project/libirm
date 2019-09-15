set (SOURCES
  Mutations/BinaryOperatorReplacementTests.cpp
  Mutations/CallRemovalTests.cpp
  Mutations/CallReplacementTests.cpp
  Mutations/CmpInstPredicateReplacementTests.cpp
  Mutations/ConstatnReplacementTests.cpp
  Mutations/InstrinsicReplacementTests.cpp
  Mutations/StoreValueReplacementTests.cpp
)
add_executable(irm-tests ${SOURCES})
target_link_libraries(irm-tests irm google-test)
target_include_directories(irm-tests SYSTEM PRIVATE ${LIBIRM_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_include_directories(irm-tests PRIVATE ${CMAKE_CURRENT_LIST_DIR})