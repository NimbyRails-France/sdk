#include "platform/linux/process.h"
#include "engine/live_state.h"
#include "engine/network.h"
#include "engine/track_usage.h"
#include "engine/signal_textures.h"
#include "engine/versioning.h"
#include <iostream>
int main(int argc,char** argv) {
    try {
        if(argc!=2){std::cerr<<"Usage: nimby_linux_inspect PID\n";return 2;}
        nimby::platform::linux_os::Process process(std::stoi(argv[1]));
        const auto image=process.executable();const auto identity=nimby::platform::linux_os::identify(image);
        const auto base=process.image_base();unsigned char header[4]{};const auto readable=process.read(base,header,sizeof header);
        std::cout<<"executable="<<image<<"\nsha256="<<identity.sha256<<"\nsize="<<identity.size<<"\nbase="<<std::hex<<base<<std::dec
            <<"\nreadable="<<readable<<"\ngame_profile=unvalidated\n";
        const bool known_roots = identity.size == 20787376 && identity.sha256 ==
            "2581d0e8157f43acb137b2bd9d52e2a7c82bd8af8b62fab5d87f00cc27eefde6";
        nimby::engine::LiveState state;
        const auto reader = [](void* context, uint64_t address, void* out, size_t size) {
            return static_cast<nimby::platform::linux_os::Process*>(context)->read(address, out, size);
        };
        const auto resolved = nimby::engine::resolve_live_state(reader, &process, base, known_roots,
            nimby::engine::LiveStateProfile::Linux119, state);
        std::cout << "roots_profile=" << (known_roots ? "linux-1.19.10-research" : "unsupported")
            << "\nroots_available=" << resolved << '\n';
        if (resolved) std::cout << std::hex << "root=" << state.root << "\ndatabase=" << state.database
            << "\ncopy=" << state.copy << "\nsimulation=" << state.simulation << '\n';
        if (resolved) {
            nimby::engine::Network network;
            std::vector<nimby::engine::Train> trains;
            const bool network_ok = nimby::engine::read_network(reader,&process,state,true,network);
            const bool trains_ok = nimby::engine::read_trains(reader,&process,state,true,trains);
            std::cout << std::dec << "network_available=" << network_ok << "\ntrains_available=" << trains_ok
                << "\ntracks=" << network.tracks.size() << "\nstations=" << network.stations.size()
                << "\nsignals=" << network.signals.size() << "\ntrains=" << trains.size() << '\n';
            for (size_t i=0;i<std::min<size_t>(trains.size(),3);++i)
                std::cout << "train=" << trains[i].id << ' ' << trains[i].name << '\n';
            std::vector<nimby::engine::TrackUsage> reservations, occupations;
            std::vector<nimby::engine::SignalTextureState> selectors;
            nimby::engine::SignalTextureCatalog textures;
            nimby::engine::VersioningObservation version;
            std::cout << "reservations_available=" << nimby::engine::read_reservations(reader,&process,state,true,reservations)
                << " count=" << reservations.size() << '\n'
                << "occupations_available=" << nimby::engine::read_occupations(reader,&process,state,true,occupations)
                << " count=" << occupations.size() << '\n'
                << "selectors_available=" << nimby::engine::read_signal_texture_states(reader,&process,state,true,selectors)
                << " count=" << selectors.size() << '\n'
                << "textures_available=" << nimby::engine::read_signal_texture_catalog(reader,&process,state,true,textures)
                << " count=" << textures.sets.size() << " mod_root=" << textures.local_mod_root << '\n'
                << "version_available=" << nimby::engine::read_versioning_observation(reader,&process,base,true,version,state.profile) << '\n';
        }
        return readable?0:3;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
