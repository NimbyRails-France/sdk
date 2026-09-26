#include <nimby/detail/signal_settings_catalog.hpp>
#include <charconv>
#include <cstdio>
int main(int argc,char** argv){try{
    if(argc!=2){std::puts("Usage: nimby_signal_settings_probe PID");return 1;}
    uint32_t pid{};const std::string_view text=argv[1];
    const auto [end,error]=std::from_chars(text.data(),text.data()+text.size(),pid);
    if(error!=std::errc{}||end!=text.data()+text.size()||!pid)return 1;
    auto client=nimby::detail::ObservationSession(pid);auto snapshot=client.capture();
    const auto& game=snapshot->getGameSession();
    if(game)std::printf("world=%s generation=%llu\n",game->worldId.c_str(),static_cast<unsigned long long>(game->generation));
    else std::puts("world=unavailable");
    auto catalog=nimby::detail::signalSettingsCatalog(*snapshot);
    size_t unresolved=0,sfr=0;
    for(const auto& row:snapshot->getAllSignalTextures()){
        const auto reference=row.getReference();
        if(!reference)++unresolved;
        else if(reference->getTexturesId()=="sfr_bal_a_cpp_v1")++sfr;
    }
    std::printf("signals=%zu texture_rows=%zu unresolved=%zu sfr=%zu complete_catalog=%s\n",
        snapshot->getAllSignals().size(),snapshot->getAllSignalTextures().size(),unresolved,sfr,catalog?"yes":"no");
    return game&&catalog?0:2;
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 3;}}
