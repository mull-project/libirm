set (SOURCES
  Mutations/DummyTests.cpp
)
add_executable(irm-tests ${SOURCES})
target_link_libraries(irm-tests irm google-test)