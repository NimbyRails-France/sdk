// Production client with deterministic native-read/transport doubles. Compare
// work performed by the old per-signal API and the live batch publication.
#include <platform/texture_connection.h>
#include <engine/signal_textures.h>
#include <runtime/texture_publications.h>
#include <runtime/texture_publication_diagnostics.h>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
int opens=0,catalogs=0,memberships=0,publishes=0,submits=0,statusBatches=0,forwards=0;
uint64_t missing=0;bool changedWorld=false;uint32_t target=0,lastOperation=0;
nimby::engine::LiveState world{1,2,3,4,5};
nimby::texture_bridge::Table table;
void failureDiagnostics(){
    using nimby::runtime::TexturePublicationDiagnostics;
    using Reason=nimby::runtime::TexturePublicationReason;
    using namespace std::chrono_literals;
    TexturePublicationDiagnostics diagnostic;
    const auto now=TexturePublicationDiagnostics::Clock::now();
    NimbyTextureUpdate row{};row.signal=0x8000000000001;row.duration_ms=2500;
    std::strcpy(row.texture_set,"test");std::strcpy(row.first_path,"a.svg");
    CHECK(nimby::runtime::invalidTextureUpdate(row)==Reason::None);
    auto malformed=row;std::memset(malformed.first_path,'x',sizeof malformed.first_path);
    CHECK(nimby::runtime::invalidTextureUpdate(malformed)==Reason::FirstPathUnterminated);
    malformed=row;malformed.signal=1;CHECK(nimby::runtime::invalidTextureUpdate(malformed)==Reason::InvalidSignal);
    malformed=row;malformed.flags=2;CHECK(nimby::runtime::invalidTextureUpdate(malformed)==Reason::InvalidFlags);
    malformed=row;malformed.duration_ms=999;CHECK(nimby::runtime::invalidTextureUpdate(malformed)==Reason::InvalidDuration);
    malformed=row;malformed.half_period_ms=500;CHECK(nimby::runtime::invalidTextureUpdate(malformed)==Reason::InvalidAnimation);
    auto after=world;++after.database;
    TexturePublicationDiagnostics::Context context{42,7,46,45,&row,&world,&after};
    unsigned reports=0;std::string message;
    const auto sink=[&](const char* text) noexcept {++reports;message=text;};
    diagnostic.report(NIMBY_OK,Reason::None,context,sink,now);CHECK(reports==0);
    diagnostic.report(NIMBY_INVALID_ARGUMENT,Reason::SignalMissing,context,sink,now);
    CHECK(reports==1&&message.find("status=1 reason=signal_missing targetPid=42 owner=7 count=46 row=45 signal=2251799813685249")!=std::string::npos);
    CHECK(message.find("textureSet=\"test\" first=\"a.svg\"")!=std::string::npos);
    CHECK(message.find("root=2 database=3 copy=4 simulation=5 afterRoot=2 afterDatabase=4 afterCopy=4 afterSimulation=5")!=std::string::npos);
    for(unsigned i=0;i<1000;++i){after.database=i;diagnostic.report(NIMBY_INVALID_ARGUMENT,Reason::SignalMissing,context,sink,now+1ms);}
    CHECK(reports==1); // Changing world counters do not trigger repeated disk writes.
    diagnostic.report(NIMBY_DATA_UNAVAILABLE,Reason::WorldChanged,context,sink,now+2ms);
    CHECK(reports==2&&message.find("suppressed=1000")!=std::string::npos);
    for(unsigned i=0;i<1000;++i){++row.signal;diagnostic.report(NIMBY_DATA_UNAVAILABLE,Reason::WorldChanged,context,sink,now+3ms);}
    CHECK(reports==4); // Changing signal IDs cannot defeat the burst cap.
    diagnostic.report(NIMBY_DATA_UNAVAILABLE,Reason::WorldChanged,context,sink,now+5s);
    CHECK(reports==5&&message.find("suppressed=998")!=std::string::npos);
    // Logging malformed ABI fields stays within fixed buffers and one line.
    TexturePublicationDiagnostics malformedDiagnostic;
    std::memset(row.texture_set,'s',sizeof row.texture_set);
    std::memset(row.first_path,'x',sizeof row.first_path);row.first_path[1]='\n';row.first_path[2]='"';
    std::memset(row.alternate_path,'a',sizeof row.alternate_path);
    malformedDiagnostic.report(NIMBY_INVALID_ARGUMENT,Reason::FirstPathUnterminated,context,sink,now);
    CHECK(message.size()<1536&&message.find('\n')==std::string::npos&&message.find("first=\"x??")!=std::string::npos);
    CHECK(message.find("suppressed=0")!=std::string::npos);
}
}
namespace nimby::platform {
struct TextureConnection::Impl{};
TextureConnection::TextureConnection():impl_(std::make_unique<Impl>()){}
TextureConnection::~TextureConnection()=default;
uint32_t TextureConnection::open(uint32_t){++opens;return NIMBY_OK;}
const engine::LiveState& TextureConnection::live()const{return world;}
bool TextureConnection::read(void*,uint64_t,void*,size_t){return false;}
uint32_t TextureConnection::submit(uint32_t,uint64_t,uint64_t,uint64_t,uint32_t,uint32_t,uint32_t){++submits;return NIMBY_OK;}
uint32_t TextureConnection::publish(uint64_t owner,std::span<const texture_bridge::Command> rows){++publishes;return texture_bridge::publishTextures(table,owner,rows,100);}
uint32_t TextureConnection::clear(uint64_t owner,std::span<const uint64_t> ids){return texture_bridge::clearTextures(table,owner,ids);}
uint32_t TextureConnection::release(uint64_t owner){texture_bridge::releaseTextures(table,owner);return NIMBY_OK;}
uint32_t TextureConnection::releaseInGameOwner(uint64_t owner){texture_bridge::releaseTextures(table,owner);return NIMBY_OK;}
uint32_t TextureConnection::statuses(std::span<const uint64_t> ids,NimbySignalTextureOverrideStatus* out){++statusBatches;for(size_t i=0;i<ids.size();++i){out[i]={};out[i].struct_size=sizeof(out[i]);}return NIMBY_OK;}
void TextureConnection::previewStatus(NimbyTexturePreviewStatus&)const{}
void TextureConnection::overrideStatus(NimbySignalTextureOverrideStatus&)const{}
uint64_t TextureConnection::now(){return 100;}
uint32_t TextureConnection::currentPid(){return 42;}
uint32_t TextureConnection::hostTarget(){return target;}
uint32_t TextureConnection::forward(uint32_t operation,uint32_t pid,const void*,size_t,uint64_t,std::span<uint8_t>,uint32_t& written){
    ++forwards;lastOperation=operation;written=0;return !pid||pid==target?NIMBY_OK:NIMBY_INVALID_ARGUMENT;
}
}
namespace nimby::engine {
bool read_signal_texture_sets(ReadMemory,void*,const LiveState&,bool,std::span<const std::string_view> names,SignalTextureCatalog& out)noexcept{
    if(names.size()!=1||names.front()!="test")return false;
    ++catalogs;out={};SignalTextureSet set;set.hash=123;set.name="test";
    set.files={{1,1,"mod","a.svg"},{1,2,"mod","b.svg"}};out.sets.emplace(123,std::move(set));return true;
}
bool read_signal_membership(ReadMemory,void*,const LiveState&,bool,uint64_t signal,bool& found)noexcept{++memberships;found=signal!=missing;return true;}
bool resolve_live_state(ReadMemory,void*,uint64_t,bool,LiveStateProfile,LiveState& out)noexcept{out=world;if(changedWorld)++out.database;return true;}
bool resolve_live_state(ReadMemory read,void* context,uint64_t base,bool recognized,LiveState& out)noexcept{return resolve_live_state(read,context,base,recognized,world.profile,out);}
}
int main(){try{
    failureDiagnostics();
    constexpr uint64_t first=0x8000000000001;
    std::vector<NimbyTextureUpdate> rows(46);
    for(size_t i=0;i<rows.size();++i){auto& row=rows[i];row.signal=first+i;row.duration_ms=2500;
        std::strcpy(row.texture_set,"test");std::strcpy(row.first_path,"a.svg");
        CHECK(NimbyInternal_ShowSignalTextureFor(42,row.signal,"test","a.svg",2500)==NIMBY_OK);
    }
    CHECK(opens==46&&catalogs==46&&memberships==46&&submits==46);
    opens=catalogs=memberships=submits=0;
    CHECK(NimbyInternal_PublishTextureUpdates(42,1,rows.data(),46)==NIMBY_OK);
    CHECK(opens==1&&catalogs==1&&memberships==46&&publishes==1&&submits==0&&table.count()==46);
    const auto before=publishes;missing=rows.back().signal;
    std::strcpy(rows.front().first_path,"b.svg");
    CHECK(NimbyInternal_PublishTextureUpdates(42,1,rows.data(),46)==NIMBY_INVALID_ARGUMENT&&publishes==before);
    CHECK(table.entries[0].index==0);missing=0;changedWorld=true;
    CHECK(NimbyInternal_PublishTextureUpdates(42,1,rows.data(),46)==NIMBY_DATA_UNAVAILABLE&&publishes==before);
    changedWorld=false;
    auto bad=rows.front();std::memset(bad.first_path,'x',sizeof bad.first_path);
    const auto opened=opens;
    CHECK(NimbyInternal_PublishTextureUpdates(42,1,&bad,1)==NIMBY_INVALID_ARGUMENT&&opens==opened);
    CHECK(NimbyInternal_PublishTextureUpdates(42,1,rows.data(),4097)==NIMBY_INVALID_ARGUMENT&&opens==opened);
    std::array<uint64_t,32> ids{};std::array<NimbySignalTextureOverrideStatus,32> statuses{};
    for(size_t i=0;i<32;++i){ids[i]=first+i;statuses[i].struct_size=sizeof statuses[i];}
    CHECK(NimbyInternal_SignalTextureOverrideStatuses(42,ids.data(),32,statuses.data())==NIMBY_OK&&statusBatches==1&&submits==0);
    target=42;const auto catalogBefore=catalogs;
    CHECK(NimbyInternal_PublishTextureUpdates(0,999,rows.data(),46)==NIMBY_OK&&forwards==1&&lastOperation==300&&catalogs==catalogBefore);
    CHECK(NimbyInternal_ClearOwnedTextures(0,999,ids.data(),32)==NIMBY_OK&&lastOperation==301);
    CHECK(NimbyInternal_ReleaseTextureOwner(0,999)==NIMBY_OK&&lastOperation==302);
    std::cout<<"PASS 46 signals: catalogue reads 46 -> 1, publications 46 -> 1; statuses32 -> one call, atomic failures and child routing\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
