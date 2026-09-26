// Deliberately small command host. The mod remains the decision-maker; this
// process only submits versioned requests to the explicit game PID.
#include <nimby/detail/observation_session.hpp>
#include <nimby/detail/control.h>
#include <charconv>
#include <iostream>
#include <limits>
#include <cmath>
namespace {
template<class T> T number(const char* text){
    const std::string_view value=text;T result{};
    const auto parsed=std::from_chars(value.data(),value.data()+value.size(),result);
    if(parsed.ec!=std::errc{}||parsed.ptr!=value.data()+value.size())throw std::invalid_argument("Invalid numeric argument");
    return result;
}
void clock(const nimby::SimulationClock& value,uint32_t count=0){
    std::cout<<"{\"epoch_seconds\":"<<value.getEpochSeconds()<<",\"ticks\":"<<(value.getElapsedTime().count()/10)<<",\"interventions\":"<<count<<"}\n";
}
}
int main(int argc,char** argv){try{
    const std::string_view action=argc>1?argv[1]:"help";
    if(action=="help"){
        std::cout<<"SDK command host. IDs and generation/owner tokens use decimal integers. Speeds use m/s.\n"
            "  clock-read PID\n  clock-set PID UTC_SECONDS [recalculate]\n"
            "  status PID MOD\n"
            "  acquire|renew PID MOD GENERATION OWNER LEASE_MS\n"
            "  release|clear PID MOD GENERATION OWNER\n"
            "  force-signal PID MOD GENERATION OWNER SIGNAL ASPECT_CODE\n"
            "  restore-signal|restore-train PID MOD GENERATION OWNER OBJECT\n"
            "  setting PID MOD GENERATION OWNER SIGNAL INDEX BOOL\n"
            "  restore-setting PID MOD GENERATION OWNER SIGNAL INDEX\n"
            "  train PID MOD GENERATION OWNER TRAIN SPEED_MPS MODE EXIT_SIGNAL REAR\n"
            "    MODE: 0 speed ceiling, 1 physical clearance, 2 stop. EXIT_SIGNAL=0: next signal. REAR: 0 or 1.\n"
            "  read-signal|read-train PID MOD GENERATION OBJECT\n"
            "Mutations are never retried. Renew the lease explicitly. A timeout has an uncertain outcome.\n";
        return 0;
    }
    if(argc<3)throw std::invalid_argument("Missing PID");
    const auto pid=number<uint32_t>(argv[2]);if(!pid)throw std::invalid_argument("PID must be positive");
    if(action=="clock-read"){
        if(argc!=3)throw std::invalid_argument("clock-read PID");
        auto client=nimby::detail::ObservationSession(pid);auto value=client.captureSignalling()->getSimulationClock();
        if(!value)throw std::runtime_error("Simulation clock unavailable");
        clock(*value);return 0;
    }
    if(action=="clock-set"){
        if(argc!=4&&argc!=5)throw std::invalid_argument("clock-set PID UTC_SECONDS [recalculate]");
        const auto utc=std::chrono::sys_seconds{std::chrono::seconds{number<int64_t>(argv[3])}};
        if(argc==5&&std::string_view(argv[4])!="recalculate")throw std::invalid_argument("Unknown clock option");
        auto client=nimby::detail::ObservationSession(pid);
        if(argc==5){auto value=client.setSimulationDateTimeAndRecalculateTrains(utc);clock(value.clock,value.interventions);}
        else clock(client.setSimulationDateTime(utc));
        return 0;
    }
    if(argc<4)throw std::invalid_argument("Missing mod ID");
    NimbyControlRequest r{};r.size=sizeof r;r.version=NIMBY_CONTROL_VERSION;
    if(action=="status"&&argc==4)r.operation=NIMBY_CONTROL_STATUS;
    else if((action=="read-signal"||action=="read-train")&&argc==6){
        r.operation=action=="read-signal"?NIMBY_CONTROL_READ_SIGNAL:NIMBY_CONTROL_READ_TRAIN;
        r.generation=number<uint64_t>(argv[4]);r.object=number<uint64_t>(argv[5]);
    }else{
        if(argc<6)throw std::invalid_argument("Missing generation/owner; see help");
        r.generation=number<uint64_t>(argv[4]);r.owner=number<uint64_t>(argv[5]);
        if((action=="acquire"||action=="renew")&&argc==7){r.operation=action=="acquire"?1:2;r.lease_ms=number<uint32_t>(argv[6]);}
        else if((action=="release"||action=="clear")&&argc==6)r.operation=action=="release"?3:10;
        else if(action=="force-signal"&&argc==8){r.operation=4;r.object=number<uint64_t>(argv[6]);r.value=number<int32_t>(argv[7]);}
        else if((action=="restore-signal"||action=="restore-train")&&argc==7){r.operation=action=="restore-signal"?5:7;r.object=number<uint64_t>(argv[6]);}
        else if(action=="setting"&&argc==9){r.operation=8;r.object=number<uint64_t>(argv[6]);r.index=number<int32_t>(argv[7]);r.value=number<int32_t>(argv[8]);}
        else if(action=="restore-setting"&&argc==8){r.operation=9;r.object=number<uint64_t>(argv[6]);r.index=number<int32_t>(argv[7]);}
        else if(action=="train"&&argc==11){
            r.operation=6;r.object=number<uint64_t>(argv[6]);r.speed_mps=number<double>(argv[7]);
            r.mode=number<uint32_t>(argv[8]);r.exit_signal=number<uint64_t>(argv[9]);r.flags=number<uint32_t>(argv[10]);
            if(!std::isfinite(r.speed_mps))throw std::invalid_argument("Non-finite speed");
        }else throw std::invalid_argument("Unknown command or argument count; see help");
    }
    NimbyControlResponse out{};out.size=sizeof out;
    const auto status=NimbyInternal_ModControl(pid,argv[3],&r,&out);
    std::cout<<"{\"result\":"<<status<<",\"generation\":\""<<out.generation<<"\",\"remaining_ms\":"<<out.remaining_ms
        <<",\"capabilities\":"<<out.capabilities<<",\"signals\":"<<out.signal_count<<",\"trains\":"<<out.train_count
        <<",\"settings\":"<<out.setting_count<<",\"active\":"<<out.active<<",\"aspect\":"<<out.aspect
        <<",\"reason\":"<<out.reason<<",\"speed_mps\":"<<out.speed_mps<<",\"exit_signal\":\""<<out.exit_signal<<"\"}\n";
    return status?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
