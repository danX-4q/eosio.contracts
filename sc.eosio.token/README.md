eosio.token
-----------

This eosio contract allows users to create, issue, and manage tokens on
eosio based blockchains.

This eosio contract also allows safe.oracle contract to create, issue
tokens automatically when someone transfers asset from safe-chain to
safecode-chain and then he/she draws the asset on safecode-chain.

For keeping permission safe, all eosio contract methods called by 
safe.oracle use new permission 'crosschain' whose parent is 'active'.
Before safe.oracle uses these methods, please run next commands:
* cleos set account permission eosio.token crosschain \
    '{"threshold": 1,"keys": [],"accounts": [{"permission":{"actor":"safe.oracle","permission":"eosio.code"},"weight":1}]}' active -p eosio.token
* cleos set account permission eosio crosschain \
    '{"threshold": 1,"keys": [],"accounts": [{"permission":{"actor":"safe.oracle","permission":"eosio.code"},"weight":1}]}' active -p eosio
* cleos set action permission eosio.token eosio.token castcreate crosschain -p 
eosio.token@active
* cleos set action permission eosio eosio.token castissue crosschain -p eosio@active
* cleos set action permission eosio eosio.token casttransfer crosschain -p eosio@active
