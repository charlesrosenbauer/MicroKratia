all:
	eosio-cpp -o microkratia/microkratia.wasm src/main.cpp --abigen --contract microkratia

create:
	./${EOSPATH}cleos create account eosio microkratia EOS686nKZkrmrGof8f1CctuC2LARatZ4SMFtzpy5WRiWs1wu8C3br -p eosio@active

submit:
	./${EOSPATH}/cleos set contract microkratia microkratia -p microkratia@active

createCustom:
	./../../eos/build/bin/cleos create account eosio microkratia EOS686nKZkrmrGof8f1CctuC2LARatZ4SMFtzpy5WRiWs1wu8C3br -p eosio@active

submitCustom:
	./../../eos/build/bin/cleos set contract microkratia microkratia -p microkratia@active
