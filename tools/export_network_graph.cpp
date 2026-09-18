// Read-only JSON export using the installed public SDK. IDs stay strings for JS.
#include <nimby/client.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
static std::string quote(const std::string& s){
    std::ostringstream o;o<<'"';for(unsigned char c:s){
        if(c=='"'||c=='\\')o<<'\\'<<c;
        else if(c<32)o<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<unsigned(c)<<std::dec;
        else o<<c;
    }o<<'"';return o.str();
}
static std::string id(nimby::Id n){std::ostringstream o;o<<"\"0x"<<std::hex<<n<<'"';return o.str();}
int main(int argc,char** argv){try{
    if(argc!=2&&argc!=6){std::cerr<<"Usage: export_network_graph output.json [xmin ymin xmax ymax]\n";return 1;}
    auto client=nimby::Client::connect();nimby::Snapshot::Ptr s;
    for(int attempt=0;attempt<5&&!s;++attempt){
        try{s=client.capture();}catch(const nimby::Exception&){if(attempt==4)throw;std::this_thread::sleep_for(std::chrono::milliseconds(300));}
    }
    std::ofstream o(argv[1],std::ios::binary);if(!o)throw std::runtime_error("Cannot create output");
    o<<std::setprecision(17)<<"{\"schema\":1,\"source\":\"NimbyRailsFranceSDK\",\"processId\":"<<s->getProcessId()
     <<",\"capturedUnixMs\":"<<std::chrono::duration_cast<std::chrono::milliseconds>(s->getCapturedAt().time_since_epoch()).count()
     <<",\"gameSha256\":"<<quote(s->getGameSha256())<<",\"stations\":[";
    bool comma=false;for(auto& st:s->getAllStations()){if(comma)o<<',';comma=true;o<<"{\"id\":"<<id(st.getId())<<",\"name\":"<<quote(st.getName().value_or(""))<<'}';}
    o<<"],\"tracks\":[";comma=false;
    for(auto& t:s->getAllTrackNodes()){
        if(comma)o<<',';comma=true;auto xy=t.getCoordinates();auto tr=s->getTrackById(t.getId());
        o<<"{\"id\":"<<id(t.getId())<<",\"a\":"<<id(t.getLinkAId().value_or(0))<<",\"b\":"<<id(t.getLinkBId().value_or(0))
         <<",\"x\":"<<xy.x<<",\"y\":"<<xy.y<<",\"station\":"<<id(tr?tr->getStationId().value_or(0):0)<<'}';
    }
    o<<"],\"junctions\":[";comma=false;
    for(auto& j:s->getAllTrackJunctions()){
        if(comma)o<<',';comma=true;o<<"{\"branch\":"<<id(j.getBranchTrackId())<<",\"main\":"<<id(j.getMainTrackId())
         <<",\"fraction\":"<<j.getMainFraction()<<",\"mainDirection\":"<<j.getMainDirection()<<",\"branchDirection\":"<<j.getBranchDirection()<<'}';
    }
    o<<"],\"signals\":[";comma=false;
    for(auto& g:s->getAllSignals()){
        if(comma)o<<',';comma=true;auto state=s->getSignalStateById(g.getId());
        o<<"{\"id\":"<<id(g.getId())<<",\"decimalId\":"<<quote(std::to_string(g.getId()))<<",\"track\":"<<id(g.getTrackId())
         <<",\"fraction\":"<<g.getFraction()<<",\"direction\":"<<g.getDirection()<<",\"kind\":"<<g.getKind()
         <<",\"exceptionCount\":"<<(state&&state->getExceptionCount()?std::to_string(*state->getExceptionCount()):"null")
         <<",\"ignoredByDefault\":"<<(state&&state->isIgnoredByDefault()?(*state->isIgnoredByDefault()?"true":"false"):"null")
         <<",\"aspect\":"<<(state&&state->getAspect()?std::to_string(*state->getAspect()):"null")
         <<",\"textureState\":"<<(state&&state->getTextureSelector()?std::to_string(*state->getTextureSelector()):"null")
         <<",\"textureDefault\":"<<(state&&state->usesDefaultTextureSelector()?"true":"false")<<'}';
    }o<<']';
    if(argc==6){
        const double xmin=std::stod(argv[2]),ymin=std::stod(argv[3]),xmax=std::stod(argv[4]),ymax=std::stod(argv[5]);
        std::vector<nimby::TrackNode> nodes;std::vector<nimby::Signal> signals;std::vector<nimby::TrackJunction> junctions;
        std::set<nimby::Id> included;
        for(const auto& n:s->getAllTrackNodes()){auto c=n.getCoordinates();if(c.x>=xmin&&c.x<=xmax&&c.y>=ymin&&c.y<=ymax){nodes.push_back(n);included.insert(n.getId());}}
        for(const auto& g:s->getAllSignals())if(included.contains(g.getTrackId()))signals.push_back(g);
        for(const auto& j:s->getAllTrackJunctions())if(included.contains(j.getMainTrackId())&&included.contains(j.getBranchTrackId()))junctions.push_back(j);
        nimby::SignalTopology topology(signals,nodes,junctions);
        o<<",\"regionBounds\":["<<xmin<<','<<ymin<<','<<xmax<<','<<ymax<<"],\"nextSignals\":[";comma=false;
        for(const auto& g:signals)for(int direction:{-1,1}){
            auto next=topology.findNextSignals(nimby::Position{g.getTrackId(),g.getFraction(),direction},16384,false,g.getId());
            if(comma)o<<',';comma=true;
            o<<"{\"signal\":"<<id(g.getId())<<",\"direction\":"<<direction<<",\"incomplete\":"<<(next.incomplete?"true":"false")
             <<",\"truncated\":"<<(next.truncated?"true":"false")<<",\"explored\":"<<next.exploredSections<<",\"next\":[";
            bool sep=false;for(const auto& dest:next.nextSignals){if(sep)o<<',';sep=true;o<<id(dest.getId());}o<<"]}";
        }o<<']';
    }
    o<<"}\n";o.close();if(!o)throw std::runtime_error("Export write failed");
    std::cout<<"Exported "<<s->getAllTrackNodes().size()<<" tracks, "<<s->getAllTrackJunctions().size()<<" junctions, "<<s->getAllSignals().size()<<" signals\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
