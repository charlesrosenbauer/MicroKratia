
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>

//#include "member.hpp"




using namespace eosio;


/*
  I was told that I need to have multiple accounts in order to get multiple
  different databases in a single dapp. Screw that, I'm tired. I'm sick of
  wasting hours dealing with technical difficulties. I'm not learning that. I
  know tagged unions and crazy hacks well enough that I'll have an easier time
  doing that here. Minimize risk. We're stuffing everything into one database.
  EVERYTHING.
*/
struct MemberUnit{
  std::string username;
  std::string contact;
  name        id;
  int         stake;
};

struct IssueUnit{
  std::string description;
  uint64_t    rulebreak;
  name        accused;
  name        accuser;
};

struct RuleUnit{
  std::string description;
  uint64_t author;
};


enum UnitType{
  MEMBER=0, ISSUE=1, RULE=2
};

union UnitUnion{
  MemberUnit   member;
  IssueUnit    issue;
  RuleUnit     rule;
};






class [[eosio::contract]] microkratia : public contract {
  public:
      using contract::contract;

      microkratia(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds),_table(receiver, receiver.value) {}

      [[eosio::action]]
      void apply( name user, std::string username, std::string contact ) {
         require_auth(user);
         eosio_assert(lookupMember(user.value) != nullptr, "You have already applied for a membership.");

         auto member = MemberUnit{username, contact, user, 0};
         tableInsert(&member, user.value, MEMBER, user);
      }




    private:
      struct [[eosio::table]] TableCell{
        name key;

        UnitType type;
        char bytes[sizeof(UnitUnion)];

        uint64_t primary_key() const {return key.value;};
      };

      typedef eosio::multi_index<"cells"_n, TableCell> Table;

      Table _table;

      MemberUnit* lookupMember(uint64_t index){
        if (_table.find(index) == _table.end())
          return nullptr;
        if(_table.get(index).type != MEMBER)
          return nullptr;
        return (MemberUnit*)(&_table.get(index).bytes);
      }

      void tableInsert(void* data, uint64_t index, UnitType ty, name payer){
        if (_table.find(index) == _table.end()){
          _table.emplace(payer, [&](auto& address){
            for(int i = 0; i < sizeof(UnitUnion); i++)
              address.bytes[i] = ((char*)data)[i];
          });
        }
      }
};
EOSIO_DISPATCH( microkratia, (apply))
