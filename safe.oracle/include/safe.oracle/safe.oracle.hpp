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

      public:     //////////////////////////////////////////////////
      
         using contract::contract;
         typedef eosio::name           name;
         typedef eosio::asset          asset;
         typedef eosio::datastream<const char*>   datastream__const_char;
         typedef std::string           string;

      private:    //////////////////////////////////////////////////

         struct chain_pos {
            uint32_t       block_num;
            uint16_t       tx_index;        //based 0

            void print() const
            {
               prints("block_num = "); printui(block_num); prints("\n");
               prints("tx_index = "); printui(tx_index); prints("\n");
            }
         };

         struct cctx_info {
            uint8_t           type;    //0: common transfer asset; 1: sync vote result from safe-chain
            name              account; //target account who is in safecode chain
            checksum256       txid;    //txid at safe chain
            asset             quantity;//asset(amount and token) at txid to account
            string            detail;  //detail(json string) at txid
         };

      public:     //////////////////////////////////////////////////

         safeoracle( name receiver, name code,  datastream__const_char ds );

         [[eosio::action]]
         void rstchainpos();

         [[eosio::action]]
         void pushcctxes( struct chain_pos curpos, struct chain_pos nextpos, const std::vector< struct cctx_info >& cctxes );

         [[eosio::action]]
         void drawassets( const std::vector< checksum256 >& txids );

      private:

         struct [[eosio::table]] cctx {
            uint64_t          id;         //auto increament
            name              account;    //target account who is in safecode chain
            checksum256       txid;       //txid at safe chain
            asset             quantity;   //asset(amount and token) at txid to account
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

         //////////////////////////////

         struct [[eosio::table]] globalkv {
            uint32_t       block_num;
            uint16_t       tx_index;        //based 0

            void print() const
            {
               prints("block_num = "); printui(block_num); prints("\n");
               prints("tx_index = "); printui(tx_index); prints("\n");
            }
         };

         typedef eosio::singleton<"globalkv"_n, globalkv>     type_table__globalkv;

         //////////////////////////////

         void init_globalkv( type_table__globalkv &tbl_globalkv );
         void push_each_cctx( type_table__cctx& tbl_cctx, const cctx_info& txinfo );
         void draw_each_asset( type_table__cctx& tbl_cctx, const checksum256 txid );

         static uint32_t   dft__last_safed_block_num;
         static string checksum256_to_string( const checksum256& m );
         static char hex_to_char( uint8_t hex );
   };

} /// namespace eosio
