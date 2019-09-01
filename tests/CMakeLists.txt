set (SOURCES
  Mutations/BinaryOperatorReplacementTests.cpp
  Mutations/CallReplacementTests.cpp
  Mutations/CmpInstPredicateReplacementTests.cpp
  Mutations/InstrinsicReplacementTests.cpp
)
add_executable(irm-tests ${SOURCES})
target_link_libraries(irm-tests irm google-test)
target_include_directories(irm-tests PRIVATE ${CMAKE_CURRENT_LIST_DIR})