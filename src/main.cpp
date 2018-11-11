
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

struct AppUnit{
  std::string username;
  std::string contact;
  name        id;
  uint64_t    timestamp;
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

struct PendRuleUnit{
  std::string description;
  uint64_t author;

  uint64_t voteKey;
};

struct FundUnit{
  uint64_t account;

  uint64_t voteKey;
};

struct VoteUnit{
  uint64_t voter;
  uint64_t nextKey;
};

enum UnitType{
  MEMBER=0, APP=1, ISSUE=2, RULE=3, PENDRULE=4, FUND=5, VOTE=6
};

union UnitUnion{
  MemberUnit   member;
  AppUnit      app;
  IssueUnit    issue;
  RuleUnit     rule;
  PendRuleUnit pend;
  FundUnit     fund;
  VoteUnit     vote;
};


struct Unit{
  UnitType type;

  // I was going to use a tagged union here. EOS-CPP doesn't seem to like them
  // though. So we're going even more insane. Unsafe casts and byte arrays!
  char bytes[sizeof(UnitUnion)];
};






class [[eosio::contract]] microkratia : public contract {
  public:
      using contract::contract;

      microkratia(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds),_table(receiver, receiver.value) {}

      [[eosio::action]]
      void apply( name user, std::string username, std::string contact ) {
         require_auth(user);
         //eosio_assert(memberAppTable.find(user.value) != memberAppTable.end(), "You have already applied for a membership.");
         //eosio_assert(_table.   find(user.value) != _table.   end(), "You already have a membership.");

         //_table.emplace(_self, [&](auto& application) {
        //   application.username  = username;
           //application.name      = user;
           //application.timestamp = current_time();
           //application.contact   = contact;
         //});
      }

      [[eosio::action]]
      void approve( name gatekeeper, name user, name application ){
        require_auth(gatekeeper);
        //auto app = memberAppTable.find(user.value);
        //eosio_assert(app != memberAppTable.end(), "Member is not in the application queue.");

        // Do stuff here
      }

      [[eosio::action]]
      void printapps(){
        for (auto& app: _table) {
          if(app.data.type == APP){
            AppUnit x = *(AppUnit*)(&app.data.bytes);
            eosio::print("{\n  Name: ", x.username, "\n  Key: ", app.key, "\n  Contact Details: ", x.contact, "\n}\n");
          }
        }
      }

    private:
      struct [[eosio::table]] TableCell{
        name key;
        Unit data;

        uint64_t primary_key() const {return key.value;};
      };

      typedef eosio::multi_index<"cells"_n, TableCell> Table;

      Table _table;

      MemberUnit* lookupMember(Table t, uint64_t index){
        index = (index * 8) + 0;
        if (t.find(index) == t.end())
          return nullptr;
        return (MemberUnit*)(&t.get(index).data.bytes);
      }

      AppUnit* lookupApp(Table t, uint64_t index){
        index = (index * 8) + 1;
        if (t.find(index) == t.end())
          return nullptr;
        return (AppUnit*)(&t.get(index).data.bytes);
      }

      IssueUnit* lookupIssue(Table t, uint64_t index){
        index = (index * 8) + 2;
        if (t.find(index) == t.end())
          return nullptr;
        return (IssueUnit*)(&t.get(index).data.bytes);
      }

      RuleUnit* lookupRule(Table t, uint64_t index){
        index = (index * 8) + 3;
        if (t.find(index) == t.end())
          return nullptr;
        return (RuleUnit*)(&t.get(index).data.bytes);
      }

      PendRuleUnit* lookupPend(Table t, uint64_t index){
        index = (index * 8) + 4;
        if (t.find(index) == t.end())
          return nullptr;
        return (PendRuleUnit*)(&t.get(index).data.bytes);
      }

      FundUnit* lookupFund(Table t, uint64_t index){
        index = (index * 8) + 5;
        if (t.find(index) == t.end())
          return nullptr;
        return (FundUnit*)(&t.get(index).data.bytes);
      }

      VoteUnit* lookupVote(Table t, uint64_t index){
        index = (index * 8) + 6;
        if (t.find(index) == t.end())
          return nullptr;
        return (VoteUnit*)(&t.get(index).data.bytes);
      }

      void tableInsert(Table t, void* data, uint64_t index, UnitType ty, name payer){
        if (t.find(index) == t.end()){
          t.emplace(payer, [&](auto& address){
            for(int i = 0; i < sizeof(UnitUnion); i++)
              address.data.bytes[i] = ((char*)data)[i];
          });
        }
      }
};
EOSIO_DISPATCH( microkratia, (apply)(approve)(printapps))
