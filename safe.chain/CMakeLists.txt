add_contract(safe.chain safe.chain ${CMAKE_CURRENT_SOURCE_DIR}/src/safe.chain.cpp)
target_include_directories(safe.chain.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(safe.chain.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
