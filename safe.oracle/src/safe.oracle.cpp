/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <safe.oracle/safe.oracle.hpp>

namespace eosio {

uint32_t safeoracle::dft__last_safed_block_num = 5453;

safeoracle::safeoracle(name receiver, name code,  datastream<const char*> ds): 
    contract(receiver, code, ds)
{
    DEBUG_PRINT_VAR(receiver);
    DEBUG_PRINT_VAR(code);
    DEBUG_PRINT_VAR(this->get_self());

    type_table__globalkv    tbl_globalkv(_code, _code.value);
    bool                    is_inited = tbl_globalkv.exists();
    if (!is_inited) {
        //首次，需初始化
        init_globalkv(tbl_globalkv);
    } else {
    }
}

void safeoracle::init_globalkv( type_table__globalkv &tbl_globalkv )
{
    globalkv    dlt {
        .block_num = dft__last_safed_block_num,
        .tx_index = 0
    };
    //dlt.print();
    tbl_globalkv.set(dlt, _code);
}

void safeoracle::pushcctxes( struct chain_pos curpos, struct chain_pos nextpos, const std::vector< struct cctx_info >& cctxes )
{
    require_auth( get_self() );
    DEBUG_PRINT_VAR( curpos );
    DEBUG_PRINT_VAR( nextpos );

    ///////////////////////////////////////////////////////

    eosio_assert(
        (
            nextpos.block_num > curpos.block_num ||
            ( nextpos.block_num == curpos.block_num && nextpos.tx_index > curpos.tx_index )
        ),
        "error, nextpos must > curpos."
    );

    type_table__globalkv    tbl_globalkv(_code, _code.value);
    globalkv    dlt = tbl_globalkv.get();
    eosio_assert(
        dlt.block_num == curpos.block_num && dlt.tx_index == curpos.tx_index,
        "error, tbl_globalkv(which `curpos` has read chain-position info) has been changed, aborting."
    );

    ///////////////////////////////////////////////////////

    if ( cctxes.size() > 0 ) {
        type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    
        for( auto & txinfo: cctxes ) {
            push_each_cctx( tbl_cctx, txinfo );
        }
    }

    //finally, update tbl_globalkv's chain-position info
    dlt.block_num   = nextpos.block_num;
    dlt.tx_index    = nextpos.tx_index;
    tbl_globalkv.set( dlt, _code );
}

void safeoracle::push_each_cctx( type_table__cctx& tbl_cctx, const cctx_info& txinfo )
{
    auto txid       = txinfo.txid;
    auto quantity   = txinfo.quantity;
    auto account    = txinfo.account;

    DEBUG_PRINT_VAR(txid);

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
        row.id      = tbl_cctx.available_primary_key();
        row.account = account;
        row.txid    = txid;
        row.quantity= quantity;
        row.status  = 0;
    });
}

void safeoracle::drawassets( const std::vector< checksum256 >& txids )
{
    eosio_assert( txids.size() > 0, "error, there is no txids to be deal with." );

    type_table__cctx tbl_cctx(get_self(), "global"_n.value);
    for( auto & txid: txids ) {
        draw_each_asset( tbl_cctx, txid );
    }
}

void safeoracle::draw_each_asset( type_table__cctx& tbl_cctx, const checksum256 txid )
{
    DEBUG_PRINT_VAR( txid );
    ///////////////////////////////////////////////////////

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
    auto quantity = itr_find_tx->quantity;

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
        std::make_tuple( quantity )
    ).send();

    //issue asset
    auto stxid = checksum256_to_string(txid); DEBUG_PRINT_VAR( stxid );
    action(
        permission_level{"eosio"_n, "crosschain"_n},
        "eosio.token"_n, "castissue"_n,
        std::make_tuple( to, quantity, 
                         string("associated with ") + stxid + "@SAFE; issued by safeoracle::drawassets automatically")
    ).send();
    
}

void safeoracle::rstchainpos()
{
    type_table__globalkv    tbl_globalkv(_code, _code.value);
    globalkv    dlt {
        .block_num = dft__last_safed_block_num,
        .tx_index = 0
    };
    //dlt.print();
    tbl_globalkv.set(dlt, _code);
}

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

EOSIO_DISPATCH( eosio::safeoracle, (pushcctxes)(drawassets)(rstchainpos) )
