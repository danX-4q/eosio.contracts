/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <safe.oracle/safe.oracle.hpp>

namespace eosio {

uint32_t safeoracle::dft__last_safed_block_num = 0;

safeoracle::safeoracle(name receiver, name code,  datastream<const char*> ds): 
    contract(receiver, code, ds)
{
    DEBUG_PRINT_VAR(receiver);
    DEBUG_PRINT_VAR(code);
    DEBUG_PRINT_VAR(this->get_self());

    type_table__globalkv    tbl_globalkv(code, code.value);
    bool                    is_inited = tbl_globalkv.exists();
    if (!is_inited) {
        //首次，需初始化
        init_globalkv(tbl_globalkv);
    } else {
    }
}

void safeoracle::init_globalkv(type_table__globalkv &tbl_globalkv)
{
    globalkv    dlt {
        .last_safed_block_num = dft__last_safed_block_num
    };
    //dlt.print();
    tbl_globalkv.set(dlt, _code);
}

void safeoracle::updatelbn( uint32_t lbn )
{
    type_table__globalkv    tbl_globalkv(_code, _code.value);
    globalkv                gkv = tbl_globalkv.get();
    
    eosio_assert( lbn > gkv.last_safed_block_num , "error, lbn must be greater then last_safed_block_num." );
    gkv.last_safed_block_num = lbn;
    tbl_globalkv.set(gkv, _code);
}

void safeoracle::resetlbn()
{
    type_table__globalkv    tbl_globalkv(_code, _code.value);
    globalkv    dlt {
        .last_safed_block_num = dft__last_safed_block_num
    };
    tbl_globalkv.set(dlt, _code);
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
        permission_level{"eosio.token"_n, "crosschain"_n},
        "eosio.token"_n, "castcreate"_n,
        std::make_tuple( ccasset )
    ).send();

    //issue asset
    auto stxid = checksum256_to_string(txid); DEBUG_PRINT_VAR( stxid );
    action(
        permission_level{"eosio"_n, "crosschain"_n},
        "eosio.token"_n, "castissue"_n,
        std::make_tuple( to, ccasset, 
                         string("associated with ") + stxid + "#SAFE; issued by safeoracle::drawasset automatically")
    ).send();
    
}

/*void safeoracle::test( checksum256 xtxid )
{
    DEBUG_PRINT_VAR( xtxid );
}*/

char safeoracle::hex_to_char( uint8_t hex )
{
    //the caller make sure 0 <= hex <= 15
    if ( hex <= 9 ) {
        return ( '0' + hex );
    } else if ( hex <= 15 )
    {
        return ( 'a' + hex - 10 );
    } else {
        return ( '?' );
    }
}

string safeoracle::checksum256_to_string(const checksum256& m)
{
    auto arr = m.extract_as_byte_array();    //std::array<uint8_t, Size>
    string s = "";
    for ( auto itr = arr.begin(); itr != arr.end(); ++itr ) {
        uint8_t temp = *itr;
        char    hi = hex_to_char( (temp & 0xf0)>>4 );
        s.push_back( hi );
        char    lo = hex_to_char( (temp & 0x0f) );
        s.push_back( lo );
    }
    return ( s );
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::safeoracle, (updatelbn)(resetlbn)(pushcctx)(drawasset) )
