/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/safechain.h>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

#include "printkit.hpp"

/*namespace eosiosystem {
   class system_contract;
}*/

namespace eosio {

   using std::string;

   class [[eosio::contract("safe.chain")]] safechain : public contract {
      public:
         using contract::contract;
         typedef eosio::name         name;
         typedef eosio::asset        asset;
         typedef eosio::datastream<const char*>   datastream__const_char;
         typedef std::string         string;

         safechain( name receiver, name code,  datastream__const_char ds );

         [[eosio::action]]
         void feed( string   txid );

         [[eosio::action]]
         void feed2( name r_account, string org_chain, string txid );

      private:

         struct [[eosio::table]] receive {
            uint64_t          id;
            name              receiver;
            string            txchain;
            string            txid;
            checksum256       xtxid;

            uint64_t primary_key() const
            {
               return (id);
            }

            checksum256 get_xtxid() const
            {
               return (xtxid);
            }
         };

         typedef eosio::multi_index<"receive"_n, receive, 
            indexed_by<"xtxid"_n, const_mem_fun<receive, checksum256, &receive::get_xtxid>>
        > type_table__receive;
   };

} /// namespace eosio
