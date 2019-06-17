add_contract(sc.eosio.token sc.eosio.token ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.token.cpp)
target_include_directories(sc.eosio.token.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(sc.eosio.token.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
