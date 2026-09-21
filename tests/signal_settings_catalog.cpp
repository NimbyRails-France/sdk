#include <nimby/detail/signal_settings_catalog.hpp>
#include <cstdio>
#include <cstring>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Failed line %d\n",__LINE__);return 1;}}while(false)
int main(){
    constexpr uint64_t first=0x8000000000001,second=0x8000000000002;
    std::vector<nimby::Signal> signals{nimby::Signal{{first,0,0,1,4}},nimby::Signal{{second,0,0,1,4}}};
    NimbySignalTexture a{},b{};a.signal_id=first;b.signal_id=second;
    a.flags=b.flags=NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID;
    std::strcpy(a.textures_id_utf8,"sfr_bal_a_cpp_v1");std::strcpy(b.textures_id_utf8,"builtin");
    auto collect=[&](std::vector<nimby::SignalTexture> rows){return nimby::detail::signalSettingsCatalog(signals,rows);};
    auto valid=collect({nimby::SignalTexture{b},nimby::SignalTexture{a}});
    CHECK(valid&&valid->size()==2&&valid->at(0).id==first&&valid->at(0).textureSet=="sfr_bal_a_cpp_v1");
    CHECK(!collect({nimby::SignalTexture{a}}));
    CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{a}}));
    auto missing=b;missing.flags=0;
    CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{missing}}));
    auto wrong=b;wrong.signal_id=0x8000000000003;
    CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{wrong}}));
    wrong=b;wrong.textures_id_utf8[0]=0;
    CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{wrong}}));
    wrong=b;std::memset(wrong.textures_id_utf8,'x',sizeof(wrong.textures_id_utf8));
    CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{wrong}}));
    const nimby::SignalTexture validTexture{a}, invalidTexture{missing};
    CHECK(validTexture.getTexturesIdView()=="sfr_bal_a_cpp_v1");
    CHECK(!invalidTexture.getTexturesIdView());
    signals[1]=signals[0];CHECK(!collect({nimby::SignalTexture{a},nimby::SignalTexture{b}}));
    signals.clear();auto empty=collect({});CHECK(empty&&empty->empty());
    std::puts("PASS complete signal settings catalog; missing and ambiguous rows rejected");
}
