/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <safe.chain/safe.chain.hpp>

namespace eosio {

safechain::safechain(name receiver, name code,  datastream<const char*> ds): 
    contract(receiver, code, ds)
{
    DEBUG_PRINT_VAR(receiver);
    DEBUG_PRINT_VAR(code);
    DEBUG_PRINT_VAR(this->get_self());
}

void safechain::feed( string   txid )
{
    //require_auth( _self );
    eosio_assert( txid.size() == 64, "txid is not 64 bytes" );
    eosio::print(txid.c_str()); eosio::print('\n');

    uint64_t confirmations = 0;
    int ret = get_txid_confirmations(txid.c_str(), txid.size(), confirmations);
    eosio::print(ret); eosio::print('\n');
    if(!ret){
        eosio::print(confirmations); eosio::print('\n');
    } else {
        eosio::print("error when call get_txid_confirmations"); eosio::print('\n');
    }
}

void safechain::feed2( name r_account, string txchain, string txid )
{
    //require_auth( r_account );
    eosio_assert( txid.size() == 64, "txid is not 64 bytes" );
    DEBUG_PRINT_VAR( txid );

    uint64_t confirmations = 0;
    int ret = get_txid_confirmations(txid.c_str(), txid.size(), confirmations);
    DEBUG_PRINT_VAR( ret );
    DEBUG_PRINT_VAR( confirmations );
    eosio_assert( ret == 0, "error when call get_txid_confirmations" );

    ///////////////////////////////////////////////////////

    checksum256 xtxid = sha256(txid.c_str(), txid.size());
    type_table__receive tbl_receive(get_self(), "global"_n.value);
    auto xtxid_index = tbl_receive.get_index<"xtxid"_n>();
    auto itr_find_xtx = xtxid_index.lower_bound(xtxid);
    eosio_assert( itr_find_xtx == xtxid_index.end(), "error, txid has been recerved once." );

    tbl_receive.emplace(get_self(), [&]( auto& row ) {
        row.id = tbl_receive.available_primary_key();
        row.receiver = r_account;
        row.txchain = txchain;
        row.xtxid = xtxid;
        row.txid = txid;
    });
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::safechain, (feed)(feed2) )
