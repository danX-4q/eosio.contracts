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

void safeoracle::pushcctx( string txid, asset ccasset, name account )
{
    require_auth( get_self() );
    eosio_assert( txid.size() == 64, "txid is not 64 bytes" );
    DEBUG_PRINT_VAR( txid );

    ///////////////////////////////////////////////////////

    checksum256 xtxid = sha256(txid.c_str(), txid.size());
    DEBUG_PRINT_VAR( xtxid );

    type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    auto xtxid_index = tbl_cctx.get_index<"xtxid"_n>();
    auto itr_find_xtx = xtxid_index.lower_bound(xtxid);
    eosio_assert( itr_find_xtx == xtxid_index.end(), "error, txid has been pushed once." );

    tbl_cctx.emplace(get_self(), [&]( auto& row ) {
        row.id = tbl_cctx.available_primary_key();
        row.account = account;
        row.txid = txid;
        row.xtxid = xtxid;
        row.ccasset = ccasset;
        row.status = 0;
    });
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::safeoracle, (pushcctx) )
