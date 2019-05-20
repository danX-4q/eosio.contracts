/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

#include "printkit.hpp"

/*namespace eosiosystem {
   class system_contract;
}*/

namespace eosio {

   using std::string;

   class [[eosio::contract("safe.oracle")]] safeoracle : public contract {
      public:
         using contract::contract;
         typedef eosio::name         name;
         typedef eosio::asset        asset;
         typedef eosio::datastream<const char*>   datastream__const_char;
         typedef std::string         string;

         safeoracle( name receiver, name code,  datastream__const_char ds );

         [[eosio::action]]
         void pushcctx( checksum256 txid, asset ccamount, name account );

         [[eosio::action]]
         void drawasset( checksum256 txid, name account );

         //[[eosio::action]]
         //void test( checksum256 xtxid );

      private:

         struct [[eosio::table]] cctx {
            uint64_t          id;         //auto increament
            name              account;    //target account who is in safecode chain
            checksum256       txid;       //txid at safe chain
            asset             ccasset;    //asset(amount and token) at txid to account
            uint8_t           status;     //0: new for being drawed; 1: has been drawed

            uint64_t primary_key() const
            {
               return (id);
            }

            checksum256 get_txid() const
            {
               return (txid);
            }
         };

         typedef eosio::multi_index<"cctx"_n, cctx, 
            indexed_by<"txid"_n, const_mem_fun<cctx, checksum256, &cctx::get_txid>>
        > type_table__cctx;
   };

} /// namespace eosio
