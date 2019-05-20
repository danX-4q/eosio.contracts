/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <safe.oracle/safe.oracle.hpp>

namespace eosio {

safeoracle::safeoracle(name receiver, name code,  datastream<const char*> ds): 
    contract(receiver, code, ds)
{
    DEBUG_PRINT_VAR(receiver);
    DEBUG_PRINT_VAR(code);
    DEBUG_PRINT_VAR(this->get_self());
}

void safeoracle::pushcctx( checksum256 txid, asset ccasset, name account )
{
    require_auth( get_self() );
    ///////////////////////////////////////////////////////
    DEBUG_PRINT_VAR( txid );

    type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    auto txid_index = tbl_cctx.get_index<"txid"_n>();
    auto itr_find_tx = txid_index.lower_bound(txid);
    eosio_assert( itr_find_tx == txid_index.end(), "error, txid has been pushed once." );

    tbl_cctx.emplace(get_self(), [&]( auto& row ) {
        row.id = tbl_cctx.available_primary_key();
        row.account = account;
        row.txid = txid;
        row.ccasset = ccasset;
        row.status = 0;
    });
}

void safeoracle::drawasset( checksum256 txid, name account )
{
    require_auth( account );
    ///////////////////////////////////////////////////////
    DEBUG_PRINT_VAR( txid );
    DEBUG_PRINT_VAR( account );

    type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    auto txid_index = tbl_cctx.get_index<"txid"_n>();
    auto itr_find_tx = txid_index.lower_bound(txid);
    eosio_assert( itr_find_tx != txid_index.end(), "error, txid has not been pushed once." );

    auto id = itr_find_tx->id;
    eosio_assert( itr_find_tx->status == 0, "error, txid has not been drawed once." );

    auto itr_find_id = tbl_cctx.find(id);
    tbl_cctx.modify(itr_find_id, get_self(), [&]( auto& row ) {
        row.status = 1;
    });

    //then cast asset!
}

/*void safeoracle::test( checksum256 xtxid )
{
    DEBUG_PRINT_VAR( xtxid );
}*/

} /// namespace eosio

EOSIO_DISPATCH( eosio::safeoracle, (pushcctx)(drawasset) )
