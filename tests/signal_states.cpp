// Private fixture: seed owned snapshots without depending on a running game.
#include "../src/runtime/observation.cpp"
#include <cstdio>
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;} } while(false)
int main() {
    nimby::engine::Signal native{};native.id=0x8000000000001;native.kind=4;
    auto absent=signal_render_state(native,true,{});
    CHECK(absent.texture_state==0&&(absent.flags&NIMBY_SIGNAL_TEXTURE_STATE_VALID));
    CHECK(absent.flags&NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT);
    CHECK(!(absent.flags&NIMBY_SIGNAL_ASPECT_VALID));
    CHECK(signal_render_state(native,false,{}).flags==0);
    const std::map<uint64_t,int32_t> other{{native.id+1,10}};
    CHECK(signal_render_state(native,true,other).flags&NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT);
    for(int selector:{0,9,10,-1}){
        const auto explicit_state=signal_render_state(native,true,{{native.id,selector}});
        CHECK(explicit_state.texture_state==selector);
        CHECK(!(explicit_state.flags&NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT));
        CHECK(explicit_state.flags&NIMBY_SIGNAL_TEXTURE_STATE_VALID);
        CHECK(signal_render_state(native,false,{{native.id,selector}}).flags==0);
    }
    Snapshot fixture;
    NimbySignalState unknown{};unknown.signal_id=0x8000000000001;
    NimbySignalState specific{};specific.signal_id=0x8000000010001;
    specific.flags=NIMBY_SIGNAL_ASPECT_VALID|NIMBY_SIGNAL_SPECIFIC_STATE_VALID|NIMBY_SIGNAL_TEXTURE_STATE_VALID;
    specific.aspect=NIMBY_SIGNAL_ASPECT_STOP;specific.texture_state=0;
    std::strcpy(specific.system_utf8,"test");std::strcpy(specific.specific_state_utf8,"stop");
    fixture.signal_states={unknown,specific};
    NimbySignalTexture texture{};texture.signal_id=specific.signal_id;
    texture.flags=NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID;texture.selected_index=10;texture.state_count=16;
    std::strcpy(texture.relative_path_utf8,"imgs/ca/sem_bal/tex10.svg");
    fixture.signal_textures={texture,texture};
    registry().snapshots.emplace(123,std::move(fixture));
    registry().snapshots.emplace(124,Snapshot{});
    uint32_t count=99;
    CHECK(NimbyInternal_CopySignalStates(123,nullptr,0,&count)==NIMBY_OK&&count==2);
    NimbySignalState output[2];std::memset(output,0x5a,sizeof output);
    unsigned char before[sizeof output];std::memcpy(before,output,sizeof output);
    CHECK(NimbyInternal_CopySignalStates(123,output,1,&count)==NIMBY_BUFFER_TOO_SMALL&&count==2);
    CHECK(std::memcmp(before,output,sizeof output)==0);
    CHECK(NimbyInternal_CopySignalStates(123,output,2,&count)==NIMBY_OK&&count==2);
    CHECK(std::memcmp(&output[0],&unknown,sizeof unknown)==0);
    CHECK(std::memcmp(&output[1],&specific,sizeof specific)==0);
    output[1].aspect=NIMBY_SIGNAL_ASPECT_PROCEED;
    CHECK(NimbyInternal_CopySignalStates(123,output,2,&count)==NIMBY_OK&&output[1].aspect==NIMBY_SIGNAL_ASPECT_STOP);
    CHECK(NimbyInternal_CopySignalStates(124,nullptr,0,&count)==NIMBY_OK&&count==0);
    CHECK(NimbyInternal_CopySignalTextures(123,nullptr,0,&count)==NIMBY_OK&&count==2);
    NimbySignalTexture textures[2]{};
    CHECK(NimbyInternal_CopySignalTextures(123,textures,1,&count)==NIMBY_BUFFER_TOO_SMALL&&count==2);
    CHECK(textures[0].signal_id==0);
    CHECK(NimbyInternal_CopySignalTextures(123,textures,2,&count)==NIMBY_OK&&count==2);
    CHECK(std::memcmp(&textures[0],&texture,sizeof texture)==0);
    textures[0].selected_index=0;
    CHECK(NimbyInternal_CopySignalTextures(123,textures,2,&count)==NIMBY_OK&&textures[0].selected_index==10);
    CHECK(NimbyInternal_CopySignalTextures(124,nullptr,0,&count)==NIMBY_OK&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(123)==NIMBY_OK);
    CHECK(NimbyInternal_CopySignalStates(123,output,2,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopySignalTextures(123,textures,2,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(124)==NIMBY_OK);
    std::puts("Signal states: unknown, specific state, valid zero texture, empty snapshot, buffers and ownership passed.");
}
