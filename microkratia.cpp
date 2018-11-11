#include <eosiolib/eosio.hpp>

using namespace eosio;

// Smart Contract Name: microkratia
// Table struct:
//   issuestruct: multi index table to store the issues
//     prim_key(uint64): primary key
//     user(name): account name for the user
//     issue(string): the issue message
//     timestamp(uint64): the store the last update block time
// Public method:
//   isnewuser => to check if the given account name has issue in table or not
// Public actions:
//   update => put the issue into the multi-index table and sign by the given account

// Replace the contract class name when you start your own project
CONTRACT microkratia : public eosio::contract {
  private:
    bool isnewuser( name user ) {
      // get issues by using secordary key
      auto issue_index = _issues.get_index<name("getbyuser")>();
      auto issue_iterator = issue_index.find(user.value);

      return issue_iterator == issue_index.end();
    }

    TABLE issuestruct {
      uint64_t      prim_key;  // primary key
      name          user;      // account name for the user
      std::string   issue;      // the issue message
      uint64_t      timestamp; // the store the last update block time

      // primary key
      auto primary_key() const { return prim_key; }
      // secondary key
      // only supports uint64_t, uint128_t, uint256_t, double or long double
      uint64_t get_by_user() const { return user.value; }
    };

    // create a multi-index table and support secondary key
    typedef eosio::multi_index< name("issuestruct"), issuestruct,
      indexed_by< name("getbyuser"), const_mem_fun<issuestruct, uint64_t, &issuestruct::get_by_user> >
      > issue_table;

    issue_table _issues;

  public:
    using contract::contract;

    // constructor
    issuechain( name receiver, name code, datastream<const char*> ds ):
                contract( receiver, code, ds ),
                _issues( receiver, receiver.value ) {}

    ACTION update( name user, std::string& issue ) {
      // to sign the action with the given account
      require_auth( user );

      // create new / update issue depends whether the user account exist or not
      if (isnewuser(user)) {
        // insert new issue
        _issues.emplace( _self, [&]( auto& new_user ) {
          new_user.prim_key    = _issue.available_primary_key();
          new_user.user        = user;
          new_user.issue        = issue;
          new_user.timestamp   = now();
        });
      } else {
        // get object by secordary key
        auto issue_index = _issues.get_index<name("getbyuser")>();
        auto &issue_entry = issue_index.get(user.value);
        // update existing issue
        _issues.modify( issue_entry, _self, [&]( auto& modified_user ) {
          modified_user.issue      = issue;
          modified_user.timestamp = now();
        });
      }
    }

};

// specify the contract name, and export a public action: update
EOSIO_DISPATCH( microkratia, (update) )
