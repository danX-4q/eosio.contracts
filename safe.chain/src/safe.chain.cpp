/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <safe.chain/safe.chain.hpp>

namespace eosio {

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

} /// namespace eosio

EOSIO_DISPATCH( eosio::safechain, (feed) )
