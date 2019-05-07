/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/safechain.h>

#include <string>

/*namespace eosiosystem {
   class system_contract;
}*/

namespace eosio {

   using std::string;

   class [[eosio::contract("safe.chain")]] safechain : public contract {
      public:
         using contract::contract;

         [[eosio::action]]
         void feed( string   txid );

      private:

   };

} /// namespace eosio
