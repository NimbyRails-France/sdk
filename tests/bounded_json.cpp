#include <nimby/detail/bounded_json.hpp>
#include <engine/train_editor_messages.h>
#include <runtime/mod_options_codec.h>
#include <nimby/detail/translations.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include "windows/allocation_fault_fixture.h"
using Json=nimby::detail::BoundedJson;
int main(){
    constexpr unsigned persistent=std::numeric_limits<unsigned>::max();
    const auto accepted=std::string(9,'[')+std::string(9,']');
    const auto rejected=std::string(10,'[')+std::string(10,']');
    {auto value=nimby::runtime::mod_options::codec::parse(accepted,1024);assert(value.is_array());}
    bool invalid=false;try{(void)nimby::runtime::mod_options::codec::parse(rejected,1024);}catch(const std::invalid_argument&){invalid=true;}assert(invalid);
    // Shape/type conversions used by codecs, including nested JSON scalars.
    const auto mixed=R"({"a":[null,true,false,-42,18446744073709551615,0.5,"s",{"nested":"v"}],"empty":[]})";
    {
        auto value=nimby::runtime::mod_options::codec::parse(mixed,2048);
        assert(value["a"][3].get<int>()==-42&&value["a"][4].get<uint64_t>()==UINT64_MAX);
        assert(value["a"][5].get<double>()==0.5&&value["a"][7]["nested"].get<std::string>()=="v");
        allocation_fault_fixture::arm(0,persistent);
    }
    auto cleanup=allocation_fault_fixture::stop();assert(cleanup.allocations==0&&cleanup.failures==0);
    {auto value=nimby::runtime::mod_options::codec::parse(accepted,1024);allocation_fault_fixture::arm(0,persistent);}
    cleanup=allocation_fault_fixture::stop();assert(cleanup.allocations==0);
    const auto sweep=[&](std::string_view text){
        allocation_fault_fixture::arm(SIZE_MAX,0);
        try{(void)nimby::runtime::mod_options::codec::parse(text,2048);}catch(const std::invalid_argument&){}
        auto total=allocation_fault_fixture::stop();
        for(size_t at=0;at<total.allocations;++at){
            allocation_fault_fixture::arm(at,persistent);bool threw=false;
            try{(void)nimby::runtime::mod_options::codec::parse(text,2048);}catch(const std::bad_alloc&){threw=true;}
            catch(const std::invalid_argument&){}
            const auto fault=allocation_fault_fixture::stop();assert(threw&&fault.failures);
        }
        return total.allocations;
    };
    const auto good=sweep(mixed),deep=sweep(rejected),bad=sweep(R"({"a":[{"b":123},)");
    const auto catalogue=R"({"fallback":"fr","languages":{"fr":{"title":"Exemple","length":"Longueur {m}"},"en":{"title":"Example","length":"Length {m}"}}})";
    {nimby::detail::Translations warm(catalogue);}
    allocation_fault_fixture::arm(SIZE_MAX,0);
    {nimby::detail::Translations value(catalogue);}
    const auto translationsTotal=allocation_fault_fixture::stop();
    for(size_t at=0;at<translationsTotal.allocations;++at){
        allocation_fault_fixture::arm(at,persistent);bool threw=false;
        try{nimby::detail::Translations value(catalogue);}catch(const std::bad_alloc&){threw=true;}
        const auto fault=allocation_fault_fixture::stop();assert(threw&&fault.failures);
    }
    const auto messages=nlohmann::json({{"messages",nlohmann::json::array({"Addition refused because this composition would exceed its permitted length.","Vehicle length is unavailable and this composition cannot be accepted.","Cannot verify this composition and the operation has been refused."})},{"translations",""}}).dump();
    {nimby::engine::train_editor::Messages warm(messages);}
    allocation_fault_fixture::arm(SIZE_MAX,0);
    {nimby::engine::train_editor::Messages value(messages);}
    const auto messagesTotal=allocation_fault_fixture::stop();
    for(size_t at=0;at<messagesTotal.allocations;++at){
        allocation_fault_fixture::arm(at,persistent);bool threw=false;
        try{nimby::engine::train_editor::Messages value(messages);}catch(const std::bad_alloc&){threw=true;}
        const auto fault=allocation_fault_fixture::stop();assert(threw&&fault.failures);
    }
    {nimby::engine::train_editor::Messages value(messages);allocation_fault_fixture::arm(0,persistent);}
    cleanup=allocation_fault_fixture::stop();assert(cleanup.allocations==0);
    // Generic DOM keeps iterative destruction: 50,000 nested arrays, including
    // mixed breadth. No bounded alias is used for this arbitrary-depth tree.
    {nlohmann::json generic=nullptr;for(size_t i=0;i<50000;++i)generic=nlohmann::json::array({std::move(generic),i});}
    std::cout<<"PASS bounded JSON: zero cleanup allocations under persistent OOM; "<<good+deep+bad+translationsTotal.allocations+messagesTotal.allocations<<" parse/Translations/Messages fault boundaries including partial/rejected DOM; generic depth50000 unchanged; sizeof="<<sizeof(Json)<<" align="<<alignof(Json)<<'\n';
}
