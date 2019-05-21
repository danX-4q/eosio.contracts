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
    auto itr_find_tx= txid_index.lower_bound(txid);
    auto itr_find_tx_up = txid_index.upper_bound(txid);
    for( ; itr_find_tx != itr_find_tx_up; ++itr_find_tx ) {
        if( itr_find_tx->txid == txid ) {
            break;
        }
    };
    eosio_assert( itr_find_tx == itr_find_tx_up, "error, txid has been pushed once." );

    tbl_cctx.emplace(get_self(), [&]( auto& row ) {
        row.id = tbl_cctx.available_primary_key();
        row.account = account;
        row.txid = txid;
        row.ccasset = ccasset;
        row.status = 0;
    });
}

void safeoracle::drawasset( checksum256 txid )
{
    DEBUG_PRINT_VAR( txid );
    ///////////////////////////////////////////////////////

    type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    auto txid_index = tbl_cctx.get_index<"txid"_n>();
    auto itr_find_tx = txid_index.lower_bound(txid);
    auto itr_find_tx_up = txid_index.upper_bound(txid);
    for( ; itr_find_tx != itr_find_tx_up; ++itr_find_tx ) {
        if( itr_find_tx->txid == txid ) {
            break;
        }
    };
    eosio_assert( itr_find_tx != itr_find_tx_up, "error, txid has not been pushed once." );

    auto id = itr_find_tx->id;
    auto to = itr_find_tx->account;
    auto ccasset = itr_find_tx->ccasset;

    eosio_assert( itr_find_tx->status == 0, "error, txid has not been drawed once." );
    require_auth( to ); //only [table]tbl_cctx.account can draw the asset!

    auto itr_find_id = tbl_cctx.find(id);
    tbl_cctx.modify(itr_find_id, get_self(), [&]( auto& row ) {
        row.status = 1;
    });

    //cast asset
    action(
        permission_level{"eosio.token"_n, "active"_n},
        "eosio.token"_n, "castcreate"_n,
        std::make_tuple( ccasset )
    ).send();

    /*
    //issue asset
    action(
        permission_level{"eosio"_n, "active"_n},
        "eosio.token"_n, "issue"_n,
        std::make_tuple( to, ccasset, string("issue by safeoracle::drawasset") )
    ).send();
    */
}

/*void safeoracle::test( checksum256 xtxid )
{
    DEBUG_PRINT_VAR( xtxid );
}*/

} /// namespace eosio

EOSIO_DISPATCH( eosio::safeoracle, (pushcctx)(drawasset) )
