#include "engine/detail/train_metadata.h"
#include "engine/detail/train_composition.h"
#include <cstdio>
#include <map>
#include <chrono>
#include <string_view>
using namespace nimby::engine;
namespace {
constexpr uint64_t trainId=0x5000000000001,lineId=0x4000000000001,childId=0x4000000010001;
constexpr uint64_t model=0x300000000,motion=0x300010000,line=0x300020000;
constexpr uint64_t catalog=0x300030000,heads=0x300040000,nodes=0x300050000,refs=0x300060000;
constexpr uint64_t modelHeads=0x400000000,modelNodes=0x400010000,configuredCars=0x400020000,currentCars=0x400030000;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    std::map<std::pair<uint64_t,size_t>,size_t> calls;
    uint64_t fail=0,change=0;size_t changeSize=0,changeCall=0,changeByte=0;uint8_t changeMask=1;bool persist=false;
    uint64_t mutateOnRead=0,mutateBase=0;size_t mutateCall=0,mutateOffset=0;
    size_t reads=0,bytes=0;
    template<class T> void put(uint64_t base,size_t offset,T value){
        auto& storage=regions[base];if(storage.size()<offset+sizeof value)storage.resize(offset+sizeof value);
        std::memcpy(storage.data()+offset,&value,sizeof value);
    }
    void name(uint64_t base,size_t offset,std::string_view value){
        auto& storage=regions[base];if(storage.size()<offset+32)storage.resize(offset+32);
        std::memcpy(storage.data()+offset,value.data(),value.size());
        put(base,offset+16,uint64_t(value.size()));put(base,offset+24,uint64_t(15));
    }
    void reset(){reads=bytes=0;calls.clear();}
};
bool read(void* context,uint64_t address,void* output,size_t size){
    auto& memory=*static_cast<Memory*>(context);++memory.reads;memory.bytes+=size;
    const auto count=++memory.calls[{address,size}];
    if(address==memory.fail)return false;
    auto found=memory.regions.upper_bound(address);if(found==memory.regions.begin())return false;--found;
    if(address-found->first>found->second.size()||size>found->second.size()-(address-found->first))return false;
    std::memcpy(output,found->second.data()+address-found->first,size);
    if(address==memory.change&&size==memory.changeSize&&count==memory.changeCall&&memory.changeByte<size){
        static_cast<unsigned char*>(output)[memory.changeByte]^=memory.changeMask;
        if(memory.persist)found->second[address-found->first+memory.changeByte]^=memory.changeMask;
    }
    if(address==memory.mutateOnRead&&count==memory.mutateCall)memory.regions.at(memory.mutateBase)[memory.mutateOffset]^=1;
    return true;
}
LiveState world(){return {0x140000000,0x200000000,0x200010000,0x200020000,0x200030000};}
void pool(Memory& memory,uint64_t address,uint64_t block,size_t stride,uint64_t id){
    memory.regions[address].resize(48);memory.put(address,4,uint32_t(1));memory.put(address,8,uint32_t(2));memory.put(address,16,uint32_t(1));
    memory.put(address,24,block-0x1000);memory.put(address,32,block-0x1000+8);memory.put(address,40,block-0x1000+8);
    memory.put(block-0x1000,0,block);memory.regions[block].resize(stride*2);memory.put(block,0,id);
}
void vector(Memory& memory,uint64_t address,size_t offset,uint64_t data,std::initializer_list<uint64_t> tags){
    memory.put(address,offset,data);memory.put(address,offset+8,data+tags.size()*8);memory.put(address,offset+16,data+tags.size()*8);
    size_t n=0;for(auto tag:tags)memory.put(data,n++*8,tag);
}
Memory fixture(){
    auto state=world();Memory m;
    m.put(state.module_base+0xb81998,0,state.root);m.put(state.root,0x540,state.database);
    m.put(state.root,0x5c0,state.copy);m.put(state.root,0x680,state.simulation);
    m.put(state.simulation+0x20,0,int64_t(1577836800));m.put(state.simulation+0x28,0,int64_t(10000));
    pool(m,state.database+0x200,model,0x178,trainId);m.name(model,0x10,"Train");
    m.put(model,0xc0,refs+0x500);m.put(model,0xc8,refs+0x500+3*32);m.put(model,0xd0,refs+0x500+3*32);
    m.put(model,0xdc,45.f);m.put(model,0xe0,.8f);m.put(model,0xec,160000.f);m.put(model,0xf0,1900000.f);
    m.put(model,0xf4,120000.f);m.put(model,0xf8,72.f);m.put(model,0x100,int32_t(320));
    pool(m,state.simulation+0xa0,motion,0x638,trainId);
    m.put(motion,8,refs+0x600);m.put(motion,0x10,refs+0x600+2*32);m.put(motion,0x18,refs+0x600+2*32);
    m.put(motion,0x24,30.f);m.put(motion,0x28,.5f);m.put(motion,0x34,100000.f);m.put(motion,0x38,1200000.f);
    m.put(motion,0x3c,80000.f);m.put(motion,0x40,48.f);m.put(motion,0x48,int32_t(210));
    m.put(motion,0x4b0,uint8_t(1));m.put(motion,0x2c0,uint64_t(0x1000000000001));m.put(motion,0x2d8,uint64_t(0x1000000010001));
    m.put(motion,0x3a8,uint64_t(0x1000000020001));m.put(motion,0x3b0,.5);m.put(motion,0x3b8,int8_t(1));m.put(motion,0x3c8,10.0);
    m.put(motion,0x46c,200.f);m.put(motion,0x470,int32_t(10));m.put(motion,0x474,0.f);m.put(motion,0x478,10.f);
    m.put(motion,0x488,int64_t(130000000));
    pool(m,state.database+0x180,line,0x280,lineId);m.name(line,0x78,"Root");m.put(line,0xfc,int32_t(0));
    m.put(line,0x280,childId);m.put(line,0x280+0x10,lineId);m.name(line,0x280+0x78,"Child");m.put(line,0x280+0xfc,int32_t(1));
    vector(m,model,0x80,refs,{17});vector(m,line,0xc8,refs+0x100,{44});vector(m,line,0x280+0xe0,refs+0x200,{17});
    m.put(state.database+0x420,0,catalog);m.regions[catalog].resize(32);m.put(catalog,0x10,heads);m.put(catalog,0x18,uint64_t(1));
    m.put(heads,0,nodes);m.put(heads,8,uint64_t(0));
    m.regions[nodes].resize(0x88*2);m.put(nodes,0,uint64_t(17));m.name(nodes,0x18,"Express");m.put(nodes,0x80,nodes+0x88);
    m.put(nodes,0x88,uint64_t(44));m.name(nodes,0x88+0x18,"Regional");
    return m;
}
Memory compositionFixture(){
    auto m=fixture();const auto state=world();
    const auto carVector=[&](uint64_t owner,size_t offset,uint64_t storage,std::initializer_list<uint64_t> keys){
        m.put(owner,offset,storage);m.put(owner,offset+8,storage+keys.size()*32);m.put(owner,offset+16,storage+keys.size()*32);
        m.regions[storage].resize(keys.size()*32);size_t i=0;for(auto key:keys)m.put(storage,i++*32,key);
    };
    carVector(model,0xc0,configuredCars,{102,107,102});carVector(motion,8,currentCars,{107,102});
    const auto rules=state.database+0xa80;m.put(rules+0x38,0,modelHeads);m.put(rules+0x38,8,uint64_t(1289));
    m.regions[modelHeads].resize(1290*8);m.regions[modelNodes].resize(3*0x2a8);
    for(size_t i=0;i<3;++i){const uint64_t id=i==0?102:i==1?107:203;const auto offset=i*0x2a8;
        m.put(modelHeads,(id%1289)*8,modelNodes+offset);m.put(modelNodes,offset,id);
        m.name(modelNodes,offset+8,i==0?"Train_front":i==1?"Train_middle":"Unrelated");
        m.name(modelNodes,offset+0x50,i==0?"First class":"Second class");m.name(modelNodes,offset+0x70,"Regional EMU");
    }
    return m;
}
#define CHECK(condition) do{if(!(condition)){std::fprintf(stderr,"line %d: %s\n",__LINE__,#condition);return false;}}while(false)
bool estimate(){
    auto m=fixture();auto a=m.regions.at(motion),b=a;
    const auto delay=[&](int64_t now=100000000){return train_metadata::predictedDelay(a.data(),b.data(),a.size(),now);};
    auto set=[&]<class T>(size_t offset,T value){std::memcpy(a.data()+offset,&value,sizeof value);b=a;};
    CHECK(delay()==-10000000);set(0x488,int64_t(110000000));CHECK(delay()==10000000);
    set(0x488,int64_t(120000000));CHECK(delay().has_value()&&*delay()==0);
    set(0x474,1.f);CHECK(delay()==-1000000); // float 19.9 seconds truncates to 19
    set(0x474,201.f);CHECK(delay()==-20000000); // passed distance clamps to zero
    set(0x474,0.f);set(0x470,int32_t(9));CHECK(!delay());set(0x470,int32_t(10));CHECK(delay());
    set(0x46c,9.99f);CHECK(!delay());set(0x46c,10.f);CHECK(delay());
    set(0x478,.999f);CHECK(!delay());set(0x478,1.f);CHECK(delay());
    for(const auto offset:{0x46c,0x474,0x478}){
        const auto saved=a;for(const auto invalid:{std::nanf(""),INFINITY,-INFINITY}){set(offset,invalid);CHECK(!delay());}a=saved;b=a;
    }
    set(0x46c,std::numeric_limits<float>::max());CHECK(!delay());set(0x46c,10.f);
    set(0x4b0,uint8_t(0));CHECK(!delay());set(0x4b0,uint8_t(1));set(0x4d0,uint8_t(1));CHECK(!delay());set(0x4d0,uint8_t(0));
    set(0x3a8,uint64_t(0x1000000000001));CHECK(!delay());set(0x3a8,uint64_t(0x1000000010001));CHECK(!delay());
    set(0x3a8,uint64_t(0));CHECK(delay());set(0x3a8,uint64_t(0x2000000000001));CHECK(!delay());set(0x3a8,uint64_t(0));
    set(0x488,int64_t(0));CHECK(!delay());set(0x488,int64_t(1));CHECK(!delay(INT64_MAX));CHECK(!delay(-1));
    for(const auto offset:{0,0x2c0,0x2d8,0x3a8,0x46c,0x470,0x474,0x478,0x488,0x4b0,0x4d0}){
        b=a;b[offset]^=1;CHECK(!delay());
    }
    return true;
}
bool tags(){
    auto m=fixture();size_t budget=train_metadata::maximumReferences;std::vector<uint64_t> tags;
    CHECK(train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags)&&tags==std::vector<uint64_t>{17});
    // Empty is known, ID 0 is an opaque catalogue key, and duplicates are corrupt.
    vector(m,model,0x80,refs,{});CHECK(train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags)&&tags.empty());
    vector(m,model,0x80,refs,{0});CHECK(train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags)&&tags[0]==0);
    vector(m,model,0x80,refs,{17,17});CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.put(model,0,trainId+1);CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.change=model;m.changeSize=8;m.changeCall=2;CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.change=refs;m.changeSize=8;m.changeCall=2;CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.change=model+0x80;m.changeSize=24;m.changeCall=2;CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.put(model,0x80,uint64_t(0));m.put(model,0x88,uint64_t(8));m.put(model,0x90,uint64_t(8));
    CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    m=fixture();m.regions[refs].resize(4096*8);for(size_t i=0;i<4096;++i)m.put(refs,i*8,uint64_t(i));
    m.put(model,0x88,refs+4096*8);m.put(model,0x90,refs+4096*8);budget=train_metadata::maximumReferences;
    for(int i=0;i<64;++i)CHECK(train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags)&&tags.size()==4096);
    CHECK(budget==0&&!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    budget=train_metadata::maximumReferences;m.put(model,0x88,refs+4097*8);m.put(model,0x90,refs+4097*8);
    CHECK(!train_metadata::objectTags(read,&m,model,trainId,0x80,budget,tags));
    return true;
}
bool characteristics(){
    auto m=fixture();std::array<unsigned char,0x50> before{},after{};
    std::memcpy(before.data(),m.regions.at(model).data()+0xc0,before.size());after=before;
    const auto decode=[&](){return train_metadata::characteristics(before.data(),after.data(),before.size());};
    CHECK(decode().flags==255&&decode().car_count==3&&decode().passenger_capacity==320&&decode().power_w==1900000&&decode().length_m==72);
    const auto set=[&]<class T>(size_t offset,T value){std::memcpy(before.data()+offset,&value,sizeof value);after=before;};
    for(const auto offset:{0x1c,0x20,0x2c,0x30,0x34,0x38}){
        const auto saved=before;set(offset,std::nanf(""));CHECK(decode().flags!=255&&decode().car_count==3);before=saved;after=before;
        set(offset,-1.f);CHECK(decode().flags!=255);before=saved;after=before;
        set(offset,0.f);CHECK(decode().flags==255);before=saved;after=before;
    }
    const auto saved=before;set(0x1c,10001.f);CHECK(!(decode().flags&NIMBY_CHARACTERISTICS_MAX_SPEED_VALID));before=saved;after=before;
    set(0x40,int32_t(-1));CHECK(!(decode().flags&NIMBY_CHARACTERISTICS_CAPACITY_VALID));before=saved;after=before;
    set(8,refs+0x500+65537*32);set(0x10,refs+0x500+65537*32);CHECK(!(decode().flags&NIMBY_CHARACTERISTICS_CAR_COUNT_VALID));before=saved;after=before;
    for(const auto offset:{0,8,0x10,0x1c,0x20,0x2c,0x30,0x34,0x38,0x40}){after=before;after[offset]^=1;CHECK(decode().flags==0);}
    return true;
}
bool catalogue(){
    auto state=world();auto m=fixture();std::vector<NimbyTag> tags;
    CHECK(train_metadata::tagCatalog(read,&m,state,tags)&&tags.size()==2&&tags[0].tag_id==17&&std::string_view(tags[1].name_utf8)=="Regional");
    m.put(nodes,0,uint64_t(0));CHECK(train_metadata::tagCatalog(read,&m,state,tags)&&tags[0].tag_id==0);
    m=fixture();m.put(nodes,0x88+0x80,nodes);CHECK(!train_metadata::tagCatalog(read,&m,state,tags)&&tags.empty());
    m=fixture();m.put(nodes,0x88,uint64_t(17));CHECK(!train_metadata::tagCatalog(read,&m,state,tags));
    for(auto [address,size,byte]:std::array<std::array<uint64_t,3>,4>{{{nodes,0x88,0},{catalog,32,0x18},{heads,16,0},{state.database+0x420,8,0}}}){
        m=fixture();m.change=address;m.changeSize=size;m.changeCall=2;m.changeByte=byte;CHECK(!train_metadata::tagCatalog(read,&m,state,tags));
    }
    m=fixture();m.put(catalog,0x18,uint64_t(65537));CHECK(!train_metadata::tagCatalog(read,&m,state,tags));
    m=fixture();m.put(heads,0,uint64_t(0));CHECK(train_metadata::tagCatalog(read,&m,state,tags)&&tags.empty());
    m=fixture();m.put(heads,8,nodes+0x88);CHECK(train_metadata::tagCatalog(read,&m,state,tags)&&tags.size()==1);
    // A real chain at the published limit succeeds; the first extra node fails boundedly.
    m=fixture();std::erase_if(m.regions,[](const auto& item){return item.first>nodes;});m.regions[nodes].assign((train_metadata::maximumTags+1)*0x88,0);
    for(size_t i=0;i<=train_metadata::maximumTags;++i){
        const auto offset=i*0x88;m.put(nodes,offset,uint64_t(i));m.name(nodes,offset+0x18,"Tag");
        if(i+1<train_metadata::maximumTags)m.put(nodes,offset+0x80,nodes+offset+0x88);
    }
    CHECK(train_metadata::tagCatalog(read,&m,state,tags)&&tags.size()==train_metadata::maximumTags);
    m.put(nodes,(train_metadata::maximumTags-1)*0x88+0x80,nodes+train_metadata::maximumTags*0x88);
    m.reset();CHECK(!train_metadata::tagCatalog(read,&m,state,tags)&&m.reads<70000);
    state.profile=LiveStateProfile::Linux119;m.reset();CHECK(!train_metadata::tagCatalog(read,&m,state,tags)&&m.reads==0);
    return true;
}
bool integration(){
    auto m=fixture();const auto state=world();std::vector<Train> trains;TrainMetadataCatalog metadata;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&trains.size()==1&&trains[0].speed_available);
    CHECK(metadata.trains.size()==1&&metadata.trains[0].flags==NIMBY_TRAIN_PREDICTED_DELAY_VALID&&metadata.trains[0].predicted_arrival_delay_us==-10000000);
    CHECK(metadata.trains[0].configured.flags==255&&metadata.trains[0].configured.car_count==3&&metadata.trains[0].configured.maximum_speed_mps==45);
    CHECK(metadata.trains[0].current.flags==255&&metadata.trains[0].current.car_count==2&&metadata.trains[0].current.maximum_speed_mps==30);
    CHECK(metadata.lines_available&&metadata.tags_available&&metadata.lines.size()==2&&metadata.tags.size()==2);
    CHECK(metadata.lines[0].parent_line_id==0&&(metadata.lines[0].flags&NIMBY_LINE_PARENT_VALID));
    CHECK(metadata.lines[1].parent_line_id==lineId&&std::string_view(metadata.lines[1].name_utf8)=="Child");
    CHECK(metadata.tag_states.size()==3&&metadata.object_tags.size()==3);
    CHECK(std::all_of(metadata.tag_states.begin(),metadata.tag_states.end(),[](auto v){return v.available==1;}));
    // Captures own every byte; later native edits cannot mutate older results.
    m.put(refs,0,uint64_t(99));CHECK(metadata.object_tags[0].tag_id==17);
    const auto stateFor=[&](uint64_t id){return std::find_if(metadata.tag_states.begin(),metadata.tag_states.end(),[&](auto v){return v.object_id==id;});};
    m=fixture();m.fail=refs;CHECK(read_trains(read,&m,state,true,trains,false,&metadata));
    CHECK(trains[0].speed_available&&stateFor(trainId)!=metadata.tag_states.end()&&!stateFor(trainId)->available&&stateFor(lineId)->available);
    m=fixture();m.fail=catalog;CHECK(read_trains(read,&m,state,true,trains,false,&metadata));
    CHECK(!metadata.tags_available&&metadata.tags.empty()&&metadata.object_tags.size()==3&&trains[0].speed_available);
    // Reused generation, replaced pool descriptor and in-place line edits remain unknown.
    m=fixture();m.change=model;m.changeSize=8;m.changeCall=1;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!stateFor(trainId)->available&&trains[0].speed_available);
    m=fixture();m.change=state.database+0x200;m.changeSize=48;m.changeCall=4;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!stateFor(trainId)->available&&metadata.trains[0].flags==0);
    m=fixture();m.mutateOnRead=model-0x1000;m.mutateCall=4;m.mutateBase=state.database+0x200;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!stateFor(trainId)->available&&metadata.trains[0].flags==0);
    m=fixture();m.change=model-0x1000;m.changeSize=8;m.changeCall=3;m.changeMask=8;m.persist=true;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!stateFor(trainId)->available&&metadata.trains[0].flags==0&&metadata.trains[0].current.flags==0);
    m=fixture();m.change=line-0x1000;m.changeSize=8;m.changeCall=3;m.changeMask=8;m.persist=true;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!stateFor(lineId)->available&&metadata.lines[0].flags==0);
    m=fixture();m.change=line;m.changeSize=0x280;m.changeCall=1;m.changeByte=0x10;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&metadata.lines[0].flags==0&&!stateFor(lineId)->available);
    m=fixture();m.put(line,0x280+0x10,lineId+1);
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&!(metadata.lines[1].flags&NIMBY_LINE_PARENT_VALID));
    m=fixture();m.fail=state.simulation+0x28;CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&metadata.trains[0].flags==0);
    m=fixture();m.change=motion;m.changeSize=0x638;m.changeCall=1;m.changeByte=0x474;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&metadata.trains[0].flags==0&&trains[0].speed_available);
    m=fixture();m.change=model+0xc0;m.changeSize=0x50;m.changeCall=1;m.changeByte=0x1c;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&metadata.trains[0].configured.flags==0&&metadata.trains[0].current.flags==255&&trains[0].speed_available);
    m=fixture();m.change=motion;m.changeSize=0x638;m.changeCall=1;m.changeByte=0x24;
    CHECK(read_trains(read,&m,state,true,trains,false,&metadata)&&metadata.trains[0].current.flags==0&&metadata.trains[0].configured.flags==255);
    // No optional metadata traffic at all in presence-only captures.
    m=fixture();CHECK(read_trains(read,&m,state,true,trains,true));const auto presenceReads=m.reads,presenceBytes=m.bytes;
    m=fixture();m.fail=catalog;CHECK(read_trains(read,&m,state,true,trains,true,&metadata)&&m.reads==presenceReads&&m.bytes==presenceBytes);
    CHECK(metadata.trains.empty()&&metadata.lines.empty()&&metadata.tag_states.empty()&&!metadata.tags_available);
    m=fixture();CHECK(read_trains(read,&m,state,true,trains));CHECK(m.calls.find({state.database+0x420,8})==m.calls.end()&&m.calls.find({model+0x80,24})==m.calls.end());
    std::printf("train metadata: presence-only unchanged (%zu reads, %zu bytes); full optional fields isolated\n",presenceReads,presenceBytes);
    return true;
}
bool selectedQueries(){
    const auto state=world();std::vector<Train> trains;TrainMetadataCatalog metadata;
    const uint64_t path=0x300080000,passengers=0x300090000;
    const auto base=[&](){auto m=fixture();m.put(motion,0x1d0,uint8_t(1));
        m.put(motion,0x320,uint8_t(1));vector(m,motion,0x338,path,{0x1000000020001});
        m.put(state.simulation+gameLayout(state.profile).passenger_query,0,passengers);
        m.put(passengers,0x90,passengers+0x100);m.put(passengers,0x98,uint64_t(1));
        m.put(passengers+0x100,0,passengers+0x200);m.put(passengers+0x100,8,uint64_t(0));
        m.put(passengers+0x200,0,trainId);m.put(passengers+0x200,8,int32_t(73));m.put(passengers+0x200,16,uint64_t(0));return m;};
    for(const auto flags:{NIMBY_TRAIN_DATA_SERVICE,NIMBY_TRAIN_DATA_CHARACTERISTICS,NIMBY_TRAIN_DATA_TIMETABLES,
                         NIMBY_TRAIN_DATA_TAGS,NIMBY_TRAIN_DATA_PASSENGERS,NIMBY_TRAIN_DATA_LOCATIONS,NIMBY_TRAIN_DATA_LINES,NIMBY_TRAIN_DATA_COMPOSITION}){
        auto m=base();CHECK(read_trains(read,&m,state,true,trains,false,&metadata,false,flags));
        const auto touched=[&](uint64_t address,size_t bytes){return m.calls.contains({address,bytes});};
        CHECK(!touched(path,8)&&!trains[0].path_available);
        CHECK(touched(state.database+0x420,8)==bool(flags&NIMBY_TRAIN_DATA_TAGS));
        CHECK(touched(model+0x80,24)==bool(flags&NIMBY_TRAIN_DATA_TAGS));
        CHECK(touched(model+0xc0,0x50)==bool(flags&(NIMBY_TRAIN_DATA_CHARACTERISTICS|NIMBY_TRAIN_DATA_COMPOSITION)));
        CHECK(touched(state.simulation+0x28,8)==bool(flags&(NIMBY_TRAIN_DATA_SERVICE|NIMBY_TRAIN_DATA_TIMETABLES)));
        CHECK(touched(state.database+0x180,48)==bool(flags&(NIMBY_TRAIN_DATA_LINES|NIMBY_TRAIN_DATA_TAGS|NIMBY_TRAIN_DATA_TIMETABLES)));
        CHECK(touched(state.simulation+gameLayout(state.profile).passenger_query,8)==bool(flags&NIMBY_TRAIN_DATA_PASSENGERS));
        if(flags&NIMBY_TRAIN_DATA_PASSENGERS)CHECK((trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)&&trains[0].details.passenger_count==73);
        if(flags&NIMBY_TRAIN_DATA_CHARACTERISTICS)CHECK(metadata.trains[0].configured.flags==255&&metadata.trains[0].current.flags==255&&metadata.trains[0].flags==0);
        std::printf("train data flags=%u reads=%zu bytes=%zu\n",flags,m.reads,m.bytes);
    }
    auto m=base();CHECK(read_trains(read,&m,state,true,trains)&&trains[0].path_available&&m.calls.contains({path,8}));
    return true;
}
bool composition(){
    auto m=compositionFixture();const auto state=world();TrainMetadataCatalog metadata;std::vector<Train> trains;
    const auto capture=[&](){return read_trains(read,&m,state,true,trains,false,&metadata,false,NIMBY_TRAIN_DATA_COMPOSITION);};
    m.fail=modelNodes+2*0x2a8; // Unreferenced models must not be inspected.
    CHECK(capture()&&metadata.vehicles.size()==5&&metadata.models_available&&metadata.models.size()==2);
    CHECK(metadata.trains.size()==1&&metadata.trains[0].flags==0&&metadata.trains[0].configured.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID);
    CHECK(metadata.trains[0].current.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID);
    CHECK(metadata.vehicles[0].model_id==102&&metadata.vehicles[2].index==2&&metadata.vehicles[2].composition==0);
    CHECK(metadata.vehicles[3].model_id==107&&metadata.vehicles[3].index==0&&metadata.vehicles[3].composition==1);
    CHECK(std::string_view(metadata.models[0].code_utf8)=="Train_front"&&std::string_view(metadata.models[1].name_en_utf8)=="Second class");
    CHECK(std::string_view(metadata.models[0].source_name_utf8)=="Regional EMU");
    CHECK(m.calls.at({modelNodes,0x2a8})==2&&m.calls.at({modelNodes+0x2a8,0x2a8})==2);
    CHECK(!m.calls.contains({state.database+0x420,8})&&!m.calls.contains({state.database+0x180,48})&&!m.calls.contains({state.simulation+0x28,8}));
    const auto oldModels=metadata.models;m.name(modelNodes,8,"Changed");CHECK(std::string_view(oldModels[0].code_utf8)=="Train_front");
    m=compositionFixture();m.change=configuredCars;m.changeSize=3*32;m.changeCall=2;
    CHECK(capture()&&metadata.trains[0].configured.flags==0&&metadata.trains[0].current.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID&&metadata.vehicles.size()==2);
    m=compositionFixture();m.change=currentCars;m.changeSize=2*32;m.changeCall=2;
    CHECK(capture()&&metadata.trains[0].configured.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID&&metadata.trains[0].current.flags==0&&metadata.vehicles.size()==3);
    m=compositionFixture();m.change=model;m.changeSize=8;m.changeCall=4;
    CHECK(capture()&&metadata.trains[0].configured.flags==0&&metadata.trains[0].current.flags==0&&metadata.vehicles.empty());
    m=compositionFixture();m.change=modelNodes;m.changeSize=0x2a8;m.changeCall=2;m.changeByte=8;
    CHECK(capture()&&!metadata.models_available&&metadata.models.empty()&&metadata.vehicles.size()==5&&trains[0].speed_available);
    m=compositionFixture();m.put(modelHeads,107*8,uint64_t(0));
    CHECK(capture()&&metadata.models_available&&metadata.models.size()==1&&metadata.vehicles.size()==5);
    m=compositionFixture();m.put(modelNodes,0x2a0,modelNodes);
    CHECK(capture()&&!metadata.models_available&&metadata.vehicles.size()==5);
    // Missing Motion is not substituted with the configured composition.
    m=compositionFixture();m.put(motion,0,uint64_t(0));
    CHECK(capture()&&metadata.trains[0].configured.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID&&metadata.trains[0].current.flags==0&&metadata.vehicles.size()==3);
    // Empty is a known composition, without any resource table access.
    m=compositionFixture();for(const auto [owner,offset]:std::array<std::pair<uint64_t,size_t>,2>{{{model,0xc0},{motion,8}}})
        for(size_t field=0;field<24;field+=8)m.put(owner,offset+field,uint64_t(0));
    CHECK(capture()&&metadata.vehicles.empty()&&metadata.models_available&&metadata.trains[0].configured.flags==NIMBY_CHARACTERISTICS_COMPOSITION_VALID);
    CHECK(!m.calls.contains({state.database+0xa80+0x38,16}));
    // Resolving 10,000 repeated vehicles reads exactly the same two models.
    m=compositionFixture();std::vector<NimbyTrainVehicle> repeated(10000);for(size_t i=0;i<repeated.size();++i)repeated[i].model_id=i%2?102:107;
    std::vector<NimbyVehicleModel> models;
    CHECK(train_metadata::vehicleModels(read,&m,state,repeated,models)&&models.size()==2&&m.calls.at({modelNodes,0x2a8})==2);
    std::printf("composition lookup: %zu cars, %zu distinct models, %zu remote reads\n",repeated.size(),models.size(),m.reads);
    for(const auto [address,size]:std::array<std::pair<uint64_t,size_t>,3>{{{state.database+0xa80+0x38,16},{modelHeads+102*8,8},{modelHeads+1289*8,8}}}){
        m=compositionFixture();m.change=address;m.changeSize=size;m.changeCall=2;
        CHECK(!train_metadata::vehicleModels(read,&m,state,repeated,models)&&models.empty());
    }
    repeated.resize(train_metadata::maximumModels+1);for(size_t i=0;i<repeated.size();++i)repeated[i].model_id=i;
    m=compositionFixture();CHECK(!train_metadata::vehicleModels(read,&m,state,repeated,models)&&m.reads==0);
    // Per-composition and total budgets apply before any unbounded remote read.
    m=compositionFixture();m.regions[configuredCars].assign(4096*32,0);m.put(model,0xc8,configuredCars+4096*32);m.put(model,0xd0,configuredCars+4096*32);
    std::array<unsigned char,0x50> expected{};std::memcpy(expected.data(),m.regions[model].data()+0xc0,expected.size());
    size_t budget=train_metadata::maximumCars;std::vector<NimbyTrainVehicle> cars;
    for(int i=0;i<64;++i)CHECK(train_metadata::cars(read,&m,model,trainId,0xc0,expected.data(),0,budget,cars)&&cars.size()==4096);
    CHECK(budget==0&&!train_metadata::cars(read,&m,model,trainId,0xc0,expected.data(),0,budget,cars));
    m.put(model,0xc8,configuredCars+4097*32);m.put(model,0xd0,configuredCars+4097*32);std::memcpy(expected.data(),m.regions[model].data()+0xc0,expected.size());
    budget=train_metadata::maximumCars;CHECK(!train_metadata::cars(read,&m,model,trainId,0xc0,expected.data(),0,budget,cars));
    return true;
}
bool locationsOnly(){
    const auto state=world();auto m=fixture();std::vector<Train> trains;TrainMetadataCatalog metadata;
    const auto capture=[&](){return read_trains(read,&m,state,true,trains,false,&metadata,false,NIMBY_TRAIN_DATA_LOCATIONS);};
    const auto absentHeavyReads=[&](){return !m.calls.contains({state.simulation+0x28,8})&&
        !m.calls.contains({state.database+0x180,48})&&!m.calls.contains({state.database+0x420,8})&&
        !m.calls.contains({state.simulation+gameLayout(state.profile).passenger_query,8})&&
        !m.calls.contains({model,0xbc})&&!m.calls.contains({model+0xc0,0x50});};
    for(const auto [flag,offset]:std::array<std::pair<size_t,size_t>,3>{{{0x218,0x1f8},{0x1d0,0xb8},{0x4b0,0x3a8}}}){
        m=fixture();m.put(motion,0x4b0,uint8_t(0));m.put(motion,flag,uint8_t(1));
        m.put(motion,offset,uint64_t(0x1000000030001));m.put(motion,offset+8,.4);m.put(motion,offset+16,int8_t(-1));
        CHECK(capture()&&trains.size()==1&&(trains[0].service.flags&NIMBY_SERVICE_LOCATION_VALID));
        CHECK(trains[0].service.location_track_id==0x1000000030001&&!(trains[0].service.flags&NIMBY_SERVICE_STATE_VALID));
        CHECK(absentHeavyReads());
        const auto stableReads=m.reads;
        m.reset();m.change=motion;m.changeSize=0x638;m.changeCall=1;m.changeByte=offset;
        CHECK(capture()&&!(trains[0].service.flags&NIMBY_SERVICE_LOCATION_VALID)&&m.reads==stableReads);
        m.reset();m.changeByte=flag;
        CHECK(capture()&&!(trains[0].service.flags&NIMBY_SERVICE_LOCATION_VALID));
    }
    // Residual position storage is unavailable when all presence optionals are absent.
    m=fixture();m.put(motion,0x4b0,uint8_t(0));
    CHECK(capture()&&!(trains[0].service.flags&NIMBY_SERVICE_LOCATION_VALID)&&absentHeavyReads());
    std::puts("locations-only: Drive, Presence and Blackhole observed without service/calendar/catalogue reads");
    return true;
}
}
int main(){const auto start=std::chrono::steady_clock::now();if(!estimate()||!characteristics()||!tags()||!catalogue()||!integration()||!selectedQueries()||!composition()||!locationsOnly())return 1;
    std::printf("train metadata thresholds, 262144 references, 16384 tags, corruption and generation tests passed in %.1f ms\n",std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());}
