add_contract(safe.oracle safe.oracle ${CMAKE_CURRENT_SOURCE_DIR}/src/safe.oracle.cpp)
target_include_directories(safe.oracle.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(safe.oracle.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
